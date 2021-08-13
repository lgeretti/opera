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

Body::Body(IdType const& id, BodyType const& type, List<IdType> const& points_ids, List<FloatType> const& thicknesses) :
    _id(id), _type(type) {
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

List<BodySegment> const& Body::segments() const {
    return _segments;
}

BodyStateHistory Body::make_history() {
    return BodyStateHistory(this);
}

BodyState::BodyState(DiscreteLocation const& location, List<Point> const& points, TimestampType const& timestamp) :
    _location(location), _points(points), _timestamp(timestamp) {}

BodyStateHistory::BodyStateHistory(Body* body) : _body(body) { }

BodySegment::BodySegment(Body* body, IdType const& id, IdType const& head_id, IdType const& tail_id, FloatType const& thickness) :
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

BodySegmentSample BodySegment::create_state(Point const& begin, Point const& end) {
    return BodySegmentSample(this, begin, end);
}

BodySegmentSample::BodySegmentSample(BodySegment* segment, Point const& head, Point const& tail) :
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