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

#include "body.hpp"

namespace Opera {

BodySegment::BodySegment(IdType const& head_id, IdType const& tail_id, FloatType const& thickness) :
    _head_id(head_id), _tail_id(tail_id), _thickness(thickness) { }

IdType BodySegment::head_id() const {
    return _head_id;
}

IdType BodySegment::tail_id() const {
    return _tail_id;
}

FloatType const& BodySegment::thickness() const {
    return _thickness;
}

BodySegmentState BodySegment::create_state(Point const& begin, Point const& end, TimestampType const& timestamp) {
    return BodySegmentState(this,begin,end,timestamp);
}

BodySegmentState::BodySegmentState(BodySegment* segment, Point const& begin, Point const& end, TimestampType const& timestamp) :
        _segment(segment), _head_position(begin), _tail_position(end), _timestamp(timestamp) {
    auto const& thickness = _segment->thickness();
    IntervalType xi(min(begin.x,end.x)-thickness,max(begin.x,end.x)+thickness);
    IntervalType yi(min(begin.y,end.y)-thickness,max(begin.y,end.y)+thickness);
    IntervalType zi(min(begin.z,end.z)-thickness,max(begin.z,end.z)+thickness);
    _bb = BoundingType({xi,yi,zi});
}

Point const& BodySegmentState::head_position() const {
    return _head_position;
}

Point const& BodySegmentState::tail_position() const {
    return _tail_position;
}

TimestampType const& BodySegmentState::timestamp() const {
    return _timestamp;
}

BoundingType const& BodySegmentState::bounding_box() const {
    return _bb;
}

bool BodySegmentState::intersects(BodySegmentState const& other) const {
    if (decide(_bb.disjoint(other.bounding_box()))) return false;
    else {
        return (decide(distance(*this,other) <= _segment->thickness() + other._segment->thickness()));
    }
}

FloatType distance(BodySegmentState const& s1, BodySegmentState const& s2) {
    return distance(s1.head_position(), s1.tail_position(), s2.head_position(), s2.tail_position());
}

}