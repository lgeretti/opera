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

const std::string MqttBrokerAccess::BODY_PRESENTATION_TOPIC = "opera/body-presentation";
const std::string MqttBrokerAccess::BODY_STATE_TOPIC = "opera/body-state";
const std::string MqttBrokerAccess::COLLISION_NOTIFICATION_TOPIC = "opera/collision-notification";

MqttBrokerAccess::MqttBrokerAccess(std::string const& hostname, int port) : _hostname(hostname), _port(port) {
    int rc = mosquitto_lib_init();
    OPERA_ASSERT_MSG(rc == MOSQ_ERR_SUCCESS, "Error initialising Mosquito library: " << mosquitto_strerror(rc))
}

MqttBrokerAccess::~MqttBrokerAccess() {
    mosquitto_lib_cleanup();
}

PublisherInterface<BodyPresentationPacket>* MqttBrokerAccess::make_body_presentation_publisher() const {
    return new MqttPublisher<BodyPresentationPacket>(BODY_PRESENTATION_TOPIC, _hostname, _port);
}

PublisherInterface<BodyStatePacket>* MqttBrokerAccess::make_body_state_publisher() const {
    return new MqttPublisher<BodyStatePacket>(BODY_STATE_TOPIC, _hostname, _port);
}

PublisherInterface<CollisionNotificationPacket>* MqttBrokerAccess::make_collision_notification_publisher() const {
    return new MqttPublisher<CollisionNotificationPacket>(COLLISION_NOTIFICATION_TOPIC, _hostname, _port);
}

SubscriberInterface<BodyPresentationPacket>* MqttBrokerAccess::make_body_presentation_subscriber(CallbackFunction<BodyPresentationPacket> const& callback) const {
    return new MqttSubscriber<BodyPresentationPacket>(BODY_PRESENTATION_TOPIC, _hostname, _port, callback);
}

SubscriberInterface<BodyStatePacket>* MqttBrokerAccess::make_body_state_subscriber(CallbackFunction<BodyStatePacket> const& callback) const {
    return new MqttSubscriber<BodyStatePacket>(BODY_STATE_TOPIC, _hostname, _port, callback);
}

SubscriberInterface<CollisionNotificationPacket>* MqttBrokerAccess::make_collision_notification_subscriber(CallbackFunction<CollisionNotificationPacket> const& callback) const {
    return new MqttSubscriber<CollisionNotificationPacket>(COLLISION_NOTIFICATION_TOPIC, _hostname, _port, callback);
}

}