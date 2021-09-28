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

//! \brief A static class to hold packets synchronously using memory
//! \details Packets are accumulated indefinitely
class MemoryBroker {
  private:
    MemoryBroker() = default;
  public:
    MemoryBroker(MemoryBroker const&) = delete;
    void operator=(MemoryBroker const&) = delete;

    //! \brief The singleton instance of this class
    static MemoryBroker& instance() {
        static MemoryBroker instance;
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

//! \brief The publisher of objects to memory
template<class T> class MemoryPublisher : public PublisherInterface<T> {
  public:
    void put(T const& obj) override { MemoryBroker::instance().put(obj); }
};

//! \brief The subscriber to objects published to memory
//! \details The advancement of acquisition is local to the subscriber, but for simplicity
//! a new subscriber starts from the beginning of memory content
template<class T> class MemorySubscriber : public SubscriberInterface<T> {
  public:
    MemorySubscriber() : _current_index(0) { }
    void get(std::deque<T>& objs) override { _current_index += MemoryBroker::instance().get(objs, _current_index); }
  private:
    SizeType _current_index;
};

//! \brief A broker to handle packets using memory
class MemoryBrokerAccess : public BrokerAccessInterface {
  public:
    Publisher<BodyPresentationPacket> body_presentation_publisher() const override;
    Publisher<BodyStatePacket> body_state_publisher() const override;
    Publisher<CollisionNotificationPacket> collision_notification_publisher() const override;
    Subscriber<BodyPresentationPacket> body_presentation_subscriber() const override;
    Subscriber<BodyStatePacket> body_state_subscriber() const override;
    Subscriber<CollisionNotificationPacket> collision_notification_subscriber() const override;
};

}

#endif // OPERA_MEMORY_HPP