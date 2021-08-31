/***************************************************************************
 *            body.hpp
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

#ifndef OPERA_BODY_HPP
#define OPERA_BODY_HPP

#include <ariadne/utility/container.hpp>
#include <ariadne/utility/handle.hpp>
#include <ariadne/utility/string.hpp>
#include "geometry.hpp"

namespace Opera {

using IdType = unsigned int;
using BodyIdType = Ariadne::String;
using TimestampType = long unsigned int; // Expressed in nanoseconds
using Ariadne::List;
using Ariadne::DiscreteLocation;
using Ariadne::SizeType;
using Ariadne::String;

class BodySegment;

class Body {
  public:
    //! \brief Construct from fields
    Body(BodyIdType const& id, List<IdType> const& points_ids, List<FloatType> const& thicknesses);

    //! \brief The body identifier
    BodyIdType const& id() const;

    //! \brief Return the segment indexed by \a idx
    BodySegment const& segment(SizeType const& idx) const;

    //! \brief The number of segments in the body
    SizeType num_segments() const;

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, Body const& b);

    //! \brief Destructor
    virtual ~Body();

  private:
    BodyIdType const _id;
  protected:
    List<BodySegment*> _segments;
};

//! \brief A human is a body able to get a singular state
class Human : public Body {
  public:
    using Body::Body;
};

//! \brief A robot is a body able to have its history
class Robot : public Body {
  public:
    //! \brief Construct from fields
    Robot(BodyIdType const& id, SizeType const& packet_frequency, List<IdType> const& points_ids, List<FloatType> const& thicknesses);
    //! \brief The frequency of packets sent by the robot, in Hz
    SizeType const& packet_frequency() const;
  private:
    SizeType const _packet_frequency;
};

class BodySegmentSample;

class BodySegment {
    friend class Body;
  public:
    //! \brief Construct from identifier, head_centre/tail_centre identifiers and thickness
    BodySegment(Body const* body, IdType const& id, IdType const& head_id, IdType const& tail_id, FloatType const& thickness);
  public:
    //! \brief Identifier for the segment within the specific body
    IdType const& id() const;

    //! \brief Identifier for the head
    IdType const& head_id() const;

    //! \brief Identifier for the tail
    IdType const& tail_id() const;

    //! \brief Return the thickness of the body segment around the geometrical segment
    FloatType const& thickness() const;

    //! \brief Create an is_empty sample
    BodySegmentSample create_sample() const;

    //! \brief Create a sample for the segment from a \a begin and \a end points
    BodySegmentSample create_sample(Point const& begin, Point const& end) const;

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, BodySegment const& s);

  private:
    IdType const _id;
    IdType const _head_id;
    IdType const _tail_id;
    FloatType const _thickness;
    Body const* _body;
};

//! \brief An approximation of a segment sample with a sphere
class SphericalApproximationSample {
  public:
    SphericalApproximationSample(Point const& centre, FloatType const& radius);

    //! \brief Return the centre
    Point const& centre() const;
    //! \brief Return the radius
    FloatType const& radius() const;

    //! \brief The distance from a sample
    friend PositiveFloatType distance(SphericalApproximationSample const& sample, BodySegmentSample const& other);
    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, SphericalApproximationSample const& s);
  private:
    Point _centre;
    FloatType _radius;
};

class BodySegmentSampleInterface {
  public:
    //! \brief Return the identifier of the segment
    virtual IdType const& segment_id() const = 0;

    //! \brief Return the centre point for the head of the segment
    virtual Point const& head_centre() const = 0;
    //! \brief Return the centre point for the tail of the segment
    virtual Point const& tail_centre() const = 0;

    //! \brief Return the maximum spherical error in the segment head/tail positions,
    //! as obtained from the centres with respect to the bounds
    virtual FloatType const& error() const = 0;

    //! \brief The thickness of the segment
    virtual FloatType const& thickness() const = 0;

    //! \brief Return the bounding box overapproximation
    virtual BoundingType const& bounding_box() const = 0;

    //! \brief Whether the segment is empty, i.e., whether either head and tail are is_empty
    virtual bool is_empty() const = 0;

    //! \brief Update the head and tail bounds from the given points
    virtual void update(Point const& head, Point const& tail) = 0;

    //! \brief Update the head and tail bounds from the given lists of points, starting from the one in \a idx
    virtual void update(List<Point> const& heads, List<Point> const& tails, SizeType const& idx = 0u) = 0;

    //! \brief Whether it intersects an \a other segment
    //! \details Returns true also in the case of tangency
    virtual bool intersects(BodySegmentSampleInterface const& other) const = 0;

    //! \brief Construct a spherical approximation from the segment
    virtual SphericalApproximationSample spherical_approximation() const = 0;

    //! \brief Calculate the minimum distance between two segment samples
    friend FloatType distance(BodySegmentSampleInterface const& s1, BodySegmentSampleInterface const& s2);

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, BodySegmentSampleInterface const& s);
};

class BodySegmentSampleBase: public BodySegmentSampleInterface {
    friend class BodySegment;
  protected:
    //! \brief Create from two singleton points
    BodySegmentSampleBase(BodySegment const* segment, Point const& head, Point const& tail);
    //! \brief Create non-valid
    BodySegmentSampleBase(BodySegment const* segment);
  public:

    IdType const& segment_id() const override;

    Point const& head_centre() const override;
    Point const& tail_centre() const override;

    FloatType const& error() const override;
    FloatType const& thickness() const override;
    BoundingType const& bounding_box() const override;

    bool is_empty() const override;

    void update(Point const& head, Point const& tail) override;
    void update(List<Point> const& heads, List<Point> const& tails, SizeType const& idx = 0u) override;

    SphericalApproximationSample spherical_approximation() const override;

    bool intersects(BodySegmentSampleInterface const& other) const override;

  private:
    //! \brief Update head and tail bounds, without recalculation of metrics
    void _update(Point const& head, Point const& tail);
    //! \brief Update only the head bounds, without recalculation of metrics
    void _update_head(Point const& head);
    //! \brief Update only the tail bounds, without recalculation of metrics
    void _update_tail(Point const& tail);
    //! \brief Re-calculate the centres, error and bounding box from the bounds
    void _recalculate_centres_radius_bb();

  private:
    BodySegment const* _segment;
    bool _is_empty;

    BoundingType _head_bounds;
    BoundingType _tail_bounds;

    Point _head_centre;
    Point _tail_centre;
    FloatType _radius;
    BoundingType _bb;
};

//! \brief Handle class for BodySegmentSampleInterface
class BodySegmentSample : public Ariadne::Handle<BodySegmentSampleInterface> {
  public:
    using Ariadne::Handle<BodySegmentSampleInterface>::Handle;

    IdType const& segment_id() const { return this->_ptr->segment_id(); };
    Point const& head_centre() const { return this->_ptr->head_centre(); };
    Point const& tail_centre() const { return this->_ptr->tail_centre(); };
    FloatType error() const { return this->_ptr->error(); }
    FloatType thickness() const { return this->_ptr->thickness(); }
    BoundingType const& bounding_box() const { return this->_ptr->bounding_box(); }
    bool is_empty() const { return this->_ptr->is_empty(); }
    void update(Point const& head, Point const& tail) { this->_ptr->update(head,tail); }
    void update(List<Point> const& heads, List<Point> const& tails, SizeType const& idx = 0u) { this->_ptr->update(heads,tails,idx); }
    bool intersects(BodySegmentSample const& other) const { return this->_ptr->intersects(other); }
    SphericalApproximationSample spherical_approximation() const { return this->_ptr->spherical_approximation(); }
    friend FloatType distance(BodySegmentSample const& s1, BodySegmentSample const& s2)  { return distance(s1.const_reference(),s2.const_reference()); }
    friend std::ostream& operator<<(std::ostream& os, BodySegmentSample const& s) { return os << s.const_reference(); }
};

}

#endif //OPERA_BODY_HPP
