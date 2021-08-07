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

BodySegmentState::BodySegmentState(BodySegment* segment, Point const& begin, Point const& end, TimestampType const& timestamp) :
    _segment(segment), _begin(begin), _end(end), _timestamp(timestamp) {
    auto const& thickness = _segment->thickness();
    IntervalType xi(min(begin.x,end.x)-thickness,max(begin.x,end.x)+thickness);
    IntervalType yi(min(begin.y,end.y)-thickness,max(begin.y,end.y)+thickness);
    IntervalType zi(min(begin.z,end.z)-thickness,max(begin.z,end.z)+thickness);
    _bb = BoundingType({xi,yi,zi});
}

Point const& BodySegmentState::head_position() const {
    return _begin;
}

Point const& BodySegmentState::tail_position() const {
    return _end;
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
        return (decide(distance(*this,other) <= _segment->thickness()+other._segment->thickness()));
    }
}

FloatType distance(BodySegmentState const& s1, BodySegmentState const& s2) {

    const FloatType SMALL_VALUE(0.000001,Ariadne::dp);

    auto u = s1.tail_position() - s1.head_position();
    auto v = s2.tail_position() - s2.head_position();
    auto w = s1.head_position() - s2.head_position();

    FloatType a = dot(u, u);
    FloatType b = dot(u, v);
    FloatType c = dot(v, v);
    FloatType d = dot(u, w);
    FloatType e = dot(v, w);
    FloatType D = a * c - b * b;
    FloatType sc = D, sN = D, sD = D;
    FloatType tc = D, tN = D, tD = D;
    if (decide(D < SMALL_VALUE)) {
        sN = 0;
        sD = 1;
        tN = e;
        tD = c;
    } else {
        sN = (b * e - c * d);
        tN = (a * e - b * d);
        if (decide(sN < 0)) {
            sN = 0;
            tN = e;
            tD = c;
        } else if (decide(sN > sD)) {
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }
    if (decide(tN < 0)) {
        tN = 0;
        if (decide(-d < 0)) {
            sN = 0;
        } else if (decide(-d > a)) {
            sN = sD;
        } else {
            sN = -d;
            sD = a;
        }
    } else if (decide(tN > tD)) {
        tN = tD;
        if (decide((-d + b) < 0)) {
            sN = 0;
        } else if (decide((-d + b) > a)) {
            sN = sD;
        } else {
            sN = (-d + b);
            sD = a;
        }
    }

    if (decide(abs(sN) < SMALL_VALUE)) sc = 0;
    else sc = sN / sD;

    if (decide(abs(tN) < SMALL_VALUE)) tc = 0;
    else tc = tN / tD;

    auto dP = w + (sc * u) - (tc * v);

    return sqrt(dot(dP, dP));
}

}