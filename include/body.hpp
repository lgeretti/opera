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

class BodySegment;
class HumanStateInstance;
class HumanStatePackage;
class RobotStateHistory;

class Body {
  protected:
    //! \brief Construct from fields
    Body(IdType const& id, SizeType const& package_frequency, List<IdType> const& points_ids, List<FloatType> const& thicknesses);
  public:
    //! \brief The body identifier
    IdType const& id() const;
    //! \brief The frequency of packages sent by the body, in Hz
    SizeType const& package_frequency() const;

    // TODO: access only a specific segment by index and the num_segments

    //! \brief Return the segment indexed by \a idx
    BodySegment const& segment(SizeType const& idx) const;

    //! \brief The number of segments in the body
    SizeType num_segments() const;

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, Body const& b);

    //! \brief Destructor
    virtual ~Body();

  private:
    IdType const _id;
    SizeType const _package_frequency;
  protected:
    List<BodySegment*> _segments;
};

//! \brief A human is a body able to get a singular state
class Human : public Body {
  public:
    //! \brief Construct from fields
    Human(IdType const& id, SizeType const& package_frequency, List<IdType> const& points_ids, List<FloatType> const& thicknesses);
    //! \brief Create a state instance from the package
    HumanStateInstance make_instance(HumanStatePackage const& pkg) const;
};

//! \brief A robot is a body able to have its history
class Robot : public Body {
  public:
    //! \brief Construct from fields
    Robot(IdType const& id, SizeType const& package_frequency, List<IdType> const& points_ids, List<FloatType> const& thicknesses);
    //! \brief Create an empty history for the robot packages received
    RobotStateHistory make_history() const;
};

class BodySegmentSample;

class BodySegment {
    friend class Body;
  public:
    //! \brief Construct from identifier, head_centre/tail_centre identifiers and thickness
    BodySegment(Body const* body, IdType const& id, IdType const& head_id, IdType const& tail_id, FloatType const& thickness);
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

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, BodySegment const& s);

  private:
    IdType const _id;
    IdType const _head_id;
    IdType const _tail_id;
    FloatType const _thickness;
    Body const* _body;
};

//! \brief A representation of an inbound package for the state of a body
class BodyStatePackage {
  protected:
    //! \brief Construct from a list of samples for each point, and a \a timestamp
    BodyStatePackage(List<List<Point>> const& points, TimestampType const& timestamp);
  public:
    //! \brief The points for each segment
    //! \details These will be always even, at least two (head and tail) but multiple points may be present
    List<List<Point>> const& points() const;
    //! \brief The timestamp associated with the package
    TimestampType const& timestamp() const;
  private:
    List<List<Point>> const _points;
    TimestampType const _timestamp;
};

//! \brief A representation of an inbound package for the state of a human body
class HumanStatePackage : public BodyStatePackage {
  public:
    //! \brief Construct from a list of samples for each point, and a \a timestamp
    HumanStatePackage(List<List<Point>> const& points, TimestampType const& timestamp);
};

//! \brief A representation of an inbound package for the state of a robot body
class RobotStatePackage : public BodyStatePackage {
  public:
    //! \brief Construct from a \a location, a list of samples for each point, and a \a timestamp
    RobotStatePackage(DiscreteLocation const& location, List<List<Point>> const& points, TimestampType const& timestamp);
    //! \brief The location
    DiscreteLocation const& location() const;
  private:
    DiscreteLocation const _location;
};

class BodySegmentSample {
    friend class BodySegment;
protected:
    //! \brief Create from two singleton points
    BodySegmentSample(BodySegment const* segment, Point const& head, Point const& tail);
public:

    //! \brief Return the center point for the head of the segment
    Point const& head_centre() const;
    //! \brief Return the center point for the tail of the segment
    Point const& tail_centre() const;

    //! \brief Return the radius of error in the segment head/tail positions,
    //! as obtained from the centers with respect to the bounds
    FloatType radius() const;

    //! \brief Update the head and tail bounds from the given points
    void update(Point const& head, Point const& tail);

    //! \brief Update the head and tail bounds from the given lists of points, starting from the one in \a idx
    void update(List<Point> const& heads, List<Point> const& tails, SizeType const& idx
    = 0u);

    //! \brief Update only the head bounds
    void update_head(Point const& head);
    //! \brief Update only the tail bounds
    void update_tail(Point const& tail);

    //! \brief Re-calculate the centers, radius and bounding box from the bounds
    void recalculate_centers_radius_bb();

    //! \brief Return the bounding box overapproximation
    BoundingType const& bounding_box() const;

    //! \brief Whether it intersects an \a other segment
    //! \details Returns true also in the case of tangency
    bool intersects(BodySegmentSample const& other) const;

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, BodySegmentSample const& s);

private:
    BoundingType _head_bounds;
    BoundingType _tail_bounds;
    Point _head_centre;
    Point _tail_centre;
    FloatType _radius;
    BodySegment const* _segment;

    BoundingType _bb;
};

//! \brief Calculate the minimum distance between two segments
FloatType distance(BodySegmentSample const& s1, BodySegmentSample const& s2);

//! \brief Holds the state of a human
class HumanStateInstance {
    friend class Human;
  protected:
    //! \brief Construct from a human pointer and the fields
    HumanStateInstance(Human const* human, List<BodySegmentSample> const& samples, TimestampType const& timestamp);
  public:
    //! \brief The samples for each segment
    List<BodySegmentSample> const& samples() const;
    //! \brief The timestamp of the instance
    TimestampType const& timestamp() const;
  private:
    List<BodySegmentSample> const _samples;
    TimestampType const _timestamp;
    Human const* _human;
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

//! \brief Holds the states reached by a robot up to now
class RobotStateHistory {
    friend class Robot;
    using BodySegmentId = IdType;
    typedef std::deque<DiscreteTransitionData> EntrancesQueueType;
    typedef Ariadne::Map<DiscreteLocation,EntrancesQueueType> LocationEntrancesType;
    typedef List<BodySegmentSample> SegmentTemporalSamplesType;
    typedef List<SegmentTemporalSamplesType> BodySamplesType;
    typedef Ariadne::Map<DiscreteLocation,BodySamplesType> LocationSamplesType;
  protected:
    RobotStateHistory(Robot const* robot);
    RobotStateHistory(RobotStateHistory const& other) = delete;
  public:
    //! \brief Acquire the \a state to be ultimately held into the hystory
    //! \details Hystory will not be effectively updated till the location changes
    void acquire(RobotStatePackage const& state);

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
    Body const* _robot;
};

}

#endif //OPERA_BODY_HPP
