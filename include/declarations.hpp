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

#include <cstring>
#include <numeric>
#include <vector>

namespace Opera {

typedef double FloatType;
typedef FloatType PositiveFloatType;
template<class T> using List = std::vector<T>;
template<class T1, class T2> using Pair = std::pair<T1,T2>;

using SizeType = size_t;

static const FloatType infinity = std::numeric_limits<FloatType>::infinity();

}

#endif //OPERA_DECLARATIONS_HPP
