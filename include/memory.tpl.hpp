/***************************************************************************
 *            memory.tpl.hpp
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

#ifndef OPERA_MEMORY_TPL_HPP
#define OPERA_MEMORY_TPL_HPP

#include "memory.hpp"

namespace Opera {

template<> BodyPresentationPacket const& MemoryBroker::get<BodyPresentationPacket>(SizeType const& idx) const { std::lock_guard<std::mutex> lock(_mux); return _body_presentations.at(idx); }
template<> BodyStatePacket const& MemoryBroker::get<BodyStatePacket>(SizeType const& idx) const { std::lock_guard<std::mutex> lock(_mux); return _body_states.at(idx); }
template<> CollisionNotificationPacket const& MemoryBroker::get<CollisionNotificationPacket>(SizeType const& idx) const { std::lock_guard<std::mutex> lock(_mux); return _collision_notifications.at(idx); }

template<> SizeType MemoryBroker::size<BodyPresentationPacket>() const { std::lock_guard<std::mutex> lock(_mux); return _body_presentations.size(); }
template<> SizeType MemoryBroker::size<BodyStatePacket>() const { std::lock_guard<std::mutex> lock(_mux); return _body_states.size(); }
template<> SizeType MemoryBroker::size<CollisionNotificationPacket>() const { std::lock_guard<std::mutex> lock(_mux); return _collision_notifications.size(); }

}

#endif // OPERA_MEMORY_TPL_HPP