/***************************************************************************
 *            memory.cpp
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