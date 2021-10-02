/***************************************************************************
 *            interval.hpp
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

#ifndef OPERA_INTERVAL_HPP
#define OPERA_INTERVAL_HPP

#include "declarations.hpp"

namespace Opera {

//! \brief A representation of an interval of values
template<class T> class Interval {
  public:
    //! \brief Construct from fields
    Interval(T const& lower, T const& upper) : _lower(lower), _upper(upper) { }

    //! \brief Get the lower bound
    T const& lower() const { return _lower; }
    //! \brief Get the upper bound
    T const& upper() const { return _upper; }

    //! \brief Set the lower bound
    void set_lower(T const& v) { _lower = v; }
    //! \brief Set the upper bound
    void set_upper(T const& v) { _upper = v; }

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, Interval<T> const& ival) {
        return os << "[" << ival.lower() << ":" << ival.upper() <<"]"; }

  private:
    T _lower;
    T _upper;
};

//! \brief Equality checking for FloatType intervals
inline bool operator==(Interval<FloatType> const& e1, Interval<FloatType> const& e2) { return e1.lower() == e2.lower() and e1.upper() == e2.upper(); }
//! \brief Equqlity checking for SizeType intervals
inline bool operator==(Interval<SizeType> const& e1, Interval<SizeType> const& e2) { return e1.lower() == e2.lower() and e1.upper() == e2.upper(); }

}

#endif //OPERA_INTERVAL_HPP
