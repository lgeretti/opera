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

PublisherInterface<BodyPresentationPacket>* MqttBrokerAccess::body_presentation_publisher() const {
    return new MqttPublisher<BodyPresentationPacket>(OPERA_PRESENTATION_TOPIC,_hostname,_port);
}

PublisherInterface<BodyStatePacket>* MqttBrokerAccess::body_state_publisher() const {
    return new MqttPublisher<BodyStatePacket>(OPERA_STATE_TOPIC,_hostname,_port);
}

PublisherInterface<CollisionNotificationPacket>* MqttBrokerAccess::collision_notification_publisher() const {
    return new MqttPublisher<CollisionNotificationPacket>(OPERA_COLLISION_NOTIFICATION_TOPIC,_hostname,_port);
}

SubscriberInterface<BodyPresentationPacket>* MqttBrokerAccess::body_presentation_subscriber() const {
    return new MqttSubscriber<BodyPresentationPacket>(OPERA_PRESENTATION_TOPIC,_hostname,_port);
}

SubscriberInterface<BodyStatePacket>* MqttBrokerAccess::body_state_subscriber() const {
    return new MqttSubscriber<BodyStatePacket>(OPERA_STATE_TOPIC,_hostname,_port);
}

SubscriberInterface<CollisionNotificationPacket>* MqttBrokerAccess::collision_notification_subscriber() const {
    return new MqttSubscriber<CollisionNotificationPacket>(OPERA_COLLISION_NOTIFICATION_TOPIC,_hostname,_port);
}

}