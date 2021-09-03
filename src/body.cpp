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
using Ariadne::Map;

namespace Opera {

Body::Body(BodyIdType const& id, List<Pair<IdType,IdType>> const& points_ids, List<FloatType> const& thicknesses) :
    _id(id) {
    ARIADNE_ASSERT_MSG(points_ids.size() == thicknesses.size(), "The number of point pairs must equal the number of thicknesses")

    for (List<IdType>::size_type i=0; i<thicknesses.size(); ++i)
        _segments.append(new BodySegment(this,i,points_ids.at(i).first,points_ids.at(i).second,thicknesses.at(i)));
}

Body::~Body() {
    for (auto s: _segments)
        delete(s);
}

BodyIdType const& Body::id() const {
    return _id;
}

BodySegment const& Body::segment(SizeType const& idx) const {
    return *_segments.at(idx);
}

SizeType Body::num_segments() const {
    return _segments.size();
}

std::ostream& operator<<(std::ostream& os, Body const& b) {
    os << "(id=" << b.id() << ", segments=[";
    for (SizeType i=0; i<b.num_segments()-1; ++i) {
        os << b.segment(i) << ",";
    }
    os << b.segment(b.num_segments()-1) << "])";
    return os;
}

Robot::Robot(BodyIdType const& id, SizeType const& packet_frequency, List<Pair<IdType,IdType>> const& points_ids, List<FloatType> const& thicknesses) :
    Body(id,points_ids,thicknesses), _packet_frequency(packet_frequency) {
    ARIADNE_ASSERT_MSG(packet_frequency > 0, "The packet frequency must be strictly positive")
}

SizeType const& Robot::packet_frequency() const {
    return _packet_frequency;
}

BodySegment::BodySegment(Body const* body, IdType const& id, IdType const& head_id, IdType const& tail_id, FloatType const& thickness) :
    _body(body), _id(id), _head_id(head_id), _tail_id(tail_id), _thickness(thickness) { }

IdType const& BodySegment::id() const {
    return _id;
}

IdType const& BodySegment::head_id() const {
    return _head_id;
}

IdType const& BodySegment::tail_id() const {
    return _tail_id;
}

FloatType const& BodySegment::thickness() const {
    return _thickness;
}

BodySegmentSample BodySegment::create_sample() const {
    return BodySegmentSample(new BodySegmentSampleBase(this));
}

BodySegmentSample BodySegment::create_sample(Point const& begin, Point const& end) const {
    return BodySegmentSample(new BodySegmentSampleBase(this, begin, end));
}

std::ostream& operator<<(std::ostream& os, BodySegment const& s) {
    return os << "(body_id=" << s._body->id() << ", id=" << s._id << ", head_id=" << s._head_id << ", tail_id=" << s._tail_id << ", thickness=" << s._thickness << ")";
}

BodySegmentSampleBase::BodySegmentSampleBase(BodySegment const* segment) :
        _segment(segment), _is_empty(true),
        _head_bounds(BoundingType({FloatIntervalType::empty_interval(), FloatIntervalType::empty_interval(), FloatIntervalType::empty_interval()})),
        _tail_bounds(_head_bounds),
        _head_centre(Point(FloatType(0,dp),FloatType(0,dp),FloatType(0,dp))),
        _tail_centre(_head_centre),
        _radius(0.0, Ariadne::dp),
        _bb(_head_bounds)
        { }

BodySegmentSampleBase::BodySegmentSampleBase(BodySegment const* segment, Point const& head, Point const& tail) :
        _segment(segment), _is_empty(false),
        _head_bounds({FloatIntervalType(head.x), FloatIntervalType(head.y), FloatIntervalType(head.z)}),
        _tail_bounds({FloatIntervalType(tail.x), FloatIntervalType(tail.y), FloatIntervalType(tail.z)}),
        _head_centre(head), _tail_centre(tail), _radius(0.0, Ariadne::dp) {
    _bb = Ariadne::widen(Ariadne::hull(_head_bounds,_tail_bounds),_segment->thickness());
}

IdType const& BodySegmentSampleBase::segment_id() const {
    return _segment->id();
}

Point const& BodySegmentSampleBase::head_centre() const {
    return _head_centre;
}

Point const& BodySegmentSampleBase::tail_centre() const {
    return _tail_centre;
}

FloatType const& BodySegmentSampleBase::error() const {
    return _radius;
}

FloatType const& BodySegmentSampleBase::thickness() const {
    return _segment->thickness();
}

void BodySegmentSampleBase::update(Point const& head, Point const& tail) {
    _is_empty = false;
    _update(head,tail);
    _recalculate_centres_radius_bb();
}

void BodySegmentSampleBase::update(List<Point> const& heads, List<Point> const& tails, SizeType const& idx) {
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
    if (not _is_empty) _recalculate_centres_radius_bb();
}

void BodySegmentSampleBase::_update(Point const& head, Point const& tail) {
    _update_head(head);
    _update_tail(tail);
}

void BodySegmentSampleBase::_update_head(Point const& head) {
    _head_bounds = BoundingType({hull(_head_bounds[0],head.x),hull(_head_bounds[1],head.y),hull(_head_bounds[2],head.z)});
}

void BodySegmentSampleBase::_update_tail(Point const& tail) {
    _tail_bounds = BoundingType({hull(_tail_bounds[0],tail.x),hull(_tail_bounds[1],tail.y),hull(_tail_bounds[2],tail.z)});
}

void BodySegmentSampleBase::_recalculate_centres_radius_bb() {
    auto hc = _head_bounds.centre();
    auto tc = _tail_bounds.centre();
    _head_centre = Point(hc.at(0), hc.at(1), hc.at(2));
    _tail_centre = Point(tc.at(0), tc.at(1), tc.at(2));
    _radius = max(circle_radius(_head_bounds),circle_radius(_tail_bounds));
    _bb = widen(hull(_head_centre,_tail_centre),_radius+_segment->thickness());
}

BoundingType const& BodySegmentSampleBase::bounding_box() const {
    return _bb;
}

bool BodySegmentSampleBase::is_empty() const {
    return _is_empty;
}

bool BodySegmentSampleBase::intersects(BodySegmentSampleInterface const& other) const {
    if (decide(bounding_box().disjoint(other.bounding_box()))) return false;
    else {
        return (decide(distance(*this,other) <= this->thickness() + this->error() + other.thickness() + other.error()));
    }
}

SphericalApproximationSample BodySegmentSampleBase::spherical_approximation() const {
    auto centre = _bb.centre();
    return SphericalApproximationSample(Point(centre.at(0), centre.at(1), centre.at(2)), circle_radius(_bb));
}

std::ostream& operator<<(std::ostream& os, BodySegmentSampleInterface const& s) {
    return os << "(h=" << s.head_centre() << ",t=" << s.tail_centre() << ")";
}

FloatType distance(BodySegmentSampleInterface const& s1, BodySegmentSampleInterface const& s2) {
    return distance(s1.head_centre(), s1.tail_centre(), s2.head_centre(), s2.tail_centre());
}

SphericalApproximationSample::SphericalApproximationSample(Point const& centre, FloatType const& radius) :
    _centre(centre), _radius(radius) { }

Point const& SphericalApproximationSample::centre() const {
    return _centre;
}

FloatType const& SphericalApproximationSample::radius() const {
    return _radius;
}

PositiveFloatType distance(SphericalApproximationSample const& sample, BodySegmentSample const& other) {
    static const FloatType zero = FloatType(0,dp);
    return cast_positive(max(zero,distance(sample._centre,other.head_centre(),other.tail_centre())-other.error()-other.thickness()-sample._radius));
}

std::ostream& operator<<(std::ostream& os, SphericalApproximationSample const& s) {
    return os << "(centre: " << s.centre() << ", radius: " << s.radius() << ")";
}

}