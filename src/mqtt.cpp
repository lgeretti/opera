/***************************************************************************
 *            mqtt.cpp
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