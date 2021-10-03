/***************************************************************************
 *            body.cpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of Opera, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "discrete_state.hpp"
#include "macros.hpp"
#include "body.hpp"

namespace Opera {

Body::Body(BodyIdType const& id, List<Pair<IdType,IdType>> const& points_ids, List<FloatType> const& thicknesses) :
    _id(id) {
    OPERA_ASSERT_MSG(points_ids.size() == thicknesses.size(), "The number of point pairs must equal the number of thicknesses")

    _num_points = 0;
    for (List<IdType>::size_type i=0; i<thicknesses.size(); ++i) {
        _num_points = std::max(std::max(_num_points,static_cast<SizeType>(points_ids.at(i).first+1)),static_cast<SizeType>(points_ids.at(i).second+1));
        _segments.push_back(new BodySegment(this,static_cast<IdType>(i),points_ids.at(i).first,points_ids.at(i).second,thicknesses.at(i)));
    }
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

SizeType Body::num_points() const {
    return _num_points;
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
    OPERA_ASSERT_MSG(packet_frequency > 0, "The packet frequency must be strictly positive")
}

SizeType const& Robot::packet_frequency() const {
    return _packet_frequency;
}

BodySegment::BodySegment(Body const* body, IdType const& id, IdType const& head_id, IdType const& tail_id, FloatType const& thickness) :
    _id(id), _head_id(head_id), _tail_id(tail_id), _thickness(thickness), _body(body) { }

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
    return BodySegmentSample(this);
}

BodySegmentSample BodySegment::create_sample(List<Point> const& begin, List<Point> const& end) const {
    BodySegmentSample result(this);
    result.update(begin,end);
    return result;
}

std::ostream& operator<<(std::ostream& os, BodySegment const& s) {
    return os << "(body_id=" << s._body->id() << ", id=" << s._id << ", head_id=" << s._head_id << ", tail_id=" << s._tail_id << ", thickness=" << s._thickness << ")";
}

BodySegmentSample::BodySegmentSample(BodySegment const* segment) :
        _segment(segment), _is_empty(true),
        _head_bounds(Box::make_empty()),
        _tail_bounds(_head_bounds),
        _head_centre(Point(0.0,0.0,0.0)),
        _tail_centre(_head_centre),
        _radius(0.0),
        _bb(_head_bounds)
        { }

IdType const& BodySegmentSample::segment_id() const {
    return _segment->id();
}

Point const& BodySegmentSample::head_centre() const {
    return _head_centre;
}

Point const& BodySegmentSample::tail_centre() const {
    return _tail_centre;
}

FloatType const& BodySegmentSample::error() const {
    return _radius;
}

FloatType const& BodySegmentSample::thickness() const {
    return _segment->thickness();
}

void BodySegmentSample::update(List<Point> const& heads, List<Point> const& tails) {
    auto const hs = heads.size();
    auto const ts = tails.size();
    auto common_size = std::min(hs,ts);
    for (SizeType j=0; j<common_size; ++j)
        _update(heads.at(j), tails.at(j));
    for (SizeType j=common_size; j<hs; ++j)
        _update_head(heads.at(j));
    for (SizeType j=common_size; j<ts; ++j)
        _update_tail(tails.at(j));
    if (_is_empty and (not _head_bounds.is_empty() and not _tail_bounds.is_empty()))
        _is_empty = false;
    if (not _is_empty) _recalculate_centres_radius_bb();
}

void BodySegmentSample::_update(Point const& head, Point const& tail) {
    _update_head(head);
    _update_tail(tail);
}

void BodySegmentSample::_update_head(Point const& head) {
    _head_bounds = Box(std::min(_head_bounds.xl(),head.x),std::max(_head_bounds.xu(),head.x),
                       std::min(_head_bounds.yl(),head.y),std::max(_head_bounds.yu(),head.y),
                       std::min(_head_bounds.zl(),head.z),std::max(_head_bounds.zu(),head.z));
}

void BodySegmentSample::_update_tail(Point const& tail) {
    _tail_bounds = Box(std::min(_tail_bounds.xl(),tail.x),std::max(_tail_bounds.xu(),tail.x),
                       std::min(_tail_bounds.yl(),tail.y),std::max(_tail_bounds.yu(),tail.y),
                       std::min(_tail_bounds.zl(),tail.z),std::max(_tail_bounds.zu(),tail.z));
}

void BodySegmentSample::_recalculate_centres_radius_bb() {
    auto hc = _head_bounds.centre();
    auto tc = _tail_bounds.centre();
    _head_centre = Point(hc.x, hc.y, hc.z);
    _tail_centre = Point(tc.x, tc.y, tc.z);
    _radius = std::max(_head_bounds.circle_radius(),_tail_bounds.circle_radius());
    _bb = widen(hull(_head_centre,_tail_centre),_radius+_segment->thickness());
}

Box const& BodySegmentSample::bounding_box() const {
    return _bb;
}

bool BodySegmentSample::is_empty() const {
    return _is_empty;
}

bool BodySegmentSample::intersects(BodySegmentSampleInterface const& other) const {
    if (bounding_box().disjoint(other.bounding_box())) return false;
    else {
        return distance(*this,other) <= this->thickness() + this->error() + other.thickness() + other.error();
    }
}

SphericalApproximationSample BodySegmentSample::spherical_approximation() const {
    auto centre = _bb.centre();
    return SphericalApproximationSample(Point(centre.x, centre.y, centre.z), _bb.circle_radius());
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
    return std::max(0.0,distance(sample._centre,other.head_centre(),other.tail_centre())-other.error()-other.thickness()-sample._radius);
}

std::ostream& operator<<(std::ostream& os, SphericalApproximationSample const& s) {
    return os << "(centre: " << s.centre() << ", radius: " << s.radius() << ")";
}

}