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
#include <functional>

#include "handle.hpp"
#include "packet.hpp"

namespace Opera {

template<class T> using CallbackFunction = std::function<void(T const&)>;

//! \brief An interface for publishing objects
template<class T> class PublisherInterface {
  public:
    //! \brief Publish the \a obj
    virtual void put(T const& obj) = 0;
    //! \brief Default destructor to avoid destructor not being called on objects of this type
    virtual ~PublisherInterface() = default;
};

//! \brief An interface for subscribing to objects published
template<class T> class SubscriberInterface {
  public:
    //! \brief Default destructor to avoid destructor not being called on objects of this type
    virtual ~SubscriberInterface() = default;
};

//! \brief Interface for access to a communication broker
class BrokerAccessInterface {
  public:
    virtual PublisherInterface<BodyPresentationPacket>* make_body_presentation_publisher() const = 0;
    virtual PublisherInterface<BodyStatePacket>* make_body_state_publisher() const = 0;
    virtual PublisherInterface<CollisionNotificationPacket>* make_collision_notification_publisher() const = 0;

    virtual SubscriberInterface<BodyPresentationPacket>* make_body_presentation_subscriber(CallbackFunction<BodyPresentationPacket> const& callback) const = 0;
    virtual SubscriberInterface<BodyStatePacket>* make_body_state_subscriber(CallbackFunction<BodyStatePacket> const& callback) const = 0;
    virtual SubscriberInterface<CollisionNotificationPacket>* make_collision_notification_subscriber(CallbackFunction<CollisionNotificationPacket> const& callback) const = 0;

    //! \brief Default destructor to avoid destructor not being called on objects of this type
    virtual ~BrokerAccessInterface() = default;
};

//! \brief Handle for a broker access
class BrokerAccess : public Handle<BrokerAccessInterface> {
  public:
    using Handle<BrokerAccessInterface>::Handle;
    PublisherInterface<BodyPresentationPacket>* make_body_presentation_publisher() const { return _ptr->make_body_presentation_publisher(); }
    PublisherInterface<BodyStatePacket>* make_body_state_publisher() const { return _ptr->make_body_state_publisher(); }
    PublisherInterface<CollisionNotificationPacket>* make_collision_notification_publisher() const { return _ptr->make_collision_notification_publisher(); }
    SubscriberInterface<BodyPresentationPacket>* make_body_presentation_subscriber(CallbackFunction<BodyPresentationPacket> const& callback) const { return _ptr->make_body_presentation_subscriber(callback); }
    SubscriberInterface<BodyStatePacket>* make_body_state_subscriber(CallbackFunction<BodyStatePacket> const& callback) const { return _ptr->make_body_state_subscriber(callback); }
    SubscriberInterface<CollisionNotificationPacket>* make_collision_notification_subscriber(CallbackFunction<CollisionNotificationPacket> const& callback) const { return _ptr->make_collision_notification_subscriber(callback); }
};

}

#endif // OPERA_BROKER_HPP