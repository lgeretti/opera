/***************************************************************************
 *            mqtt.cpp
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

#include "mqtt.hpp"

namespace Opera {

MqttBrokerAccess::MqttBrokerAccess(std::string const& hostname, int port) : _hostname(hostname), _port(port) {
    int rc = mosquitto_lib_init();
    ARIADNE_ASSERT_MSG(rc == MOSQ_ERR_SUCCESS, "Error initialising Mosquito library: " << mosquitto_strerror(rc))
}

MqttBrokerAccess::~MqttBrokerAccess() {
    int rc = mosquitto_lib_cleanup();
    ARIADNE_ASSERT_MSG(rc == MOSQ_ERR_SUCCESS, "Error cleaning up Mosquito library: " << mosquitto_strerror(rc))
}

BodyPresentationPacketMqttPublisher::BodyPresentationPacketMqttPublisher(std::string const& hostname, int port)
    : MqttPublisherBase<BodyPresentationPacket>(OPERA_PRESENTATION_TOPIC,hostname,port) { }

std::string BodyPresentationPacketMqttPublisher::serialise(BodyPresentationPacket const& obj) const {
    return BodyPresentationPacketSerialiser(obj).to_string();
}

BodyStatePacketMqttPublisher::BodyStatePacketMqttPublisher(std::string const& hostname, int port)
    : MqttPublisherBase<BodyStatePacket>(OPERA_STATE_TOPIC,hostname,port) { }

std::string BodyStatePacketMqttPublisher::serialise(BodyStatePacket const& obj) const {
    return BodyStatePacketSerialiser(obj).to_string();
}

CollisionNotificationPacketMqttPublisher::CollisionNotificationPacketMqttPublisher(std::string const& hostname, int port)
    : MqttPublisherBase<CollisionNotificationPacket>(OPERA_COLLISION_NOTIFICATION_TOPIC,hostname,port) { }

std::string CollisionNotificationPacketMqttPublisher::serialise(CollisionNotificationPacket const& obj) const {
    return CollisionNotificationPacketSerialiser(obj).to_string();
}

BodyPresentationPacketMqttSubscriber::BodyPresentationPacketMqttSubscriber(std::string const& hostname, int port)
    : MqttSubscriberBase<BodyPresentationPacket>(OPERA_PRESENTATION_TOPIC,hostname,port) { }

void BodyPresentationPacketMqttSubscriber::message_callback_set() {
    mosquitto_message_callback_set(_subscriber, subscriber_on_body_presentation_message);
}

BodyStatePacketMqttSubscriber::BodyStatePacketMqttSubscriber(std::string const& hostname, int port)
    : MqttSubscriberBase<BodyStatePacket>(OPERA_STATE_TOPIC,hostname,port) { }

void BodyStatePacketMqttSubscriber::message_callback_set() {
    mosquitto_message_callback_set(_subscriber, subscriber_on_body_state_message);
}

CollisionNotificationPacketMqttSubscriber::CollisionNotificationPacketMqttSubscriber(std::string const& hostname, int port)
    : MqttSubscriberBase<CollisionNotificationPacket>(OPERA_COLLISION_NOTIFICATION_TOPIC,hostname,port) { }

void CollisionNotificationPacketMqttSubscriber::message_callback_set() {
    mosquitto_message_callback_set(_subscriber, subscriber_on_collision_notification_message);
}

PublisherInterface<BodyPresentationPacket>* MqttBrokerAccess::body_presentation_publisher() const {
    return new BodyPresentationPacketMqttPublisher(_hostname,_port);
}

PublisherInterface<BodyStatePacket>* MqttBrokerAccess::body_state_publisher() const {
    return new BodyStatePacketMqttPublisher(_hostname,_port);
}

PublisherInterface<CollisionNotificationPacket>* MqttBrokerAccess::collision_notification_publisher() const {
    return new CollisionNotificationPacketMqttPublisher(_hostname,_port);
}

SubscriberInterface<BodyPresentationPacket>* MqttBrokerAccess::body_presentation_subscriber() const {
    return new BodyPresentationPacketMqttSubscriber(_hostname,_port);
}

SubscriberInterface<BodyStatePacket>* MqttBrokerAccess::body_state_subscriber() const {
    return new BodyStatePacketMqttSubscriber(_hostname,_port);
}

SubscriberInterface<CollisionNotificationPacket>* MqttBrokerAccess::collision_notification_subscriber() const {
    return new CollisionNotificationPacketMqttSubscriber(_hostname,_port);
}

}