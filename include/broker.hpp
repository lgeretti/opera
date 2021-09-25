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

    virtual void receive(std::deque<BodyPresentationPacket>& packets) = 0;
    virtual void receive(std::deque<BodyStatePacket>& packets) = 0;
    virtual void receive(std::deque<CollisionNotificationPacket>& packets) = 0;
};

//! \brief Handle for a broker
class Broker : public Ariadne::Handle<BrokerInterface> {
  public:
    using Ariadne::Handle<BrokerInterface>::Handle;
    BrokerKind kind() const { return _ptr->kind(); }
    template<class T> void send(T const& p) { _ptr->send(p); }
    template<class T> void receive(std::deque<T>& packets) { return _ptr->receive(packets); }
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
    //! \brief The number of brokers added
    SizeType num_brokers() const;
    //! \brief Return if a broker of the given \a kind is present
    bool has_broker(BrokerKind const& kind) const;
    //! \brief Remove the brokers
    void clear();

    //! \brief Send the packet \a p to all brokers
    template<class T> void send(T const& p) { for (auto& b : _brokers) b.second.send(p); }
    //! \brief Receive packets from all brokers and append them to \a packets
    template<class T> void receive(std::deque<T>& packets) { for (auto& b : _brokers) b.second.receive(packets); }

  private:
    Ariadne::Map<BrokerKind,Broker> _brokers;
};

}

#endif // OPERA_BROKER_HPP