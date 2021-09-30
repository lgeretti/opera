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

BodyPresentationPacket const& MemoryBroker::get_body_presentation(SizeType const& idx) const {
    std::lock_guard<std::mutex> lock(_mux);
    return _body_presentations.at(idx);
}

BodyStatePacket const& MemoryBroker::get_body_state(SizeType const& idx) const {
    std::lock_guard<std::mutex> lock(_mux);
    return _body_states.at(idx);
}

CollisionNotificationPacket const& MemoryBroker::get_collision_notification(SizeType const& idx) const {
    std::lock_guard<std::mutex> lock(_mux);
    return _collision_notifications.at(idx);
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
    return new BodyPresentationPacketMemorySubscriber(callback);
}

SubscriberInterface<BodyStatePacket>* MemoryBrokerAccess::make_body_state_subscriber(CallbackFunction<BodyStatePacket> const& callback) const {
    return new BodyStatePacketMemorySubscriber(callback);
}

SubscriberInterface<CollisionNotificationPacket>* MemoryBrokerAccess::make_collision_notification_subscriber(CallbackFunction<CollisionNotificationPacket> const& callback) const {
    return new CollisionNotificationPacketMemorySubscriber(callback);
}

BodyPresentationPacketMemorySubscriber::BodyPresentationPacketMemorySubscriber(CallbackFunction<BodyPresentationPacket> const& callback)
    : MemorySubscriberBase<BodyPresentationPacket>(callback) { }

bool BodyPresentationPacketMemorySubscriber::has_new_objects() const {
    return MemoryBroker::instance().size<BodyPresentationPacket>() > _next_index;
}

BodyPresentationPacket const& BodyPresentationPacketMemorySubscriber::get_new_object() const {
    return MemoryBroker::instance().get_body_presentation(_next_index);
}

BodyStatePacketMemorySubscriber::BodyStatePacketMemorySubscriber(CallbackFunction<BodyStatePacket> const& callback)
    : MemorySubscriberBase<BodyStatePacket>(callback) { }

bool BodyStatePacketMemorySubscriber::has_new_objects() const {
    return MemoryBroker::instance().size<BodyStatePacket>() > _next_index;
}

BodyStatePacket const& BodyStatePacketMemorySubscriber::get_new_object() const {
    return MemoryBroker::instance().get_body_state(_next_index);
}

CollisionNotificationPacketMemorySubscriber::CollisionNotificationPacketMemorySubscriber(CallbackFunction<CollisionNotificationPacket> const& callback)
    : MemorySubscriberBase<CollisionNotificationPacket>(callback) { }

bool CollisionNotificationPacketMemorySubscriber::has_new_objects() const {
    return MemoryBroker::instance().size<CollisionNotificationPacket>() > _next_index;
}

CollisionNotificationPacket const& CollisionNotificationPacketMemorySubscriber::get_new_object() const {
    return MemoryBroker::instance().get_collision_notification(_next_index);
}

}