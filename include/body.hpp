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
using Ariadne::DiscreteLocation;
using Ariadne::SizeType;

//! \brief Enumeration for the type of body
enum class BodyType { ROBOT, WORKER };

class BodySegment;
class BodyStateHistory;

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

    //! \brief Create an empty history for the body
    BodyStateHistory make_history();
  private:
    IdType const _id;
    BodyType const _type;
    List<BodySegment> _segments;
};

class BodySegmentSample;

class BodySegment {
    friend class Body;
  protected:
    //! \brief Construct from identifier, head_centre/tail_centre identifiers and thickness
    BodySegment(Body* body, IdType const& id, IdType const& head_id, IdType const& tail_id, FloatType const& thickness);
  public:
    //! \brief Identifier for the segment within the specific body
    IdType id() const;


    //! \brief Identifier for the head
    IdType head_id() const;

    //! \brief Identifier for the tail
    IdType tail_id() const;

    //! \brief Return the thickness of the body segment around the geometrical segment
    FloatType const& thickness() const;

    //! \brief Create a state for the segment from a \a begin and \a end points
    BodySegmentSample create_state(Point const& begin, Point const& end);

  private:
    IdType const _id;
    IdType const _head_id;
    IdType const _tail_id;
    FloatType const _thickness;
    Body* const _body;
};

class BodyState {
  public:
    BodyState(DiscreteLocation const& location, List<Point> const& points, TimestampType const& timestamp);
  private:
    DiscreteLocation const _location;
    List<Point> const _points;
    TimestampType const _timestamp;
};

//! \brief Holds the states reached by the body up to now
class BodyStateHistory {
    friend class Body;
    using BodySegmentId = IdType;
    typedef List<List<Point>> PointStatesBuffer;
    typedef Ariadne::Map<DiscreteLocation,std::deque<TimestampType>> LocationEntriesType;
    typedef Ariadne::Map<BodySegmentId,List<BodySegmentSample>> SegmentSamplesType;
    typedef Ariadne::Map<DiscreteLocation,SegmentSamplesType> LocationStatesType;
  protected:
    BodyStateHistory(Body* body);
  public:
    //! \brief Acquire the \a state to be ultimately held into the hystory
    //! \details Hystory will not be effectively updated till the location changes
    void acquire(BodyState const& state);
  private:
    LocationEntriesType _location_entries;
    LocationStatesType _location_states;
    DiscreteLocation _current_location;
    PointStatesBuffer _current_location_states_buffer;
    Body* const _body;
};

class BodySegmentSample {
    friend class BodySegment;
  protected:
    //! \brief Create from two singleton points
    BodySegmentSample(BodySegment* segment, Point const& head, Point const& tail);
  public:

    //! \brief Return the center point for the head of the segment
    Point const& head_centre() const;
    //! \brief Return the center point for the tail of the segment
    Point const& tail_centre() const;

    //! \brief Return the radius of error in the segment head/tail positions,
    //! as obtained from the centers with respect to the bounds
    FloatType radius() const;

    //! \brief Update the begin and end bounds with the given points
    void update(Point const& begin, Point const& end);

    //! \brief Return the bounding box overapproximation
    BoundingType const& bounding_box() const;

    //! \brief Whether it intersects an \a other segment
    //! \details Returns true also in the case of tangency
    bool intersects(BodySegmentSample const& other) const;

  private:
    BoundingType _head_bounds;
    BoundingType _tail_bounds;
    Point _head_centre;
    Point _tail_centre;
    FloatType _radius;
    BodySegment* const _segment;

    BoundingType _bb;
};

//! \brief Calculate the minimum distance between two segments
FloatType distance(BodySegmentSample const& s1, BodySegmentSample const& s2);

}

#endif //OPERA_BODY_HPP
