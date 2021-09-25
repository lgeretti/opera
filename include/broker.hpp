/***************************************************************************
 *            broker.hpp
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

#ifndef OPERA_BROKER_HPP
#define OPERA_BROKER_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <tuple>

#include <ariadne/utility/handle.hpp>
#include <ariadne/utility/container.hpp>
#include "packet.hpp"

namespace Opera {

//! \brief The different kinds of communication brokers supported
enum class BrokerKind { MEMORY, KAFKA, ROS };

//! \brief Interface for a communication broker
class BrokerInterface {
  public:
    //! \brief The kind of the broker
    virtual BrokerKind kind() const = 0;

    virtual void send(BodyPresentationPacket const& p) = 0;
    virtual void send(BodyStatePacket const& p) = 0;
    virtual void send(CollisionNotificationPacket const& p) = 0;

    virtual void receive(List<BodyPresentationPacket>& packets) = 0;
    virtual void receive(List<BodyStatePacket>& packets) = 0;
    virtual void receive(List<CollisionNotificationPacket>& packets) = 0;
};

//! \brief Handle for a broker
class Broker : public Ariadne::Handle<BrokerInterface> {
  public:
    using Ariadne::Handle<BrokerInterface>::Handle;
    BrokerKind kind() const { return _ptr->kind(); }
    void send(BodyPresentationPacket const& p) { _ptr->send(p); }
    void send(BodyStatePacket const& p) { _ptr->send(p); }
    void send(CollisionNotificationPacket const& p) { _ptr->send(p); }
    void receive(List<BodyPresentationPacket>& packets) { return _ptr->receive(packets); }
    void receive(List<BodyStatePacket>& packets) { return _ptr->receive(packets); }
    void receive(List<CollisionNotificationPacket>& packets) { return _ptr->receive(packets); }
};

//! \brief A class holding communication brokers for production/consumption of packets
class BrokerManager {
  private:
    BrokerManager() = default;
  public:
    BrokerManager(BrokerManager const&) = delete;
    void operator=(BrokerManager const&) = delete;

    //! \brief The singleton instance of this class
    static BrokerManager& instance() {
        static BrokerManager instance;
        return instance;
    }

    //! \brief Add a \a broker
    void add(Broker const& broker);
    //! \brief Return the broker for the given \a kind
    Broker& get(BrokerKind const& kind);

  private:
    Ariadne::Map<BrokerKind,Broker> _brokers;
};

}

#endif // OPERA_BROKER_HPP