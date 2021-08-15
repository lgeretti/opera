/***************************************************************************
 *            body.cpp
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
#include "body.hpp"

using Ariadne::StringStream;

namespace Opera {

Body::Body(IdType const& id, BodyType const& type, SizeType const& package_frequency, List<IdType> const& points_ids, List<FloatType> const& thicknesses) :
    _id(id), _type(type), _package_frequency(package_frequency) {
    ARIADNE_ASSERT_MSG(package_frequency > 0, "The package frequency must be strictly positive")
    ARIADNE_ASSERT_MSG(points_ids.size() == thicknesses.size()*2, "The point ids must be twice the thicknesses")

    for (List<IdType>::size_type i=0; i<thicknesses.size(); ++i)
        _segments.append(BodySegment(this,i,points_ids.at(2*i),points_ids.at(2*i+1),thicknesses.at(i)));
}

IdType const& Body::id() const {
    return _id;
}

BodyType const& Body::type() const {
    return _type;
}

SizeType const& Body::package_frequency() const {
    return _package_frequency;
}

List<BodySegment> const& Body::segments() const {
    return _segments;
}

BodyStateHistory Body::make_history() const {
    return BodyStateHistory(this);
}

BodyStatePackage::BodyStatePackage(DiscreteLocation const& location, List<List<Point>> const& points, TimestampType const& timestamp) :
    _location(location), _points(points), _timestamp(timestamp) {}

DiscreteLocation const& BodyStatePackage::location() const {
    return _location;
}
List<List<Point>> const& BodyStatePackage::points() const {
    return _points;
}
TimestampType const& BodyStatePackage::timestamp() const {
    return _timestamp;
}

DiscreteTransitionData::DiscreteTransitionData(DiscreteLocation const& source, TimestampType const& timestamp) :
    _source(source), _timestamp(timestamp) { }

DiscreteLocation const& DiscreteTransitionData::source() const {
    return _source;
}

TimestampType const& DiscreteTransitionData::timestamp() const {
    return _timestamp;
}

BodyStateHistory::BodyStateHistory(Body const* body) : _body(body) { }

DiscreteLocation const& BodyStateHistory::current_location() const {
    return _current_location;
}

bool BodyStateHistory::has_samples(DiscreteLocation const& location) const {
    return _location_states.find(location) != _location_states.end();
}

auto BodyStateHistory::samples(DiscreteLocation const& location) const -> BodySamplesType const& {
    return _location_states.get(location);
}

auto BodyStateHistory::entrances(DiscreteLocation const& location) const -> EntrancesQueueType const& {
    return _location_entrances.get(location);
}

SizeType BodyStateHistory::_update_index(TimestampType const& timestamp) const {
    return floor((timestamp-_location_entrances.get(_current_location).back().timestamp())/1000*_body->package_frequency());
}

void BodyStateHistory::acquire(BodyStatePackage const& state) {
    /*
     * 1) If the location is different from the current one (including the first location inserted)
     *   a) Put the buffered content (if it exists) in place of the current location content
     *   b) Create a new empty buffer
     *   c) Add the entrance (or create from scratch if a new location)
     *   d) Update the current location
     * 3) Check if the location has a history
     *   a) If not, adding each segment sample to the buffer
     *   b) If it has, identify the index from the timestamp and update the sample on the corresponding entry, adding it to the buffer
     */

    if (_current_location.values().empty() or _current_location != state.location()) {
        if (not _current_location_states_buffer.empty())
            _location_states[_current_location] = std::move(_current_location_states_buffer);
        _current_location_states_buffer = List<List<BodySegmentSample>>();
        for (SizeType i=0; i<_body->segments().size(); ++i)
            _current_location_states_buffer.push_back(List<BodySegmentSample>());

        if (_location_states.find(state.location()) == _location_states.end()) {
            auto timestamps = std::deque<DiscreteTransitionData>();
            timestamps.push_back(DiscreteTransitionData(_current_location,state.timestamp()));
            _location_entrances.insert(make_pair(state.location(), timestamps));
        } else {
            _location_entrances[state.location()].push_back(DiscreteTransitionData(_current_location,state.timestamp()));
        }

        _current_location = state.location();
    }

    auto segments = _body->segments();
    bool has_history_for_location = (_location_states.find(_current_location) != _location_states.end());
    SizeType j0 = (has_history_for_location ? 0 : 1);
    for (SizeType i=0; i<segments.size(); ++i) {
        auto head_pts = state.points().at(segments.at(i).head_id());
        auto tail_pts = state.points().at(segments.at(i).tail_id());
        BodySegmentSample s = (has_history_for_location ? _location_states[_current_location].at(i).at(_update_index(state.timestamp())) :
                                                          segments.at(i).create_sample(head_pts.at(0),tail_pts.at(0)));
        auto common_size = std::min(head_pts.size(),tail_pts.size());
        for (SizeType j=j0; j<common_size; ++j)
            s.update(head_pts.at(j),tail_pts.at(j));
        for (SizeType j=common_size; j<head_pts.size(); ++j)
            s.update_head(head_pts.at(j));
        for (SizeType j=common_size; j<tail_pts.size(); ++j)
            s.update_tail(tail_pts.at(j));
        _current_location_states_buffer.at(i).push_back(s);
    }
}

