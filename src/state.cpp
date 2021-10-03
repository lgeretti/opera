/***************************************************************************
 *            state.cpp
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

#include <math.h>
#include "macros.hpp"
#include "state.hpp"

namespace Opera {

HumanStateInstance::HumanStateInstance(Human const& human, List<List<Point>> const& points, TimestampType const& timestamp) : _timestamp(timestamp) {
    OPERA_PRECONDITION(human.num_points() == points.size())
    for (SizeType i=0; i<human.num_segments(); ++i) {
        auto const& segment = human.segment(i);
        auto head_pts = points.at(segment.head_id());
        auto tail_pts = points.at(segment.tail_id());
        BodySegmentSample sample = segment.create_sample();
        sample.update(head_pts,tail_pts);
        _samples.push_back(sample);
    }
}

List<BodySegmentSample> const& HumanStateInstance::samples() const {
    return _samples;
}

TimestampType const& HumanStateInstance::timestamp() const {
    return _timestamp;
}

RobotLocationPresence::RobotLocationPresence(DiscreteState const& location, DiscreteState const& exit_destination, TimestampType const& from, TimestampType const& to) :
_location(location), _exit_destination(exit_destination), _from(from), _to(to) { }

DiscreteState const& RobotLocationPresence::location() const {
    return _location;
}

DiscreteState const& RobotLocationPresence::exit_destination() const {
    return _exit_destination;
}

TimestampType const& RobotLocationPresence::from() const {
    return _from;
}

TimestampType const& RobotLocationPresence::to() const {
    return _to;
}

std::ostream& operator<<(std::ostream& os, RobotLocationPresence const& p) {
    return os << "(in '" << p.location() << "' from " << p.from() << " to " << p.to() << ", exit to '" << p.exit_destination() << "')";
}

RobotDiscreteTrace::RobotDiscreteTrace() : _probability(1.0) { }

RobotDiscreteTrace::RobotDiscreteTrace(RobotDiscreteTrace const& other) :
    _locations(other._locations), _probability(other._probability), _next_locations(other._next_locations) { }

RobotDiscreteTrace& RobotDiscreteTrace::push_front(DiscreteState const& location) {
    _locations.push_front(location);
    _next_locations.clear();
    return *this;
}

RobotDiscreteTrace& RobotDiscreteTrace::push_back(DiscreteState const& location, PositiveFloatType const& probability) {
    _locations.push_back(location);
    _next_locations.clear();
    _probability *= probability;
    return *this;
}

SizeType RobotDiscreteTrace::size() const {
    return _locations.size();
}

DiscreteState RobotDiscreteTrace::at(SizeType const& idx) const {
    OPERA_PRECONDITION(idx < _locations.size())
    return _locations.at(idx);
}

PositiveFloatType const& RobotDiscreteTrace::probability() const {
    return _probability;
}

bool RobotDiscreteTrace::operator==(RobotDiscreteTrace const& other) const {
    return this->_locations == other._locations;
}

RobotDiscreteTrace& RobotDiscreteTrace::operator=(RobotDiscreteTrace const& other) {
    _locations = other._locations;
    _probability = other._probability;
    _next_locations = other._next_locations;
    return *this;
}

std::ostream& operator<<(std::ostream& os, RobotDiscreteTrace const& t) {
    List<DiscreteState> locations;
    for (auto l : t._locations)
        locations.push_back(l);
    return os << "{" << locations << ", p:" << t._probability << "}";
}

struct RobotDiscreteTraceBacktracking {
    RobotDiscreteTraceBacktracking(SizeType const& index_, RobotDiscreteTrace const& trace_, DiscreteState const& _next_location, bool const& _is_valid)
        : index(index_), trace(trace_), next_location(_next_location), is_valid(_is_valid) { }
    SizeType index;
    RobotDiscreteTrace trace;
    DiscreteState next_location;
    bool is_valid;
};

Map<DiscreteState,PositiveFloatType> const& RobotDiscreteTrace::next_locations() const {
    if (_next_locations.empty()) {
        List<RobotDiscreteTraceBacktracking> tracking;
        for (SizeType i=0; i<_locations.size()-1; ++i)
            if (_locations.at(i) == _locations.back())
                tracking.push_back(RobotDiscreteTraceBacktracking(i,RobotDiscreteTrace().push_back(_locations.at(i)),_locations.at(i+1),true));
        SizeType maximum_trace_size = 0;
        SizeType num_having_maximum_trace_size = 0;
        SizeType num_valid = tracking.size();
        while (num_valid > 0) {
            maximum_trace_size++;
            num_having_maximum_trace_size = num_valid;
            for (auto& st : tracking) {
                if (st.is_valid) {
                    if (st.index > 0) {
                        st.index--;
                        if (_locations.at(st.index) == _locations.at(_locations.size() - 1 - maximum_trace_size)) {
                            st.trace.push_front(_locations.at(st.index));
                        } else {
                            num_valid--;
                            st.is_valid = false;
                        }
                    } else {
                        num_valid--;
                        st.is_valid = false;
                    }
                }
            }
        }
        for (auto t : tracking) {
            if (t.trace.size() == maximum_trace_size) {
                if (_next_locations.has_key(t.next_location))
                    _next_locations.at(t.next_location) += 1.0;
                else
                    _next_locations.insert(std::make_pair(t.next_location,1.0));
            }
        }
        for (auto& l : _next_locations) {
            l.second = _probability*l.second/num_having_maximum_trace_size;
        }
    }
    return _next_locations;
}

RobotStateHistory::RobotStateHistory(Robot const* robot) :
    _current_location_states_buffer(List<List<BodySegmentSample>>()), _robot(robot) {
    for (SizeType i=0; i < _robot->num_segments(); ++i)
        _current_location_states_buffer.push_back(List<BodySegmentSample>());
}

DiscreteState const& RobotStateHistory::current_location() const {
    return _current_location;
}

RobotDiscreteTrace RobotStateHistory::discrete_trace() const {
    RobotDiscreteTrace trace;
    for (auto const& p : _location_presences)
        trace.push_back(p.exit_destination());
    return trace;
}

bool RobotStateHistory::has_samples(DiscreteState const& location) const {
    return _location_states.has_key(location);
}

auto RobotStateHistory::samples(DiscreteState const& location) const -> BodySamplesType const& {
    return _location_states.at(location);
}

List<RobotLocationPresence> RobotStateHistory::presences_in(DiscreteState const& location) const {
    List<RobotLocationPresence> result;
    for (auto const& p : _location_presences)
        if ((not p.location().values().empty()) and p.location() == location)
            result.push_back(p);
    return result;
}

List<RobotLocationPresence> RobotStateHistory::presences_between(DiscreteState const& source, DiscreteState const& destination) const {
    List<RobotLocationPresence> result;
    for (auto const& p : _location_presences)
        if ((not p.location().values().empty()) and p.location() == source and p.exit_destination() == destination)
            result.push_back(p);
    return result;
}

List<RobotLocationPresence> RobotStateHistory::presences_exiting_into(DiscreteState const& location) const {
    List<RobotLocationPresence> result;
    for (auto const& p : _location_presences)
        if (p.exit_destination() == location)
            result.push_back(p);
    return result;
}

Interval<SizeType> RobotStateHistory::_range_of_num_samples_within(List<RobotLocationPresence> const& presences) const {
    if (presences.empty())
        return Interval<SizeType>(0u,0u);

    SizeType min_value = std::numeric_limits<SizeType>::max();
    SizeType max_value = 0;
    for (auto p : presences) {
        auto val = static_cast<SizeType>(floor(static_cast<double>(p.to()-p.from())/1e9*_robot->packet_frequency()));
        min_value = std::min(min_value,val);
        max_value = std::max(max_value,val);
    }
    return Interval<SizeType>(min_value,max_value);
}

Interval<SizeType> RobotStateHistory::range_of_num_samples_in(DiscreteState const& location) const {
    return _range_of_num_samples_within(presences_in(location));
}

Interval<SizeType> RobotStateHistory::range_of_num_samples_between(DiscreteState const& source, DiscreteState const& target) const {
    return _range_of_num_samples_within(presences_between(source,target));
}

SizeType RobotStateHistory::_update_index(TimestampType const& timestamp) const {
    return floor(double(timestamp- _location_presences.back().to()) / 1e9 * FloatType(_robot->packet_frequency()));
}

void RobotStateHistory::acquire(DiscreteState const& location, List<List<Point>> const& points, TimestampType const& timestamp) {
    /*
     * 1) If the location is different from the current one (including the first location inserted)
     *   a) Save the buffered content
     *   b) Create a new is_empty buffered content
     *   c) Add the location presence
     *   d) Update the current location
     * 3) Check if the location has a history
     *   a) If not, adding each segment sample to the buffer
     *   b) If it has, identify the index from the timestamp and update the sample on the corresponding entry, adding it to the buffer
     */
    OPERA_ASSERT(points.size() == _robot->num_points())

    if (_current_location.values().empty() or _current_location != location) {
        _location_states[_current_location] = std::move(_current_location_states_buffer);

        _current_location_states_buffer = List<List<BodySegmentSample>>();
        for (SizeType i=0; i < _robot->num_segments(); ++i)
            _current_location_states_buffer.push_back(List<BodySegmentSample>());

        TimestampType entrance_timestamp = (_location_presences.empty() ? timestamp : _location_presences.back().to());
        _location_presences.push_back(RobotLocationPresence(_current_location, location, entrance_timestamp, timestamp));

        _current_location = location;
    }

    bool has_history_for_location = _location_states.has_key(_current_location);
    SizeType update_idx = _update_index(timestamp);
    SizeType updating_sample = (has_history_for_location and _location_states[_current_location].at(0).size() > update_idx);
    for (SizeType i=0; i<_robot->num_segments(); ++i) {
        auto head_pts = points.at(_robot->segment(i).head_id());
        auto tail_pts = points.at(_robot->segment(i).tail_id());
        BodySegmentSample s = (updating_sample ? _location_states[_current_location].at(i).at(update_idx) : _robot->segment(i).create_sample());
        s.update(head_pts,tail_pts);
        _current_location_states_buffer.at(i).push_back(s);
    }
}

}