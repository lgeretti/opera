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

BodySegmentSample::BodySegmentSample(BodySegment* segment, Point const& begin, Point const& end) :
        _segment(segment), _head_position(begin), _tail_position(end) {
    auto const& thickness = _segment->thickness();
    IntervalType xi(min(begin.x,end.x)-thickness,max(begin.x,end.x)+thickness);
    IntervalType yi(min(begin.y,end.y)-thickness,max(begin.y,end.y)+thickness);
    IntervalType zi(min(begin.z,end.z)-thickness,max(begin.z,end.z)+thickness);
    _bb = BoundingType({xi,yi,zi});
}

Point const& BodySegmentSample::head_position() const {
    return _head_position;
}

Point const& BodySegmentSample::tail_position() const {
    return _tail_position;
}

BoundingType const& BodySegmentSample::bounding_box() const {
    return _bb;
}

bool BodySegmentSample::intersects(BodySegmentSample const& other) const {
    if (decide(_bb.disjoint(other.bounding_box()))) return false;
    else {
        return (decide(distance(*this,other) <= _segment->thickness() + other._segment->thickness()));
    }
}

FloatType distance(BodySegmentSample const& s1, BodySegmentSample const& s2) {
    return distance(s1.head_position(), s1.tail_position(), s2.head_position(), s2.tail_position());
}

}