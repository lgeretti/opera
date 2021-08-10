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
#include "geometry.hpp"

namespace Opera {

using IdType = unsigned int;
using TimestampType = long unsigned int;
using Ariadne::List;

//! \brief Enumeration for the type of body
enum class BodyType { ROBOT, WORKER };

class BodySegment;

class Body {
  public:
    //! \brief Construct from id and type, and a list of ids representing the point ids for the segments, along with a list of thicknesses for each segment
    Body(IdType const& id, BodyType const& type, List<IdType> const& points_ids, List<FloatType> const& thicknesses);

    //! \brief The body identifier
    IdType const& id() const;
    //! \brief The type of body
    BodyType const& type() const;

    //! \brief The segments making the body
    List<BodySegment> const& segments() const;
  private:
    IdType const _id;
    BodyType const _type;
    List<BodySegment> _segments;
};

class BodySegmentState;

class BodySegment {
    friend class Body;
  protected:
    //! \brief Construct from identifier, head_position/tail_position identifiers and thickness
    BodySegment(Body* body, IdType const& id, IdType const& head_id, IdType const& tail_id, FloatType const& thickness);
  public:
    //! \brief Identifier for the segment within the specific body
    IdType id() const;

    //! \brief Identifier for the head_position point
    IdType head_id() const;

    //! \brief Identifier for the tail_position point
    IdType tail_id() const;

    //! \brief Return the thickness of the body segment around the geometrical segment
    FloatType const& thickness() const;

    //! \brief Create a state for the segment from a \a begin and \a end points along with a \a timestamp
    BodySegmentState create_state(Point const& begin, Point const& end, TimestampType const& timestamp);

  private:
    IdType const _id;
    IdType const _head_id;
    IdType const _tail_id;
    FloatType const _thickness;
    Body* const _body;
};

class BodySegmentState {
    friend class BodySegment;
  protected:
    //! \brief Construct from two points and the timestamp
    BodySegmentState(BodySegment* segment, Point const& begin, Point const& end, TimestampType const& timestamp);
  public:

    //! \brief Return the position of the head_position point of the segment
    Point const& head_position() const;
    //! \brief Return the position of the tail_position point of the segment
    Point const& tail_position() const;

    //! \brief Return the timestamp of this state
    TimestampType const& timestamp() const;

    //! \brief Return the bounding box overapproximation
    BoundingType const& bounding_box() const;

    //! \brief Whether it intersects an \a other segment
    //! \details Returns true also in the case of tangency
    bool intersects(BodySegmentState const& other) const;

  private:
    Point const _head_position;
    Point const _tail_position;
    TimestampType const _timestamp;
    BodySegment* const _segment;
    BoundingType _bb;
};

//! \brief Calculate the minimum distance between two segments
FloatType distance(BodySegmentState const& s1, BodySegmentState const& s2);

}

#endif //OPERA_BODY_HPP
