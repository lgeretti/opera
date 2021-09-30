/***************************************************************************
 *            thread.cpp
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

#include "utility.hpp"
#include "thread.hpp"
#include "logging.hpp"

namespace Opera {

Thread::Thread(std::function<void()> task, std::string name)
        : _name(name), _got_id_future(_got_id_promise.get_future()), _registered_thread_future(_registered_thread_promise.get_future())
{
    _thread = std::thread([=,this]() {
        _id = std::this_thread::get_id();
        _got_id_promise.set_value();
        _registered_thread_future.get();
        task();
    });
    _got_id_future.get();
    if (_name == std::string()) _name = to_string(_id);
    Logger::instance().register_thread(this->id(),this->name());
    _registered_thread_promise.set_value();
}

std::thread::id Thread::id() const {
    return _id;
}

std::string Thread::name() const {
    return _name;
}

Thread::~Thread() {
    _thread.join();
    Logger::instance().unregister_thread(this->id());
}

} // namespace Opera
