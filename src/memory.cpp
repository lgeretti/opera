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
    _body_presentations.append(p);
}

void PacketMemoryServer::put(BodyStatePacket const& p) {
    std::lock_guard<std::mutex> lock(_mux);
    _body_states.append(p);
}

void PacketMemoryServer::put(CollisionNotificationPacket const& p) {
    std::lock_guard<std::mutex> lock(_mux);
    _collision_notifications.append(p);
}

void PacketMemoryServer::get(List<BodyPresentationPacket>& packets) {
    std::lock_guard<std::mutex> lock(_mux);
    packets.append(_body_presentations);
    _body_presentations.clear();
}

void PacketMemoryServer::get(List<BodyStatePacket>& packets) {
    std::lock_guard<std::mutex> lock(_mux);
    packets.append(_body_states);
    _body_states.clear();
}

void PacketMemoryServer::get(List<CollisionNotificationPacket>& packets) {
    std::lock_guard<std::mutex> lock(_mux);
    packets.append(_collision_notifications);
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

void MemoryBroker::receive(List<BodyPresentationPacket>& packets) {
    PacketMemoryServer::instance().get(packets);
}

void MemoryBroker::receive(List<BodyStatePacket>& packets) {
    PacketMemoryServer::instance().get(packets);
}

void MemoryBroker::receive(List<CollisionNotificationPacket>& packets) {
    PacketMemoryServer::instance().get(packets);
}

}