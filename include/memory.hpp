/***************************************************************************
 *            memory.hpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of Opera, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

    //! \brief The \a idx element in the template argument list
    template<class T> T const& get(SizeType const& idx) const;

    //! \brief Size of template argument list
    template<class T> SizeType size() const;

  private:
    List<BodyPresentationPacket> _body_presentations;
    List<BodyStatePacket> _body_states;
    List<CollisionNotificationPacket> _collision_notifications;
    mutable std::mutex _mux;
};

template<> BodyPresentationPacket const& MemoryBroker::get<BodyPresentationPacket>(SizeType const& idx) const { std::lock_guard<std::mutex> lock(_mux); return _body_presentations.at(idx); }
template<> BodyStatePacket const& MemoryBroker::get<BodyStatePacket>(SizeType const& idx) const { std::lock_guard<std::mutex> lock(_mux); return _body_states.at(idx); }
template<> CollisionNotificationPacket const& MemoryBroker::get<CollisionNotificationPacket>(SizeType const& idx) const { std::lock_guard<std::mutex> lock(_mux); return _collision_notifications.at(idx); }

template<> SizeType MemoryBroker::size<BodyPresentationPacket>() const { std::lock_guard<std::mutex> lock(_mux); return _body_presentations.size(); }
template<> SizeType MemoryBroker::size<BodyStatePacket>() const { std::lock_guard<std::mutex> lock(_mux); return _body_states.size(); }
template<> SizeType MemoryBroker::size<CollisionNotificationPacket>() const { std::lock_guard<std::mutex> lock(_mux); return _collision_notifications.size(); }

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
    //! \brief Constructor
    MemorySubscriber(CallbackFunction<T> const& callback) : _next_index(MemoryBroker::instance().size<T>()), _exit_future(_exit_promise.get_future()), _callback(callback),
                                                            _thr(Thread([&] {
            while (_exit_future.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
                if (MemoryBroker::instance().size<T>() > _next_index) {
                    _callback(MemoryBroker::instance().get<T>(_next_index));
                    _next_index++;
                }
            }
        },"subc")) { }

    ~MemorySubscriber() {
        _exit_promise.set_value();
    }

  protected:
    SizeType _next_index;
    std::promise<void> _exit_promise;
    std::future<void> _exit_future;
    CallbackFunction<T> const _callback;
    Thread const _thr;
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