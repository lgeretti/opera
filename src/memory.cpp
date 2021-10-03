/***************************************************************************
 *            memory.cpp
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

#include "memory.hpp"

namespace Opera {

void MemoryBroker::put(BodyPresentationPacket const& p) {
    std::lock_guard<std::mutex> lock(_mux);
    _body_presentations.push_back(p);
}

void MemoryBroker::put(BodyStatePacket const& p) {
    std::lock_guard<std::mutex> lock(_mux);
    _body_states.push_back(p);
}

void MemoryBroker::put(CollisionNotificationPacket const& p) {
    std::lock_guard<std::mutex> lock(_mux);
    _collision_notifications.push_back(p);
}

PublisherInterface<BodyPresentationPacket>* MemoryBrokerAccess::make_body_presentation_publisher() const {
    return new MemoryPublisher<BodyPresentationPacket>();
}

PublisherInterface<BodyStatePacket>* MemoryBrokerAccess::make_body_state_publisher() const {
    return new MemoryPublisher<BodyStatePacket>();
}

PublisherInterface<CollisionNotificationPacket>* MemoryBrokerAccess::make_collision_notification_publisher() const {
    return new MemoryPublisher<CollisionNotificationPacket>();
}

SubscriberInterface<BodyPresentationPacket>* MemoryBrokerAccess::make_body_presentation_subscriber(CallbackFunction<BodyPresentationPacket> const& callback) const {
    return new MemorySubscriber<BodyPresentationPacket>(callback);
}

SubscriberInterface<BodyStatePacket>* MemoryBrokerAccess::make_body_state_subscriber(CallbackFunction<BodyStatePacket> const& callback) const {
    return new MemorySubscriber<BodyStatePacket>(callback);
}

SubscriberInterface<CollisionNotificationPacket>* MemoryBrokerAccess::make_collision_notification_subscriber(CallbackFunction<CollisionNotificationPacket> const& callback) const {
    return new MemorySubscriber<CollisionNotificationPacket>(callback);
}

}