/***************************************************************************
 *            state.cpp
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

#include <ariadne/utility/macros.hpp>
#include "state.hpp"

using Ariadne::StringStream;
using Ariadne::Map;

namespace Opera {

HumanStateInstance::HumanStateInstance(Human const& human, List<List<Point>> const& points, TimestampType const& timestamp) : _timestamp(timestamp) {
    for (SizeType i=0; i<human.num_segments(); ++i) {
        auto const& segment = human.segment(i);
        auto head_pts = points.at(segment.head_id());
        auto tail_pts = points.at(segment.tail_id());
        BodySegmentSample sample = segment.create_sample(head_pts.at(0), tail_pts.at(0));
        sample.update(head_pts,tail_pts,1);
        _samples.push_back(sample);
    }
}

List<BodySegmentSample> const& HumanStateInstance::samples() const {
    return _samples;
}

TimestampType const& HumanStateInstance::timestamp() const {
    return _timestamp;
}

RobotLocationPresence::RobotLocationPresence(DiscreteLocation const& location, DiscreteLocation const& exit_destination, TimestampType const& from, TimestampType const& to) :
_location(location), _exit_destination(exit_destination), _from(from), _to(to) { }

DiscreteLocation const& RobotLocationPresence::location() const {
    return _location;
}

DiscreteLocation const& RobotLocationPresence::exit_destination() const {
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

RobotDiscreteTraceBuilder& RobotDiscreteTraceBuilder::push_front(DiscreteLocation const& location) {
    _locations.push_front(location);
    return *this;
}

RobotDiscreteTraceBuilder& RobotDiscreteTraceBuilder::push_back(DiscreteLocation const& location) {
    _locations.push_back(location);
    return *this;
}

RobotDiscreteTrace RobotDiscreteTraceBuilder::build() const {
    List<DiscreteLocation> result;
    for (auto l : _locations) result.append(l);
    return result;
}

SizeType RobotDiscreteTraceBuilder::size() const {
    return _locations.size();
}

RobotDiscreteTrace::RobotDiscreteTrace(List<DiscreteLocation> const& locations) : _locations(locations) { }

List<DiscreteLocation> const& RobotDiscreteTrace::locations() const {
    return _locations;
}

bool RobotDiscreteTrace::operator==(RobotDiscreteTrace const& other) const {
    return this->locations() == other.locations();
}

std::ostream& operator<<(std::ostream& os, RobotDiscreteTrace const& t) {
    return os << t.locations();
}

struct RobotDiscreteTraceBacktracking {
    RobotDiscreteTraceBacktracking(SizeType const& index_, RobotDiscreteTraceBuilder const& _trace_builder, DiscreteLocation const& _next_location, bool const& _is_valid)
        : index(index_), trace_builder(_trace_builder), next_location(_next_location), is_valid(_is_valid) { }
    SizeType index;
    RobotDiscreteTraceBuilder trace_builder;
    DiscreteLocation next_location;
    bool is_valid;
};

Map<DiscreteLocation,PositiveFloatType> const& RobotDiscreteTrace::next_locations() const {
    if (_next_locations.empty()) {
        auto tail = RobotDiscreteTraceBuilder().push_back(_locations.back());
        List<RobotDiscreteTraceBacktracking> tracking;
        for (int i=0; i<_locations.size()-1; ++i)
            if (_locations.at(i) == _locations.back())
                tracking.push_back(RobotDiscreteTraceBacktracking(i,RobotDiscreteTraceBuilder().push_back(_locations.at(i)),_locations.at(i+1),true));
        SizeType maximum_trace_size = 0;
        SizeType num_having_maximum_trace_size = 0;
        SizeType num_valid = tracking.size();
        while (num_valid > 0) {
            maximum_trace_size++;
            num_having_maximum_trace_size = num_valid;
            tail.push_front(_locations.at(_locations.size() - 1 - maximum_trace_size));
            auto tail_trace = tail.build();
            for (auto& st : tracking) {
                if (st.is_valid) {
                    if (st.index > 0) {
                        st.index--;
                        if (_locations.at(st.index) == _locations.at(_locations.size() - 1 - maximum_trace_size)) {
                            st.trace_builder.push_front(_locations.at(st.index));
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
        PositiveFloatType probability = cast_positive(FloatType(1.0/num_having_maximum_trace_size,dp));
        for (auto t : tracking) {
            if (t.trace_builder.size() == maximum_trace_size)
                _next_locations.insert(std::make_pair(t.next_location,probability));
        }
    }
    return _next_locations;
}

RobotStateHistory::RobotStateHistory(Robot const* robot) :
    _robot(robot), _current_location_states_buffer(List<List<BodySegmentSample>>()) {
    for (SizeType i=0; i < _robot->num_segments(); ++i)
        _current_location_states_buffer.push_back(List<BodySegmentSample>());
}

DiscreteLocation const& RobotStateHistory::current_location() const {
    return _current_location;
}

RobotDiscreteTrace RobotStateHistory::discrete_trace() const {
    RobotDiscreteTraceBuilder builder;
    for (auto p : _location_presences)
        builder.push_back(p.exit_destination());
    return builder.build();
}

bool RobotStateHistory::has_samples(DiscreteLocation const& location) const {
    return _location_states.has_key(location);
}

auto RobotStateHistory::samples(DiscreteLocation const& location) const -> BodySamplesType const& {
    return _location_states.get(location);
}

List<RobotLocationPresence> RobotStateHistory::presences_in(DiscreteLocation const& location) const {
    List<RobotLocationPresence> result;
    for (auto p : _location_presences)
        if ((not p.location().values().empty()) and p.location() == location)
            result.append(p);
        return result;
}

List<RobotLocationPresence> RobotStateHistory::presences_between(DiscreteLocation const& source, DiscreteLocation const& destination) const {
    List<RobotLocationPresence> result;
    for (auto p : _location_presences)
        if ((not p.location().values().empty()) and p.location() == source and p.exit_destination() == destination)
            result.append(p);
        return result;
}

List<RobotLocationPresence> RobotStateHistory::presences_exiting_into(DiscreteLocation const& location) const {
    List<RobotLocationPresence> result;
    for (auto p : _location_presences)
        if (p.exit_destination() == location)
            result.append(p);
    return result;
}

Interval<Natural> RobotStateHistory::_range_of_num_samples_within(List<RobotLocationPresence> const& presences) const {
    if (presences.empty())
        return Interval<Natural>(0u,0u);

    SizeType min_value = std::numeric_limits<SizeType>::max();
    SizeType max_value = 0;
    for (auto p : presences) {
        auto val = static_cast<SizeType>(floor(static_cast<double>(p.to()-p.from())/1e9*_robot->packet_frequency()));
        min_value = std::min(min_value,val);
        max_value = std::max(max_value,val);
    }
    return Interval<Natural>(min_value,max_value);
}

Interval<Natural> RobotStateHistory::range_of_num_samples_in(DiscreteLocation const& location) const {
    return _range_of_num_samples_within(presences_in(location));
}

Interval<Natural> RobotStateHistory::range_of_num_samples_between(DiscreteLocation const& source, DiscreteLocation const& target) const {
    return _range_of_num_samples_within(presences_between(source,target));
}

SizeType RobotStateHistory::_update_index(TimestampType const& timestamp) const {
    return floor(double(timestamp- _location_presences.back().to()) / 1e9 * _robot->packet_frequency());
}

void RobotStateHistory::acquire(DiscreteLocation const& location, List<List<Point>> const& points, TimestampType const& timestamp) {
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
    SizeType j0 = (has_history_for_location ? 0 : 1);
    SizeType update_idx = _update_index(timestamp);
    SizeType updating_sample = (has_history_for_location and _location_states[_current_location].size() > update_idx);
    for (SizeType i=0; i<_robot->num_segments(); ++i) {
        auto head_pts = points.at(_robot->segment(i).head_id());
        auto tail_pts = points.at(_robot->segment(i).tail_id());
        BodySegmentSample s = (updating_sample ? _location_states[_current_location].at(i).at(update_idx) :
                                   _robot->segment(i).create_sample(head_pts.at(0),tail_pts.at(0)));
        s.update(head_pts,tail_pts,j0);
        _current_location_states_buffer.at(i).push_back(s);
    }
}

}