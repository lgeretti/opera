/***************************************************************************
 *            thread.hpp
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

#ifndef OPERA_THREAD_HPP
#define OPERA_THREAD_HPP

#include <utility>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <functional>

namespace Opera {

//! \brief A class for handling a thread for a pool in a smarter way.
//! \details It allows to wait for the start of the \a task before extracting the thread id, which is held along with
//! a readable \a name.
class Thread {
  public:

    //! \brief Construct with an optional name
    //! \details The thread will start and store the id
    Thread(std::function<void()> task, std::string name = std::string());

    //! \brief Get the thread id
    std::thread::id id() const;
    //! \brief Get the readable name
    std::string name() const;

    //! \brief Destroy the instance
    ~Thread();

  private:
    std::string _name;
    std::thread::id _id;
    std::promise<void> _got_id_promise;
    std::future<void> _got_id_future;
    std::promise<void> _registered_thread_promise;
    std::future<void> _registered_thread_future;
    std::thread _thread;
};

} // namespace Opera

#endif // OPERA_THREAD_HPP
