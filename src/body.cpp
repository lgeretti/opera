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

BodySegmentTimedSample BodySegment::create_state(Point const& begin, Point const& end, TimestampType const& timestamp) {
    return BodySegmentTimedSample(this, begin, end, timestamp);
}

BodySegmentTimedSample::BodySegmentTimedSample(BodySegment* segment, Point const& begin, Point const& end, TimestampType const& timestamp) :
        _segment(segment), _head_position(begin), _tail_position(end), _timestamp(timestamp) {
    auto const& thickness = _segment->thickness();
    IntervalType xi(min(begin.x,end.x)-thickness,max(begin.x,end.x)+thickness);
    IntervalType yi(min(begin.y,end.y)-thickness,max(begin.y,end.y)+thickness);
    IntervalType zi(min(begin.z,end.z)-thickness,max(begin.z,end.z)+thickness);
    _bb = BoundingType({xi,yi,zi});
}

Point const& BodySegmentTimedSample::head_position() const {
    return _head_position;
}

Point const& BodySegmentTimedSample::tail_position() const {
    return _tail_position;
}

TimestampType const& BodySegmentTimedSample::timestamp() const {
    return _timestamp;
}

BoundingType const& BodySegmentTimedSample::bounding_box() const {
    return _bb;
}

bool BodySegmentTimedSample::intersects(BodySegmentTimedSample const& other) const {
    if (decide(_bb.disjoint(other.bounding_box()))) return false;
    else {
        return (decide(distance(*this,other) <= _segment->thickness() + other._segment->thickness()));
    }
}

FloatType distance(BodySegmentTimedSample const& s1, BodySegmentTimedSample const& s2) {
    return distance(s1.head_position(), s1.tail_position(), s2.head_position(), s2.tail_position());
}

}