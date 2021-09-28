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

    BodyPresentationPacket const& get_body_presentation(SizeType const& idx) const;
    BodyStatePacket const& get_body_state(SizeType const& idx) const;
    CollisionNotificationPacket const& get_collision_notification(SizeType const& at) const;

    SizeType body_presentations_size() const;
    SizeType body_states_size() const;
    SizeType collision_notifications_size() const;
  private:
    List<BodyPresentationPacket> _body_presentations;
    List<BodyStatePacket> _body_states;
    List<CollisionNotificationPacket> _collision_notifications;
    mutable std::mutex _mux;
};

//! \brief The publisher of objects to memory
template<class T> class MemoryPublisher : public PublisherInterface<T> {
  public:
    void put(T const& obj) override { MemoryBroker::instance().put(obj); }
};

//! \brief The subscriber to objects published to memory
//! \details The advancement of acquisition is local to the subscriber, but for simplicity
//! a new subscriber starts from the beginning of memory content
template<class T> class MemorySubscriberBase : public SubscriberInterface<T> {
    typedef std::function<void(T const&)> FunctionType;
  protected:
    //! \brief Set the next index and make sure that _stop is false
    MemorySubscriberBase() : _next_index(0), _started(false), _stop(false) { }

    //! \brief Whether there are new objects that have not been got yet
    virtual bool has_new_objects() const = 0;

    //! \brief Get the new object with respect to the current index
    virtual T const& get_new_object() const = 0;

  public:
    //! \brief The main asynchronous loop for getting objects from memory
    //! \details Allows multiple calls, waiting to finish the previous callback if necessary
    void loop_get(FunctionType const& callback) override {
        if (_started) {
            _stop = true;
            _thr.join();
            _started = false;
        }
        _thr = std::thread([&,callback] {
            while (not _stop) {
                if (has_new_objects()) {
                    callback(get_new_object());
                    _next_index++;
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
        });
        _started = true;
    }

    virtual ~MemorySubscriberBase() {
        _stop = true;
        _thr.join();
    }

  protected:
    SizeType _next_index;
    bool _started;
    bool _stop;
    std::thread _thr;
};

class BodyPresentationPacketMemorySubscriber : public MemorySubscriberBase<BodyPresentationPacket> {
  public:
    bool has_new_objects() const override;
    BodyPresentationPacket const& get_new_object() const override;
};

class BodyStatePacketMemorySubscriber : public MemorySubscriberBase<BodyStatePacket> {
  public:
    bool has_new_objects() const override;
    BodyStatePacket const& get_new_object() const override;
};

class CollisionNotificationPacketMemorySubscriber : public MemorySubscriberBase<CollisionNotificationPacket> {
  public:
    bool has_new_objects() const override;
    CollisionNotificationPacket const& get_new_object() const override;
};

//! \brief A broker to handle packets using memory
class MemoryBrokerAccess : public BrokerAccessInterface {
  public:
    PublisherInterface<BodyPresentationPacket>* body_presentation_publisher() const override;
    PublisherInterface<BodyStatePacket>* body_state_publisher() const override;
    PublisherInterface<CollisionNotificationPacket>* collision_notification_publisher() const override;
    SubscriberInterface<BodyPresentationPacket>* body_presentation_subscriber() const override;
    SubscriberInterface<BodyStatePacket>* body_state_subscriber() const override;
    SubscriberInterface<CollisionNotificationPacket>* collision_notification_subscriber() const override;
};

}

#endif // OPERA_MEMORY_HPP