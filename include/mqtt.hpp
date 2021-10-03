/***************************************************************************
 *            mqtt.hpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of Opera, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
#include "logging.hpp"

namespace Opera {

//! \brief The publisher of objects to the MQTT broker
template<class T> class MqttPublisher : public PublisherInterface<T> {
  public:
    MqttPublisher(std::string const& topic, std::string const& hostname, int port) : _topic(topic) {
        _mosquitto_publisher = mosquitto_new(nullptr, true, nullptr);
        OPERA_ASSERT_MSG(_mosquitto_publisher != nullptr, "Error: Out of memory.")

        int rc = mosquitto_connect(_mosquitto_publisher, hostname.c_str(), port, 60);
        if (rc != MOSQ_ERR_SUCCESS){
            mosquitto_destroy(_mosquitto_publisher);
            OPERA_THROW_RTE("Error connecting: " << mosquitto_strerror(rc))
        }

        mosquitto_loop_start(_mosquitto_publisher);
    }

    void put(T const& obj) override {
        std::string payload = Serialiser<T>(obj).to_string();
        int rc = mosquitto_publish(_mosquitto_publisher, nullptr, _topic.c_str(), payload.size(), payload.c_str(), 2, false);
        OPERA_ASSERT_MSG(rc == MOSQ_ERR_SUCCESS,"Error publishing: " << mosquitto_strerror(rc))
    }

    ~MqttPublisher() {
        mosquitto_destroy(_mosquitto_publisher);
    }

  private:
    std::string const _topic;
    struct mosquitto* _mosquitto_publisher;
};

//! \brief Struct for holding context for a callback, including data for proper registration of the callback thread (otherwise inaccessible)
template<class T> struct CallbackContext {
    CallbackContext(CallbackFunction<T> f, int pll, std::string ptn) :
            function(f), parent_logger_level(pll), parent_thread_name(ptn), thread_id(std::this_thread::get_id()), registered(false) { }
    CallbackFunction<T> function;
    int parent_logger_level;
    std::string parent_thread_name;
    std::thread::id thread_id;
    bool registered;
};

//! \brief Callback for an MQTT message, used for running the actual callback on the deserialised message
template<class T> void subscriber_on_message(struct mosquitto*, void *obj, const struct mosquitto_message *msg) {
    auto callback_context = static_cast<CallbackContext<T>*>(obj);
    if (not callback_context->registered) {
        callback_context->thread_id = std::this_thread::get_id();
        Logger::instance().register_self_thread(callback_context->parent_thread_name, callback_context->parent_logger_level);
        callback_context->registered = true;
    }

    Deserialiser<T> deserialiser(std::string((char *)msg->payload,static_cast<SizeType>(msg->payloadlen)).c_str());
    callback_context->function(deserialiser.make());
}

//! \brief The subscriber to objects published to MQTT
template<class T> class MqttSubscriber : public SubscriberInterface<T> {
  public:
    //! \brief Connects and starts the main asynchronous loop for getting messages
    MqttSubscriber(std::string const& topic, std::string const& hostname, int port, CallbackFunction<T> const& callback)
        : _topic(topic), _hostname(hostname), _port(port),
          _callback_context(callback, Logger::instance().current_level(), Logger::instance().current_thread_name())
    {
        _subscriber = mosquitto_new(nullptr, true, (void*)&_callback_context);
        OPERA_ASSERT_MSG(_subscriber != nullptr,"Error: out of memory.")

        mosquitto_message_callback_set(_subscriber, subscriber_on_message<T>);

        int rc = mosquitto_connect(_subscriber, _hostname.c_str(), _port, 60);
        if (rc != MOSQ_ERR_SUCCESS) {
            mosquitto_destroy(_subscriber);
            OPERA_THROW_RTE("Error connecting: " << mosquitto_strerror(rc))
        }

        mosquitto_subscribe(_subscriber, nullptr, _topic.c_str(), 2);
        mosquitto_loop_start(_subscriber);
    }

    virtual ~MqttSubscriber() {
        if (_subscriber != nullptr) {
            mosquitto_destroy(_subscriber);
        }
        if (_callback_context.registered)
            Logger::instance().unregister_thread(_callback_context.thread_id);
    }

  protected:
    std::string const _topic;
    std::string const _hostname;
    int const _port;
    CallbackContext<T> const _callback_context;
    struct mosquitto* _subscriber;
};

//! \brief Access to a broker to handle Opera-specific packets using MQTT
class MqttBrokerAccess : public BrokerAccessInterface {
  public:
    static const std::string BODY_PRESENTATION_TOPIC;
    static const std::string BODY_STATE_TOPIC;
    static const std::string COLLISION_NOTIFICATION_TOPIC;
  public:
    MqttBrokerAccess(std::string const& hostname, int port);
    PublisherInterface<BodyPresentationPacket>* make_body_presentation_publisher() const override;
    PublisherInterface<BodyStatePacket>* make_body_state_publisher() const override;
    PublisherInterface<CollisionNotificationPacket>* make_collision_notification_publisher() const override;
    SubscriberInterface<BodyPresentationPacket>* make_body_presentation_subscriber(CallbackFunction<BodyPresentationPacket> const& callback) const override;
    SubscriberInterface<BodyStatePacket>* make_body_state_subscriber(CallbackFunction<BodyStatePacket> const& callback) const override;
    SubscriberInterface<CollisionNotificationPacket>* make_collision_notification_subscriber(CallbackFunction<CollisionNotificationPacket> const& callback) const override;
    ~MqttBrokerAccess();

  private:
    std::string const _hostname;
    int const _port;
};

}

#endif // OPERA_MQTT_HPP