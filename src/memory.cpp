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

void PacketMemoryServer::put(BodyPresentationPacket const& p) {
    std::lock_guard<std::mutex> lock(_mux);
    _body_presentations.push_back(p);
}

void PacketMemoryServer::put(BodyStatePacket const& p) {
    std::lock_guard<std::mutex> lock(_mux);
    _body_states.push_back(p);
}

void PacketMemoryServer::put(CollisionNotificationPacket const& p) {
    std::lock_guard<std::mutex> lock(_mux);
    _collision_notifications.push_back(p);
}

SizeType PacketMemoryServer::get(std::deque<BodyPresentationPacket>& packets, SizeType const& from) {
    std::lock_guard<std::mutex> lock(_mux);
    for (SizeType i=from; i<_body_presentations.size(); ++i) packets.push_back(_body_presentations.at(i));
    return _body_presentations.size()-from;
}

SizeType PacketMemoryServer::get(std::deque<BodyStatePacket>& packets, SizeType const& from) {
    std::lock_guard<std::mutex> lock(_mux);
    for (SizeType i=from; i<_body_states.size(); ++i) packets.push_back(_body_states.at(i));
    return _body_states.size()-from;
}

SizeType PacketMemoryServer::get(std::deque<CollisionNotificationPacket>& packets, SizeType const& from) {
    std::lock_guard<std::mutex> lock(_mux);
    for (SizeType i=from; i<_collision_notifications.size(); ++i) packets.push_back(_collision_notifications.at(i));
    return _collision_notifications.size()-from;
}

MemoryBroker::MemoryBroker() : _body_presentation_index(0), _body_state_index(0), _collision_notification_index(0) { }

BrokerKind MemoryBroker::kind() const {
    return BrokerKind::MEMORY;
}

void MemoryBroker::send(BodyPresentationPacket const& p) {
    PacketMemoryServer::instance().put(p);
}

void MemoryBroker::send(BodyStatePacket const& p) {
    PacketMemoryServer::instance().put(p);
}

void MemoryBroker::send(CollisionNotificationPacket const& p) {
    PacketMemoryServer::instance().put(p);
}

void MemoryBroker::receive(std::deque<BodyPresentationPacket>& packets) {
    _body_presentation_index += PacketMemoryServer::instance().get(packets,_body_presentation_index);
}

void MemoryBroker::receive(std::deque<BodyStatePacket>& packets) {
    _body_state_index += PacketMemoryServer::instance().get(packets,_body_state_index);
}

void MemoryBroker::receive(std::deque<CollisionNotificationPacket>& packets) {
    _collision_notification_index += PacketMemoryServer::instance().get(packets,_collision_notification_index);
}

}