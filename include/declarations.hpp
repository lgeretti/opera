/***************************************************************************
 *            declarations.hpp
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

#ifndef OPERA_DECLARATIONS_HPP
#define OPERA_DECLARATIONS_HPP

#include <ariadne.hpp>

namespace Opera {

typedef Ariadne::FloatDPApproximation FloatType;
typedef Ariadne::Positive<FloatType> PositiveFloatType;
typedef Ariadne::Interval<FloatType> FloatIntervalType;
typedef Ariadne::Box<FloatIntervalType> BoundingType;

using Ariadne::SizeType;
using Ariadne::dp;
using Ariadne::Interval;
using Ariadne::Natural;
using Ariadne::StringStream;

}

#endif //OPERA_DECLARATIONS_HPP
