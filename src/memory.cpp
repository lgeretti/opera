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

void PacketMemoryServer::get(std::deque<BodyPresentationPacket>& packets) {
    std::lock_guard<std::mutex> lock(_mux);
    for (auto& p : _body_presentations) packets.push_back(p);
    _body_presentations.clear();
}

void PacketMemoryServer::get(std::deque<BodyStatePacket>& packets) {
    std::lock_guard<std::mutex> lock(_mux);
    for (auto& p : _body_states) packets.push_back(p);
    _body_states.clear();
}

void PacketMemoryServer::get(std::deque<CollisionNotificationPacket>& packets) {
    std::lock_guard<std::mutex> lock(_mux);
    for (auto& p : _collision_notifications) packets.push_back(p);
    _collision_notifications.clear();
}

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
    PacketMemoryServer::instance().get(packets);
}

void MemoryBroker::receive(std::deque<BodyStatePacket>& packets) {
    PacketMemoryServer::instance().get(packets);
}

void MemoryBroker::receive(std::deque<CollisionNotificationPacket>& packets) {
    PacketMemoryServer::instance().get(packets);
}

}