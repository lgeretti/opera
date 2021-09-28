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

SizeType MemoryBroker::get(std::deque<BodyPresentationPacket>& packets, SizeType const& from) {
    std::lock_guard<std::mutex> lock(_mux);
    for (SizeType i=from; i<_body_presentations.size(); ++i) packets.push_back(_body_presentations.at(i));
    return _body_presentations.size()-from;
}

SizeType MemoryBroker::get(std::deque<BodyStatePacket>& packets, SizeType const& from) {
    std::lock_guard<std::mutex> lock(_mux);
    for (SizeType i=from; i<_body_states.size(); ++i) packets.push_back(_body_states.at(i));
    return _body_states.size()-from;
}

SizeType MemoryBroker::get(std::deque<CollisionNotificationPacket>& packets, SizeType const& from) {
    std::lock_guard<std::mutex> lock(_mux);
    for (SizeType i=from; i<_collision_notifications.size(); ++i) packets.push_back(_collision_notifications.at(i));
    return _collision_notifications.size()-from;
}

Publisher<BodyPresentationPacket> MemoryBrokerAccess::body_presentation_publisher() const {
    return MemoryPublisher<BodyPresentationPacket>();
}

Publisher<BodyStatePacket> MemoryBrokerAccess::body_state_publisher() const {
    return MemoryPublisher<BodyStatePacket>();
}

Publisher<CollisionNotificationPacket> MemoryBrokerAccess::collision_notification_publisher() const {
    return MemoryPublisher<CollisionNotificationPacket>();
}

Subscriber<BodyPresentationPacket> MemoryBrokerAccess::body_presentation_subscriber() const {
    return MemorySubscriber<BodyPresentationPacket>();
}

Subscriber<BodyStatePacket> MemoryBrokerAccess::body_state_subscriber() const {
    return MemorySubscriber<BodyStatePacket>();
}

Subscriber<CollisionNotificationPacket> MemoryBrokerAccess::collision_notification_subscriber() const {
    return MemorySubscriber<CollisionNotificationPacket>();
}

}