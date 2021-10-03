/***************************************************************************
 *            barrier.cpp
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

#include "macros.hpp"
#include "barrier.hpp"

namespace Opera {

MinimumDistanceBarrier::MinimumDistanceBarrier(PositiveFloatType const& minimum_distance, DiscreteState const& farthest_location, SizeType const& maximum_index) :
    _minimum_distance(minimum_distance), _farthest_location(farthest_location), _maximum_index(maximum_index) { }

PositiveFloatType const& MinimumDistanceBarrier::minimum_distance() const {
    return _minimum_distance;
}

DiscreteState const& MinimumDistanceBarrier::farthest_location() const {
    return _farthest_location;
}

SizeType const& MinimumDistanceBarrier::maximum_index() const {
    return _maximum_index;
}

void MinimumDistanceBarrier::set_farthest_location(DiscreteState const& location) {
    _farthest_location = location;
}

void MinimumDistanceBarrier::increase_maximum_index() {
    _maximum_index++;
}

void MinimumDistanceBarrier::reset_maximum_index() {
    _maximum_index = 0;
}

std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrier const& s) {
        return os << "(d:" << s.minimum_distance() << ", l:" << s.farthest_location() << ", i:" << s.maximum_index() << ")";
}

MinimumDistanceBarrierTrace::MinimumDistanceBarrierTrace(BodySegmentSample const& human_sample, IdType const& robot_segment_id) :
    _human_segment_id(human_sample.segment_id()), _robot_segment_id(robot_segment_id),
    _spherical_approximation(human_sample.spherical_approximation()),
    _next_index(0), _barriers({}) { }

IdType const& MinimumDistanceBarrierTrace::human_segment_id() const {
    return _human_segment_id;
}

IdType const& MinimumDistanceBarrierTrace::robot_segment_id() const {
    return _robot_segment_id;
}

MinimumDistanceBarrier const& MinimumDistanceBarrierTrace::barrier(SizeType const& idx) const {
    return _barriers.at(idx);
}

SizeType MinimumDistanceBarrierTrace::size() const {
    return _barriers.size();
}

void MinimumDistanceBarrierTrace::add_barrier(DiscreteState const& farthest_location, PositiveFloatType const& minimum_distance) {
    _barriers.push_back(MinimumDistanceBarrier(minimum_distance,farthest_location,_next_index));
}

void MinimumDistanceBarrierTrace::remove_barrier() {
    OPERA_PRECONDITION(not is_empty())
    _barriers.pop_front();
}

bool MinimumDistanceBarrierTrace::try_update_with(DiscreteState const& location, BodySegmentSample const& segment_sample) {
    auto current_distance = distance(_spherical_approximation,segment_sample);
    if (current_distance > 0) {
        if (current_distance<current_minimum_distance()) {
            add_barrier(location,current_distance);
        } else {
            if (location != _barriers.back().farthest_location()) {
                _barriers.back().set_farthest_location(location);
                _barriers.back().reset_maximum_index();
            } else
                _barriers.back().increase_maximum_index();
        }
        ++_next_index;
        return true;
    }
    return false;
}

int MinimumDistanceBarrierTrace::resume_element(SphericalApproximationSample const& other) const {
    if (is_empty()) return -1;
    auto deviation = distance(_spherical_approximation.centre(),other.centre());
    auto radius_difference = other.radius() - _spherical_approximation.radius();
    if (radius_difference > 0) deviation += radius_difference;
    SizeType lower=0;
    SizeType upper=_barriers.size()-1;
    if (deviation > _barriers.at(lower).minimum_distance()) return -1;
    if (deviation <= _barriers.at(upper).minimum_distance()) return static_cast<int>(upper);
    SizeType result = (upper+lower)/2;
    while (upper>lower+1) {
        if (deviation > _barriers.at(result).minimum_distance()) upper = result;
        else lower = result;
        result = (upper+lower)/2;
    }
    return static_cast<int>(result);
}

void MinimumDistanceBarrierTrace::reset(BodySegmentSample const& human_sample, RobotStateHistory const& history) {
    OPERA_ASSERT(human_sample.segment_id() == _human_segment_id)
    auto sas = human_sample.spherical_approximation();
    int resume = resume_element(sas);
    _next_index = (resume >= 0 ? _barriers.at(static_cast<SizeType>(resume)).maximum_index()+1 : 0);
    _spherical_approximation = sas;
    if (_next_index > 0) {
        auto location = _barriers.at(static_cast<SizeType>(resume)).farthest_location();
        _barriers.clear();
        _barriers.emplace_back(MinimumDistanceBarrier(
                distance(_spherical_approximation,history.samples(location).at(_robot_segment_id).at(_next_index-1)),
                location,_next_index-1));
    } else {
        _barriers.clear();
    }
}

bool MinimumDistanceBarrierTrace::is_empty() const {
    return _barriers.empty();
}

PositiveFloatType const& MinimumDistanceBarrierTrace::current_minimum_distance() const {
    if (is_empty()) return infinity;
    else return _barriers.back().minimum_distance();
}

SizeType const& MinimumDistanceBarrierTrace::next_index() const {
    return _next_index;
}

std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrierTrace const& t) {
    List<MinimumDistanceBarrier> barriers;
    for (auto b : t._barriers) barriers.push_back(b);
    return os << barriers;
}

}
