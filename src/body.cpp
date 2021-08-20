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

Body::Body(IdType const& id, SizeType const& package_frequency, List<IdType> const& points_ids, List<FloatType> const& thicknesses) :
    _id(id), _package_frequency(package_frequency) {
    ARIADNE_ASSERT_MSG(package_frequency > 0, "The package frequency must be strictly positive")
    ARIADNE_ASSERT_MSG(points_ids.size() == thicknesses.size()*2, "The point ids must be twice the thicknesses")

    for (List<IdType>::size_type i=0; i<thicknesses.size(); ++i)
        _segments.append(new BodySegment(this,i,points_ids.at(2*i),points_ids.at(2*i+1),thicknesses.at(i)));
}

Body::~Body() {
    for (auto s: _segments)
        delete(s);
}

IdType const& Body::id() const {
    return _id;
}

SizeType const& Body::package_frequency() const {
    return _package_frequency;
}

BodySegment const& Body::segment(SizeType const& idx) const {
    return *_segments.at(idx);
}

SizeType Body::num_segments() const {
    return _segments.size();
}

std::ostream& operator<<(std::ostream& os, Body const& b) {
    os << "(id=" << b.id() << ", package_frequency=" << b.package_frequency() << ", segments=[";
    for (SizeType i=0; i<b.num_segments()-1; ++i) {
        os << b.segment(i) << ",";
    }
    os << b.segment(b.num_segments()-1) << "])";
    return os;
}

Human::Human(IdType const& id, SizeType const& package_frequency, List<IdType> const& points_ids, List<FloatType> const& thicknesses) :
    Body(id,package_frequency,points_ids,thicknesses) { }

HumanStateInstance Human::make_instance(HumanStatePackage const& pkg) const {

    List<BodySegmentSample> samples;

    auto segments = _segments;
    for (auto segment : _segments) {
        auto head_pts = pkg.points().at(segment->head_id());
        auto tail_pts = pkg.points().at(segment->tail_id());
        BodySegmentSample sample = segment->create_sample(head_pts.at(0),tail_pts.at(0));
        sample.update(head_pts,tail_pts,1);
        samples.push_back(sample);
    }

    return HumanStateInstance(this, samples, pkg.timestamp());
}

Robot::Robot(IdType const& id, SizeType const& package_frequency, List<IdType> const& points_ids, List<FloatType> const& thicknesses) :
    Body(id,package_frequency,points_ids,thicknesses) { }

RobotStateHistory Robot::make_history() const {
    return RobotStateHistory(this);
}

BodyStatePackage::BodyStatePackage(List<List<Point>> const& points, TimestampType const& timestamp) :
    _points(points), _timestamp(timestamp) {}

RobotStatePackage::RobotStatePackage(DiscreteLocation const& location, List<List<Point>> const& points, TimestampType const& timestamp) :
    BodyStatePackage(points,timestamp), _location(location) {}

HumanStatePackage::HumanStatePackage(List<List<Point>> const& points, TimestampType const& timestamp) :
    BodyStatePackage(points,timestamp) {}

List<List<Point>> const& BodyStatePackage::points() const {
    return _points;
}

TimestampType const& BodyStatePackage::timestamp() const {
    return _timestamp;
}

DiscreteLocation const& RobotStatePackage::location() const {
    return _location;
}

HumanStateInstance::HumanStateInstance(Human const* human, List<BodySegmentSample> const& samples, TimestampType const& timestamp) :
    _samples(samples), _timestamp(timestamp), _human(human) { }

List<BodySegmentSample> const& HumanStateInstance::samples() const {
    return _samples;
}

