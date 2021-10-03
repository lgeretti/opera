/***************************************************************************
 *            handle.hpp
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

#ifndef OPERA_HANDLE_HPP
#define OPERA_HANDLE_HPP

#include <memory>

namespace Opera {

//! \brief A handle for interface implementations
template<class I> class Handle {
  protected:
    mutable std::shared_ptr<I> _ptr;
  public:
    ~Handle() { }
    explicit Handle(I* p) : _ptr(p) { }
    Handle(std::shared_ptr<I> p) : _ptr(p) { }
    template<class T> Handle(const T& t)
        : _ptr(new T(t)) { }
    Handle(const Handle<I>& h) = default;
    Handle(Handle<I>&& h) = default;
    Handle<I>& operator=(const Handle<I>& h) = default;
    Handle<I>& operator=(Handle<I>&& h) = default;
    operator I const& () const { return *_ptr; }
};

} // namespace Opera

#endif // OPERA_HANDLE_HPP
