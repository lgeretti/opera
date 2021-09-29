/***************************************************************************
 *            callback_buffer.hpp
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

#ifndef OPERA_CALLBACK_QUEUE_HPP
#define OPERA_CALLBACK_QUEUE_HPP

#include <ariadne/utility/container.hpp>

namespace Opera {

using IdType = unsigned int;
using BodyIdType = Ariadne::String;
using TimestampType = long unsigned int; // Expressed in nanoseconds
using Ariadne::List;
using Ariadne::Pair;
using Ariadne::SizeType;
using Ariadne::String;

//! \brief Holds objects and supports a callback every time you add or remove an element
//! \details The class is thread-safe
template<class T> class CallbackQueue {
    using CallbackType = std::function<void(T const&)>;
    using LockGuard = std::lock_guard<std::mutex>;
    using Mux = std::mutex;
  public:
    //! \brief Constructor sets callbacks as empty
    CallbackQueue() : _add_callback([](auto){}), _remove_callback([](auto){}) { }

    //! \brief Set the callback for adding an element
    void set_add_callback(CallbackType const& cb) { _add_callback = cb; }
    //! \brief Set the callback for removing an element
    void set_remove_callback(CallbackType const& cb) { _remove_callback = cb; }

    //! \brief Add an element
    void add(T const& e) { _add_callback(e); LockGuard lock(_mux); _deque.push_back(e); }
    //! \brief The least recent element added
    T const& front() const { ARIADNE_PRECONDITION(not empty()) LockGuard lock(_mux); return _deque.front(); }
    //! \brief The most recent element added
    T const& back() const { ARIADNE_PRECONDITION(not empty()) LockGuard lock(_mux); return _deque.back(); }
    //! \brief Remove an element
    void remove() { ARIADNE_PRECONDITION(not empty()) LockGuard lock(_mux); _remove_callback(_deque.front()); _deque.pop_front(); }
    //! \brief The size of the queue
    SizeType size() const { LockGuard lock(_mux); return _deque.size(); }
    //! \brief If the queue is empty
    bool empty() const { return this->size() == 0; }

  private:
    mutable Mux _mux;
    std::deque<T> _deque;
    CallbackType _add_callback;
    CallbackType _remove_callback;
};

}

#endif //OPERA_CALLBACK_QUEUE_HPP