TimestampType const& HumanStateInstance::timestamp() const {
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

RobotStateHistory::RobotStateHistory(Robot const* robot) : _robot(robot) {
}

DiscreteLocation const& RobotStateHistory::current_location() const {
    return _current_location;
}

bool RobotStateHistory::has_samples(DiscreteLocation const& location) const {
    return _location_states.find(location) != _location_states.end();
}

auto RobotStateHistory::samples(DiscreteLocation const& location) const -> BodySamplesType const& {
    return _location_states.get(location);
}

auto RobotStateHistory::entrances(DiscreteLocation const& location) const -> EntrancesQueueType const& {
    return _location_entrances.get(location);
}

SizeType RobotStateHistory::_update_index(TimestampType const& timestamp) const {
    return floor((timestamp-_location_entrances.get(_current_location).back().timestamp()) / 1000 * _robot->package_frequency());
}

void RobotStateHistory::acquire(RobotStatePackage const& state) {
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
        for (SizeType i=0; i < _robot->num_segments(); ++i)
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

    bool has_history_for_location = (_location_states.find(_current_location) != _location_states.end());
    SizeType j0 = (has_history_for_location ? 0 : 1);
    for (SizeType i=0; i<_robot->num_segments(); ++i) {
        auto head_pts = state.points().at(_robot->segment(i).head_id());
        auto tail_pts = state.points().at(_robot->segment(i).tail_id());
        BodySegmentSample s = (has_history_for_location ? _location_states[_current_location].at(i).at(_update_index(state.timestamp())) :
                                                          _robot->segment(i).create_sample(head_pts.at(0),tail_pts.at(0)));
        s.update(head_pts,tail_pts,j0);
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

BodySegmentSample BodySegment::create_sample() const {
    return BodySegmentSample(this);
}

BodySegmentSample BodySegment::create_sample(Point const& begin, Point const& end) const {
    return BodySegmentSample(this, begin, end);
}

std::ostream& operator<<(std::ostream& os, BodySegment const& s) {
    return os << "(body_id=" << s._body->id() << ", id=" << s._id << ", head_id=" << s._head_id << ", tail_id=" << s._tail_id << ", thickness=" << s._thickness << ")";
}

BodySegmentSample::BodySegmentSample(BodySegment const* segment) :
        _segment(segment), _is_empty(true),
        _head_bounds(BoundingType(3,IntervalType::empty_interval())), _tail_bounds(_head_bounds),
        _head_centre(Point(FloatType(0,dp),FloatType(0,dp),FloatType(0,dp))), _tail_centre(_head_centre),
        _radius(0.0, Ariadne::dp), _bb(BoundingType(3,IntervalType::empty_interval())) { }

BodySegmentSample::BodySegmentSample(BodySegment const* segment, Point const& head, Point const& tail) :
        _segment(segment), _is_empty(false),
        _head_bounds({IntervalType(head.x), IntervalType(head.y), IntervalType(head.z)}),
        _tail_bounds({IntervalType(tail.x), IntervalType(tail.y), IntervalType(tail.z)}),
        _head_centre(head), _tail_centre(tail), _radius(0.0, Ariadne::dp) {
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
    _is_empty = false;
    _update(head,tail);
    _recalculate_centers_radius_bb();
}

void BodySegmentSample::update(List<Point> const& heads, List<Point> const& tails, SizeType const& idx) {
    auto const hs = heads.size();
    auto const ts = tails.size();
    auto common_size = std::min(hs,ts);
    for (SizeType j=idx; j<common_size; ++j)
        _update(heads.at(j), tails.at(j));
    for (SizeType j=common_size; j<hs; ++j)
        _update_head(heads.at(j));
    for (SizeType j=common_size; j<ts; ++j)
        _update_tail(tails.at(j));
    if (_is_empty and decide(not _head_bounds.is_empty() and not _tail_bounds.is_empty()))
        _is_empty = false;
    if (not _is_empty) _recalculate_centers_radius_bb();
}

void BodySegmentSample::_update(Point const& head, Point const& tail) {
    _update_head(head);
    _update_tail(tail);
}

void BodySegmentSample::_update_head(Point const& head) {
    _head_bounds = BoundingType({hull(_head_bounds[0],head.x),hull(_head_bounds[1],head.y),hull(_head_bounds[2],head.z)});
}

void BodySegmentSample::_update_tail(Point const& tail) {
    _tail_bounds = BoundingType({hull(_tail_bounds[0],tail.x),hull(_tail_bounds[1],tail.y),hull(_tail_bounds[2],tail.z)});
}

void BodySegmentSample::_recalculate_centers_radius_bb() {
    auto hc = _head_bounds.centre();
    auto tc = _tail_bounds.centre();
    _head_centre = Point(hc.at(0), hc.at(1), hc.at(2));
    _tail_centre = Point(tc.at(0), tc.at(1), tc.at(2));
    _radius = max(circle_radius(_head_bounds),circle_radius(_tail_bounds));
    _bb = widen(hull(_head_bounds,_tail_bounds),_segment->thickness());
}

BoundingType const& BodySegmentSample::bounding_box() const {
    return _bb;
}

bool BodySegmentSample::is_empty() const {
    return _is_empty;
}

bool BodySegmentSample::intersects(BodySegmentSample const& other) const {
    if (decide(_bb.disjoint(other.bounding_box()))) return false;
    else {
        return (decide(distance(*this,other) <= _segment->thickness()+_radius + other._segment->thickness()+other._radius));
    }
}

std::ostream& operator<<(std::ostream& os, BodySegmentSample const& s) {
    return os << "(s.id=" << s._segment->id() << ",h=" << s._head_centre << ",t=" << s._tail_centre << ")";
}

FloatType distance(BodySegmentSample const& s1, BodySegmentSample const& s2) {
    return distance(s1.head_centre(), s1.tail_centre(), s2.head_centre(), s2.tail_centre());
}

}