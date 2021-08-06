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

using namespace Opera;

BodySegment::BodySegment(unsigned int head_id, unsigned int tail_id, FloatType const& thickness) :
_head_id(head_id), _tail_id(tail_id), _thickness(thickness) { }

unsigned int BodySegment::head_id() const {
    return _head_id;
}

//! \brief Identifier for the tail_position point
unsigned int BodySegment::tail_id() const {
    return _tail_id;
}

FloatType const& BodySegment::thickness() const {
    return _thickness;
}

BodySegmentOccupancy::BodySegmentOccupancy(BodySegment* segment, Point const& begin, Point const& end) :
    _segment(segment), _begin(begin), _end(end) {
    auto const& thickness = _segment->thickness();
    IntervalType xi(min(begin.x,end.x)-thickness,max(begin.x,end.x)+thickness);
    IntervalType yi(min(begin.y,end.y)-thickness,max(begin.y,end.y)+thickness);
    IntervalType zi(min(begin.z,end.z)-thickness,max(begin.z,end.z)+thickness);
    _bb = BoundingType({xi,yi,zi});
}

Point const& BodySegmentOccupancy::head_position() const {
    return _begin;
}

Point const& BodySegmentOccupancy::tail_position() const {
    return _end;
}

BoundingType const& BodySegmentOccupancy::bounding_box() const {
    return _bb;
}

bool BodySegmentOccupancy::intersects(BodySegmentOccupancy const& other) const {
    if (decide(_bb.disjoint(other.bounding_box()))) return false;
    else {
        return (decide(distance(*this,other) <= _segment->thickness()+other._segment->thickness()));
    }
}
