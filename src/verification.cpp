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

MinimumDistanceBarrier::MinimumDistanceBarrier(PositiveFloatType const& minimum_distance, SphericalApproximationSample const& sample, SizeType const& maximum_index) :
    _minimum_distance(minimum_distance), _sample(sample), _maximum_index(maximum_index) { }

PositiveFloatType const& MinimumDistanceBarrier::minimum_distance() const {
    return _minimum_distance;
}

SphericalApproximationSample const& MinimumDistanceBarrier::sample() const {
    return _sample;
}

SizeType const& MinimumDistanceBarrier::maximum_index() const {
    return _maximum_index;
}

void MinimumDistanceBarrier::increase_maximum_index() {
    _maximum_index++;
}

std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrier const& s) {
    return os << "(d:" << s.minimum_distance() << ",&s:" << &s.sample() << ",i:" << s.maximum_index() << ")";
}

MinimumDistanceBarrierTrace::MinimumDistanceBarrierTrace() : _barriers({}) { }

List<MinimumDistanceBarrier> const& MinimumDistanceBarrierTrace::barriers() const {
    return _barriers;
}

void MinimumDistanceBarrierTrace::add_barrier(PositiveFloatType const& minimum_distance, SphericalApproximationSample const& sample) {
    SizeType idx = (_barriers.empty() ? 0 : current_index()+1);
    _barriers.append(MinimumDistanceBarrier(minimum_distance,sample,idx));
}

void MinimumDistanceBarrierTrace::apply(SphericalApproximationSample const& spherical_sample, BodySegmentSample const& segment_sample) {
    auto current_distance = distance(spherical_sample,segment_sample);
    if (decide(current_distance != 0)) {
        if (decide(current_distance<current_minimum_distance())) {
            add_barrier(current_distance,spherical_sample);
        } else {
            _barriers.back().increase_maximum_index();
        }
    }
}

bool MinimumDistanceBarrierTrace::is_empty() const {
    return _barriers.empty();
}

void MinimumDistanceBarrierTrace::clear() {
    _barriers.clear();
}

PositiveFloatType const& MinimumDistanceBarrierTrace::current_minimum_distance() const {
    if (is_empty()) return pa_infty;
    else return _barriers.back().minimum_distance();
}

SizeType const& MinimumDistanceBarrierTrace::current_index() const {
    ARIADNE_ASSERT(not is_empty())
    return _barriers.back().maximum_index();
}

std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrierTrace const& t) {
    return os << t.barriers();
}

}
