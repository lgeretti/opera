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
#include "thread.hpp"

#include "broker.hpp"

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

    BodyPresentationPacket const& get_body_presentation(SizeType const& idx) const;
    BodyStatePacket const& get_body_state(SizeType const& idx) const;
    CollisionNotificationPacket const& get_collision_notification(SizeType const& at) const;

    template<class T> SizeType size() const;
  private:
    List<BodyPresentationPacket> _body_presentations;
    List<BodyStatePacket> _body_states;
    List<CollisionNotificationPacket> _collision_notifications;
    mutable std::mutex _mux;
};

template<> SizeType MemoryBroker::size<BodyPresentationPacket>() const {
    return _body_presentations.size();
}

template<> SizeType MemoryBroker::size<BodyStatePacket>() const {
    return _body_states.size();
}

template<> SizeType MemoryBroker::size<CollisionNotificationPacket>() const {
    return _collision_notifications.size();
}

//! \brief The publisher of objects to memory
template<class T> class MemoryPublisher : public PublisherInterface<T> {
  public:
    void put(T const& obj) override { MemoryBroker::instance().put(obj); }
};

//! \brief The subscriber to objects published to memory
//! \details The advancement of acquisition is local to the subscriber, but for simplicity
//! a new subscriber starts from the beginning of memory content
template<class T> class MemorySubscriberBase : public SubscriberInterface<T> {
  protected:
    //! \brief Constructor
    MemorySubscriberBase(CallbackFunction<T> const& callback) : _next_index(MemoryBroker::instance().size<T>()), _exit_future(_exit_promise.get_future()), _callback(callback),
        _thr(Thread([&] {
            while (_exit_future.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
                if (has_new_objects()) {
                    _callback(get_new_object());
                    _next_index++;
                }
            }
        },"subc")) { }

    //! \brief Whether there are new objects that have not been got yet
    virtual bool has_new_objects() const = 0;

    //! \brief Get the new object with respect to the current index
    virtual T const& get_new_object() const = 0;

  public:

    virtual ~MemorySubscriberBase() {
        _exit_promise.set_value();
    }

  protected:
    SizeType _next_index;
    std::promise<void> _exit_promise;
    std::future<void> _exit_future;
    CallbackFunction<T> const _callback;
    Thread const _thr;
};

class BodyPresentationPacketMemorySubscriber : public MemorySubscriberBase<BodyPresentationPacket> {
  public:
    BodyPresentationPacketMemorySubscriber(CallbackFunction<BodyPresentationPacket> const& callback);
  protected:
    bool has_new_objects() const override;
    BodyPresentationPacket const& get_new_object() const override;
};

class BodyStatePacketMemorySubscriber : public MemorySubscriberBase<BodyStatePacket> {
  public:
    BodyStatePacketMemorySubscriber(CallbackFunction<BodyStatePacket> const& callback);
  protected:
    bool has_new_objects() const override;
    BodyStatePacket const& get_new_object() const override;
};

class CollisionNotificationPacketMemorySubscriber : public MemorySubscriberBase<CollisionNotificationPacket> {
  public:
    CollisionNotificationPacketMemorySubscriber(CallbackFunction<CollisionNotificationPacket> const& callback);
  protected:
    bool has_new_objects() const override;
    CollisionNotificationPacket const& get_new_object() const override;
};

//! \brief A broker to handle packets using memory
class MemoryBrokerAccess : public BrokerAccessInterface {
  public:
    PublisherInterface<BodyPresentationPacket>* make_body_presentation_publisher() const override;
    PublisherInterface<BodyStatePacket>* make_body_state_publisher() const override;
    PublisherInterface<CollisionNotificationPacket>* make_collision_notification_publisher() const override;
    SubscriberInterface<BodyPresentationPacket>* make_body_presentation_subscriber(CallbackFunction<BodyPresentationPacket> const& callback) const override;
    SubscriberInterface<BodyStatePacket>* make_body_state_subscriber(CallbackFunction<BodyStatePacket> const& callback) const override;
    SubscriberInterface<CollisionNotificationPacket>* make_collision_notification_subscriber(CallbackFunction<CollisionNotificationPacket> const& callback) const override;
};

}

#endif // OPERA_MEMORY_HPP