BodySegment::BodySegment(Body const* body, IdType const& id, IdType const& head_id, IdType const& tail_id, FloatType const& thickness) :
    _body(body), _id(id), _head_id(head_id), _tail_id(tail_id), _thickness(thickness) { }

IdType BodySegment::id() const {
    return _id;
}

IdType BodySegment::head_id() const {
    return _head_id;
}

IdType BodySegment::tail_id() const {
    return _tail_id;
}

FloatType const& BodySegment::thickness() const {
    return _thickness;
}

BodySegmentSample BodySegment::create_sample(Point const& begin, Point const& end) const {
    return BodySegmentSample(this, begin, end);
}

BodySegmentSample::BodySegmentSample(BodySegment const* segment, Point const& head, Point const& tail) :
        _head_bounds({IntervalType(head.x), IntervalType(head.y), IntervalType(head.z)}),
        _tail_bounds({IntervalType(tail.x), IntervalType(tail.y), IntervalType(tail.z)}),
        _head_centre(head), _tail_centre(tail), _radius(0.0, Ariadne::dp), _segment(segment) {
    _bb = Ariadne::widen(Ariadne::hull(_head_bounds,_tail_bounds),_segment->thickness());
}

Point const& BodySegmentSample::head_centre() const {
    return _head_centre;
}

Point const& BodySegmentSample::tail_centre() const {
    return _tail_centre;
}

FloatType BodySegmentSample::radius() const {
    return _radius;
}

void BodySegmentSample::update(Point const& head, Point const& tail) {
    BoundingType nhb(3), ntb(3);
    nhb[0]=hull(_head_bounds[0],head.x);
    nhb[1]=hull(_head_bounds[1],head.y);
    nhb[2]=hull(_head_bounds[2],head.z);
    ntb[0]=hull(_tail_bounds[0],tail.x);
    ntb[1]=hull(_tail_bounds[1],tail.y);
    ntb[2]=hull(_tail_bounds[2],tail.z);
    _head_bounds = nhb;
    _tail_bounds = ntb;
    recalculate_centers_radius_bb();
}

void BodySegmentSample::update_head(Point const& head) {
    BoundingType nhb(3);
    nhb[0]=hull(_head_bounds[0],head.x);
    nhb[1]=hull(_head_bounds[1],head.y);
    nhb[2]=hull(_head_bounds[2],head.z);
    _head_bounds = nhb;
    recalculate_centers_radius_bb();
}

void BodySegmentSample::update_tail(Point const& tail) {
    BoundingType ntb(3);
    ntb[0]=hull(_tail_bounds[0],tail.x);
    ntb[1]=hull(_tail_bounds[1],tail.y);
    ntb[2]=hull(_tail_bounds[2],tail.z);
    _tail_bounds = ntb;
    recalculate_centers_radius_bb();
}

void BodySegmentSample::recalculate_centers_radius_bb() {
    auto hc = _head_bounds.centre();
    auto tc = _tail_bounds.centre();
    _head_centre = Point(hc.at(0), hc.at(1), hc.at(2));
    _tail_centre = Point(tc.at(0), tc.at(1), tc.at(2));
    _radius = max(_head_bounds.radius(),_tail_bounds.radius());
    _bb = widen(hull(_head_bounds,_tail_bounds),_segment->thickness());
}

BoundingType const& BodySegmentSample::bounding_box() const {
    return _bb;
}

bool BodySegmentSample::intersects(BodySegmentSample const& other) const {
    if (decide(_bb.disjoint(other.bounding_box()))) return false;
    else {
        return (decide(distance(*this,other) <= _segment->thickness()+_radius + other._segment->thickness()+other._radius));
    }
}

FloatType distance(BodySegmentSample const& s1, BodySegmentSample const& s2) {
    return distance(s1.head_centre(), s1.tail_centre(), s2.head_centre(), s2.tail_centre());
}

}