/***************************************************************************
 *            memory.hpp
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

#ifndef OPERA_MEMORY_HPP
#define OPERA_MEMORY_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <tuple>
#include <thread>

#include "broker.hpp"
#include "serialisation.hpp"
#include "deserialisation.hpp"

namespace Opera {

//! A static class to hold packets synchronously, when using a memory broker
class PacketMemoryServer {
  private:
    PacketMemoryServer() = default;
  public:
    PacketMemoryServer(PacketMemoryServer const&) = delete;
    void operator=(PacketMemoryServer const&) = delete;

    //! \brief The singleton instance of this class
    static PacketMemoryServer& instance() {
        static PacketMemoryServer instance;
        return instance;
    }

    void put(BodyPresentationPacket const& p);
    void put(BodyStatePacket const& p);
    void put(CollisionNotificationPacket const& p);

    SizeType get(std::deque<BodyPresentationPacket>& packets, SizeType const& from);
    SizeType get(std::deque<BodyStatePacket>& packets, SizeType const& from);
    SizeType get(std::deque<CollisionNotificationPacket>& packets, SizeType const& from);
  private:
    List<BodyPresentationPacket> _body_presentations;
    List<BodyStatePacket> _body_states;
    List<CollisionNotificationPacket> _collision_notifications;
    std::mutex _mux;
};

//! \brief A broker to handle packets using memory
class MemoryBroker : public BrokerInterface {
  public:
    MemoryBroker();
    BrokerKind kind() const override;

    void send(BodyPresentationPacket const& p) override;
    void send(BodyStatePacket const& p) override;
    void send(CollisionNotificationPacket const& p) override;

    void receive(std::deque<BodyPresentationPacket>& packets) override;
    void receive(std::deque<BodyStatePacket>& packets) override;
    void receive(std::deque<CollisionNotificationPacket>& packets) override;

  private:
    SizeType _body_presentation_index;
    SizeType _body_state_index;
    SizeType _collision_notification_index;
};

}

#endif // OPERA_MEMORY_HPP