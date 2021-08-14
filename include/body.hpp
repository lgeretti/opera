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
    Body(IdType const& id, BodyType const& type, SizeType const& package_frequency, List<IdType> const& points_ids, List<FloatType> const& thicknesses);

    //! \brief The body identifier
    IdType const& id() const;
    //! \brief The type of body
    BodyType const& type() const;
    //! \brief The frequency of packages sent by the body, in Hz
    SizeType const& package_frequency() const;

    //! \brief The segments making the body
    List<BodySegment> const& segments() const;

    //! \brief Create an empty history for the body
    BodyStateHistory make_history() const;
  private:
    IdType const _id;
    BodyType const _type;
    SizeType const _package_frequency;
    List<BodySegment> _segments;
};

class BodySegmentSample;

class BodySegment {
    friend class Body;
  protected:
    //! \brief Construct from identifier, head_centre/tail_centre identifiers and thickness
    BodySegment(const Body* body, IdType const& id, IdType const& head_id, IdType const& tail_id, FloatType const& thickness);
  public:
    //! \brief Identifier for the segment within the specific body
    IdType id() const;


    //! \brief Identifier for the head
    IdType head_id() const;

    //! \brief Identifier for the tail
    IdType tail_id() const;

    //! \brief Return the thickness of the body segment around the geometrical segment
    FloatType const& thickness() const;

    //! \brief Create a sample for the segment from a \a begin and \a end points
    BodySegmentSample create_sample(Point const& begin, Point const& end) const;

  private:
    IdType const _id;
    IdType const _head_id;
    IdType const _tail_id;
    FloatType const _thickness;
    const Body* _body;
};

//! \brief A representation of an inbound package for the state of a body
class BodyStatePackage {
  public:
    //! \brief Construct from a \a location, a list of samples for each point, and a \a timestamp
    BodyStatePackage(DiscreteLocation const& location, List<List<Point>> const& points, TimestampType const& timestamp);
    //! \brief The location
    DiscreteLocation const& location() const;
    //! \brief The points for each segment
    //! \details These will be always even, at least two (head and tail) but multiple points may be present
    List<List<Point>> const& points() const;
    //! \brief The timestamp associated with the package
    TimestampType const& timestamp() const;

  private:
    DiscreteLocation const _location;
    List<List<Point>> const _points;
    TimestampType const _timestamp;
};

//! \brief The data (source+timestamp) for a discrete transition
class DiscreteTransitionData {
  public:
    //! \brief Construct from a \a source and a \a timestamp
    DiscreteTransitionData(DiscreteLocation const& source, TimestampType const& timestamp);
    //! \brief The source location of the transition
    DiscreteLocation const& source() const;
    //! \brief The timestamp of the transition
    TimestampType const& timestamp() const;
  private:
    DiscreteLocation const _source;
    TimestampType const _timestamp;
};

//! \brief Holds the states reached by the body up to now
class BodyStateHistory {
    friend class Body;
    using BodySegmentId = IdType;
    typedef std::deque<DiscreteTransitionData> EntrancesQueueType;
    typedef Ariadne::Map<DiscreteLocation,EntrancesQueueType> LocationEntrancesType;
    typedef List<BodySegmentSample> SegmentTemporalSamplesType;
    typedef List<SegmentTemporalSamplesType> BodySamplesType;
    typedef Ariadne::Map<DiscreteLocation,BodySamplesType> LocationSamplesType;
  protected:
    BodyStateHistory(const Body* body);
  public:
    //! \brief Acquire the \a state to be ultimately held into the hystory
    //! \details Hystory will not be effectively updated till the location changes
    void acquire(BodyStatePackage const& state);

    //! \brief The current location
    DiscreteLocation const& current_location() const;

    //! \brief Whether there are samples registered for the \a location
    //! \details Until the location changes, samples acquired are not registered
    bool has_samples(DiscreteLocation const& location) const;

    //! \brief She samples in a given \a location
    BodySamplesType const& samples(DiscreteLocation const& location) const;

    //! \brief The entrances in a given \a location
    EntrancesQueueType const& entrances(DiscreteLocation const& location) const;

  private:
    //! \brief Find the index of the sample to update given the current \a timestamp
    SizeType _update_index(TimestampType const& timestamp) const;

  private:
    LocationEntrancesType _location_entrances;
    LocationSamplesType _location_states;
    DiscreteLocation _current_location;
    BodySamplesType _current_location_states_buffer;
    const Body* _body;
};

class BodySegmentSample {
    friend class BodySegment;
  protected:
    //! \brief Create from two singleton points
    BodySegmentSample(const BodySegment* segment, Point const& head, Point const& tail);
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

    //! \brief Update only the head bounds
    void update_head(Point const& head);
    //! \brief Update only the tail bounds
    void update_tail(Point const& tail);

    //! \brief Return the bounding box overapproximation
    BoundingType const& bounding_box() const;

    //! \brief Whether it intersects an \a other segment
    //! \details Returns true also in the case of tangency
    bool intersects(BodySegmentSample const& other) const;

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, const BodySegmentSample& s) {
        return os << "(s.id=" << s._segment->id() << ",h=" << s._head_centre << ",t=" << s._tail_centre << ")"; }

  private:

    //! \brief Re-calculate the centers, radius and bounding box after an update
    void _recalculate_centers_radius_bb();

  private:
    BoundingType _head_bounds;
    BoundingType _tail_bounds;
    Point _head_centre;
    Point _tail_centre;
    FloatType _radius;
    const BodySegment* _segment;

    BoundingType _bb;
};



//! \brief Calculate the minimum distance between two segments
FloatType distance(BodySegmentSample const& s1, BodySegmentSample const& s2);

}

#endif //OPERA_BODY_HPP
