/***************************************************************************
 *            stopwatch.hpp
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

#ifndef OPERA_STOPWATCH_HPP
#define OPERA_STOPWATCH_HPP

#include <chrono>

namespace Opera {

using Seconds = std::chrono::seconds;
using Milliseconds = std::chrono::milliseconds;
using Microseconds = std::chrono::microseconds;

template<class D> class Stopwatch {
public:
    using ResolutionType = std::chrono::high_resolution_clock;
    using TimePointType = std::chrono::time_point<ResolutionType>;

    Stopwatch() { restart(); }

    //! \brief Get the duration in the given type
    D duration() const { return std::chrono::duration_cast<D>(_clicked-_initial); }
    //! \brief Get the duration in seconds, in double precision
    double elapsed_seconds() const { return std::chrono::duration_cast<std::chrono::duration<double>>(duration()).count(); }

    //! \brief Restart the watch time to zero
    Stopwatch& restart() { _initial = ResolutionType::now(); _clicked = _initial; return *this; }
    //! \brief Save the current time
    Stopwatch& click() { _clicked = ResolutionType::now(); return *this; }

private:
    TimePointType _initial;
    TimePointType _clicked;
};

} // namespace Opera

#endif /* OPERA_STOPWATCH_HPP */
