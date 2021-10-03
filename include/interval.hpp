/***************************************************************************
 *            interval.hpp
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
