/***************************************************************************
 *            mqtt.hpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Opera.
 *
 *  Opera is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Opera is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Opera.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OPERA_MQTT_HPP
#define OPERA_MQTT_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <tuple>
#include <thread>
#include <mosquitto.h>

#include "broker.hpp"
#include "serialisation.hpp"
#include "deserialisation.hpp"

namespace Opera {

static const std::string OPERA_PRESENTATION_TOPIC = "opera/presentation";
static const std::string OPERA_STATE_TOPIC = "opera/state";
static const std::string OPERA_COLLISION_NOTIFICATION_TOPIC = "opera/collision-notification";

//! \brief The publisher of objects to the MQTT broker
template<class T> class MqttPublisherBase : public PublisherInterface<T> {
  protected:
    MqttPublisherBase(std::string const& topic, std::string const& hostname, int port) : _topic(topic) {
        int rc;

        /* Create a new client instance.
         * id = NULL -> ask the broker to generate a client id for us
         * clean session = true -> the broker should remove old sessions when we connect
         * obj = NULL -> we aren't passing any of our private data for callbacks
         */
        _mosquitto_publisher = mosquitto_new(NULL, true, NULL);
        ARIADNE_ASSERT_MSG(_mosquitto_publisher != NULL, "Error: Out of memory.")

        /* Connect to test.mosquitto.org on port 1883, with a keepalive of 60 seconds.
         * This call makes the socket connection only, it does not complete the MQTT
         * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
         * mosquitto_loop_forever() for processing net traffic. */
        rc = mosquitto_connect(_mosquitto_publisher, hostname.c_str(), port, 60);
        if (rc != MOSQ_ERR_SUCCESS){
            mosquitto_destroy(_mosquitto_publisher);
            ARIADNE_ERROR("Error: " << mosquitto_strerror(rc))
        }

        /* Run the network loop in a background thread, this call returns quickly. */
        rc = mosquitto_loop_start(_mosquitto_publisher);
        if (rc != MOSQ_ERR_SUCCESS){
            mosquitto_destroy(_mosquitto_publisher);
            ARIADNE_ERROR("Error: " << mosquitto_strerror(rc))
        }
    }

    void put(T const& obj) override {
        const char* payload = serialise(obj).c_str();
        int rc = mosquitto_publish(_mosquitto_publisher, NULL, _topic.c_str(), strlen(payload), payload, 2, false);
        ARIADNE_ASSERT_MSG(rc == MOSQ_ERR_SUCCESS,"Error publishing: " << mosquitto_strerror(rc))
    }

    virtual std::string serialise(T const& obj) const = 0;

    ~MqttPublisherBase() {
        delete _mosquitto_publisher;
    }

  private:
    std::string const _topic;
    struct mosquitto* _mosquitto_publisher;
};

class BodyPresentationPacketMqttPublisher : public MqttPublisherBase<BodyPresentationPacket> {
public:
    BodyPresentationPacketMqttPublisher(std::string const& hostname, int port);
    std::string serialise(BodyPresentationPacket const& obj) const override;
};

class BodyStatePacketMqttPublisher : public MqttPublisherBase<BodyStatePacket> {
public:
    BodyStatePacketMqttPublisher(std::string const& hostname, int port);
    std::string serialise(BodyStatePacket const& obj) const override;
};

class CollisionNotificationPacketMqttPublisher : public MqttPublisherBase<CollisionNotificationPacket> {
public:
    CollisionNotificationPacketMqttPublisher(std::string const& hostname, int port);
    std::string serialise(CollisionNotificationPacket const& obj) const override;
};

void subscriber_on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
    if(reason_code != 0){
        mosquitto_disconnect(mosq);
    }
    std::cout << "callback on connect" << std::endl;
    auto topic = static_cast<std::string*>(obj);
    std::cout << "topic: " << *topic << std::endl;

    int rc = mosquitto_subscribe(mosq, NULL, topic->c_str(), 1);
    if(rc != MOSQ_ERR_SUCCESS){
        mosquitto_disconnect(mosq);
        fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
    }
}

template<class T> class CallbackData {
    std::string const& topic;
    T const&
};

void subscriber_on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {

}

//! \brief The subscriber to objects published to MQTT
template<class T> class MqttSubscriberBase : public SubscriberInterface<T> {
    typedef std::function<void(T const&)> FunctionType;
  protected:
    //! \brief Set the next index and make sure that _stop is false
    MqttSubscriberBase(std::string const& topic, std::string const& hostname, int port) : _topic(topic), _hostname(hostname), _port(port), _started(false) {
    }

  public:
    //! \brief The main asynchronous loop for getting objects from memory
    //! \details Allows multiple calls, waiting to finish the previous callback if necessary
    void loop_get(FunctionType const& callback) override {
        if (_started) {
            mosquitto_disconnect(_subscriber);
        } else {
            _subscriber = mosquitto_new(NULL, true, (void*)&_topic);
            ARIADNE_ASSERT_MSG(_subscriber != NULL,"Error: out of memory.")
        }

        mosquitto_connect_callback_set(_subscriber, subscriber_on_connect);
        mosquitto_message_callback_set(_subscriber, subscriber_on_message);

        /* This call makes the socket connection only, it does not complete the MQTT
         * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
         * mosquitto_loop_forever() for processing net traffic. */
        int rc = mosquitto_connect(_subscriber, _hostname.c_str(), _port, 60);
        if (rc != MOSQ_ERR_SUCCESS) {
            mosquitto_destroy(_subscriber);
            ARIADNE_ERROR("Error connecting: " << mosquitto_strerror(rc))
        }

        rc = mosquitto_subscribe(_subscriber, NULL, _topic.c_str(), 1);
        if (rc != MOSQ_ERR_SUCCESS) {
            mosquitto_destroy(_subscriber);
            ARIADNE_ERROR("Error subscribing: " << mosquitto_strerror(rc))
        }

        mosquitto_loop_start(_subscriber);
        _started = true;
    }

    virtual ~MqttSubscriberBase() {
        mosquitto_disconnect(_subscriber);
    }

  protected:
    struct mosquitto* _subscriber;
    std::string const _topic;
    std::string const _hostname;
    int const _port;
    bool _started;
};

class BodyPresentationPacketMqttSubscriber : public MqttSubscriberBase<BodyPresentationPacket> {
  public:
    BodyPresentationPacketMqttSubscriber(std::string const& hostname, int port);
};

class BodyStatePacketMqttSubscriber : public MqttSubscriberBase<BodyStatePacket> {
  public:
    BodyStatePacketMqttSubscriber(std::string const& hostname, int port);
};

class CollisionNotificationPacketMqttSubscriber : public MqttSubscriberBase<CollisionNotificationPacket> {
  public:
    CollisionNotificationPacketMqttSubscriber(std::string const& hostname, int port);
};

//! \brief A broker to handle packets using MQTT
class MqttBrokerAccess : public BrokerAccessInterface {
  public:
    MqttBrokerAccess(std::string const& hostname, int port);
    PublisherInterface<BodyPresentationPacket>* body_presentation_publisher() const override;
    PublisherInterface<BodyStatePacket>* body_state_publisher() const override;
    PublisherInterface<CollisionNotificationPacket>* collision_notification_publisher() const override;
    SubscriberInterface<BodyPresentationPacket>* body_presentation_subscriber() const override;
    SubscriberInterface<BodyStatePacket>* body_state_subscriber() const override;
    SubscriberInterface<CollisionNotificationPacket>* collision_notification_subscriber() const override;
    ~MqttBrokerAccess();

  private:
    std::string const _hostname;
    int const _port;
};

}

#endif // OPERA_MQTT_HPP