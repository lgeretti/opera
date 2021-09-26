/***************************************************************************
 *            kafka.cpp
 *
 *  Copyright  2021  Andrea Gagliardo, Sandro Ferrari, Luca Geretti
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

#include "websocket.hpp"

namespace Opera {

BrokerKind websocket::kind() const {
    return BrokerKind::ROS;
}

void websocket::send(BodyPresentationPacket const& p) {
    //
}

void websocket::send(BodyStatePacket const& p) {
    //
}

void websocket::send(CollisionNotificationPacket const& p) {
    websocket::send_notification_message(p);
}

void websocket::receive(std::deque<BodyPresentationPacket>& packets) {
    for (auto& p : presentations) packets.push_back(p);
}

void websocket::receive(std::deque<BodyStatePacket>& packets) {
    for (auto& p : states) packets.push_back(p);
}

void websocket::receive(std::deque<CollisionNotificationPacket>& packets) {
    //
}

}