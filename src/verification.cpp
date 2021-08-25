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

bool MinimumDistanceBarrierTrace::try_update_with(BodySegmentSample const& segment_sample) {
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

SizeType MinimumDistanceBarrierTrace::resume_index(SphericalApproximationSample const& other) const {
    if (is_empty()) return 0;
    auto deviation = distance(_spherical_approximation.centre(),other.centre());
    auto radius_difference = other.radius() - _spherical_approximation.radius();
    if (decide(radius_difference > 0)) deviation += radius_difference;
    SizeType lower=0;
    SizeType upper=_barriers.size()-1;
    if (decide(deviation > _barriers.at(lower).minimum_distance())) return 0;
    if (decide(deviation <= _barriers.at(upper).minimum_distance())) return _barriers.at(upper).maximum_index()+1;
    SizeType result = (upper+lower)/2;
    while (upper>lower+1) {
        if (decide(deviation > _barriers.at(result).minimum_distance())) upper = result;
        else lower = result;
        result = (upper+lower)/2;
    }
    return _barriers.at(result).maximum_index()+1;
}

void MinimumDistanceBarrierTrace::reset(BodySegmentSample const& human_sample, List<BodySegmentSample> const& robot_samples) {
    auto sas = human_sample.spherical_approximation();
    _next_index = resume_index(sas);
    _spherical_approximation = sas;
    _barriers.clear();
    if (_next_index > 0) _barriers.append(MinimumDistanceBarrier(distance(_spherical_approximation,robot_samples.at(_next_index-1)),_next_index-1));
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

std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrierTrace const& t) {
    return os << t.barriers();
}

}
