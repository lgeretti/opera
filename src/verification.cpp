/***************************************************************************
 *            verification.cpp
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

#include "verification.hpp"

namespace Opera {

MinimumDistanceBarrier::MinimumDistanceBarrier(PositiveFloatType const& minimum_distance, SizeType const& maximum_index) :
    _minimum_distance(minimum_distance), _maximum_index(maximum_index) { }

PositiveFloatType const& MinimumDistanceBarrier::minimum_distance() const {
    return _minimum_distance;
}

SizeType const& MinimumDistanceBarrier::maximum_index() const {
    return _maximum_index;
}

void MinimumDistanceBarrier::increase_maximum_index() {
    _maximum_index++;
}

std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrier const& s) {
    return os << "(d:" << s.minimum_distance() << ",i:" << s.maximum_index() << ")";
}

MinimumDistanceBarrierTrace::MinimumDistanceBarrierTrace(SphericalApproximationSample const& sample, SizeType const& starting_next_index) :
    _spherical_approximation(sample), _next_index(starting_next_index), _barriers({}) { }

List<MinimumDistanceBarrier> const& MinimumDistanceBarrierTrace::barriers() const {
    return _barriers;
}

void MinimumDistanceBarrierTrace::add_barrier(PositiveFloatType const& minimum_distance) {
    _barriers.append(MinimumDistanceBarrier(minimum_distance,_next_index));
}

bool MinimumDistanceBarrierTrace::try_check(BodySegmentSample const& segment_sample) {
    auto current_distance = distance(_spherical_approximation,segment_sample);
    if (decide(current_distance != 0)) {
        if (decide(current_distance<current_minimum_distance())) {
            add_barrier(current_distance);
        } else {
            _barriers.back().increase_maximum_index();
        }
        ++_next_index;
        return true;
    }
    return false;
}

bool MinimumDistanceBarrierTrace::is_empty() const {
    return _barriers.empty();
}

void MinimumDistanceBarrierTrace::clear() {
    _next_index = 0;
    _barriers.clear();
}

PositiveFloatType const& MinimumDistanceBarrierTrace::current_minimum_distance() const {
    if (is_empty()) return pa_infty;
    else return _barriers.back().minimum_distance();
}

SizeType const& MinimumDistanceBarrierTrace::next_index() const {
    return _next_index;
}

SizeType MinimumDistanceBarrierTrace::resume_index(SphericalApproximationSample const& sample) const {
    return 0;
}

std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrierTrace const& t) {
    return os << t.barriers();
}

}
