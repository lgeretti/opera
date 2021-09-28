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

//! \brief An interface for publishing objects
template<class T> class PublisherInterface {
  public:
    virtual void put(T const& obj) = 0;
};

//! \brief An interface for subscribing to objects published
template<class T> class SubscriberInterface {
  public:
    virtual void get(std::deque<T>& objs) = 0;
};

//! \brief Handle class for a publisher
template<class T> class Publisher : public Ariadne::Handle<PublisherInterface<T>> {
  public:
    using Ariadne::Handle<PublisherInterface<T>>::Handle;
    void put(T const& obj) { this->_ptr->put(obj); }
};

//! \brief Handle class for a subscriber
template<class T> class Subscriber : public Ariadne::Handle<SubscriberInterface<T>> {
public:
    using Ariadne::Handle<SubscriberInterface<T>>::Handle;
    void get(std::deque<T>& objs) { this->_ptr->get(objs); }
};

//! \brief Interface for access to a communication broker
class BrokerAccessInterface {
  public:
    virtual Publisher<BodyPresentationPacket> body_presentation_publisher() const = 0;
    virtual Publisher<BodyStatePacket> body_state_publisher() const = 0;
    virtual Publisher<CollisionNotificationPacket> collision_notification_publisher() const = 0;

    virtual Subscriber<BodyPresentationPacket> body_presentation_subscriber() const = 0;
    virtual Subscriber<BodyStatePacket> body_state_subscriber() const = 0;
    virtual Subscriber<CollisionNotificationPacket> collision_notification_subscriber() const = 0;
};

//! \brief Handle for a broker access
class BrokerAccess : public Ariadne::Handle<BrokerAccessInterface> {
  public:
    using Ariadne::Handle<BrokerAccessInterface>::Handle;
    Publisher<BodyPresentationPacket> body_presentation_publisher() const { return _ptr->body_presentation_publisher(); }
    Publisher<BodyStatePacket> body_state_publisher() const { return _ptr->body_state_publisher(); }
    Publisher<CollisionNotificationPacket> collision_notification_publisher() const { return _ptr->collision_notification_publisher(); }
    Subscriber<BodyPresentationPacket> body_presentation_subscriber() const { return _ptr->body_presentation_subscriber(); }
    Subscriber<BodyStatePacket> body_state_subscriber() const { return _ptr->body_state_subscriber(); }
    Subscriber<CollisionNotificationPacket> collision_notification_subscriber() const { return _ptr->collision_notification_subscriber(); }
};

}

#endif // OPERA_BROKER_HPP