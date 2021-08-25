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
#include "geometry.hpp"

namespace Opera {

using IdType = unsigned int;
using TimestampType = long unsigned int; // Expressed in nanoseconds
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
    //! \brief Create an is_empty history for the robot packages received
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

//! \brief The presence of a robot in a given location
class RobotLocationPresence {
  public:
    //! \brief Construct from a \a source, \a exit_destination, \a from and \a to for entrance/exit in the source location
    RobotLocationPresence(DiscreteLocation const& location, DiscreteLocation const& exit_destination, TimestampType const& from, TimestampType const& to);
    //! \brief The location of presence
    DiscreteLocation const& location() const;
    //! \brief The destination location after exiting
    DiscreteLocation const& exit_destination() const;
    //! \brief The timestamp of entrance
    TimestampType const& from() const;
    //! \brief The timestamp of exit
    TimestampType const& to() const;

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, RobotLocationPresence const& p);
  private:
    DiscreteLocation const _location;
    DiscreteLocation const _exit_destination;
    TimestampType const _from;
    TimestampType const _to;
};

//! \brief The probability to get to a destination
class RobotDestinationLikelihood {
  public:
    RobotDestinationLikelihood(DiscreteLocation const& destination, PositiveFloatType const& probability);

    //! \brief The destination
    DiscreteLocation const& destination() const;
    //! \brief The probability
    PositiveFloatType const& probability() const;

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, RobotDestinationLikelihood const& l);

  private:
    DiscreteLocation const _destination;
    PositiveFloatType const _probability;
};

//! \brief Holds the states reached by a robot up to now
class RobotStateHistory {
    friend class Robot;
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

    //! \brief The samples in a given \a location
    BodySamplesType const& samples(DiscreteLocation const& location) const;

    //! \brief The presences in a given \a location
    List<RobotLocationPresence> presences_in(DiscreteLocation const& location) const;
    //! \brief The presences exiting into a given \a location
    List<RobotLocationPresence> presences_exiting_into(DiscreteLocation const& location) const;
    //! \brief The presences between a \a source and \a destination locations
    List<RobotLocationPresence> presences_between(DiscreteLocation const& source, DiscreteLocation const& destination) const;
    //! \brief The range of number of samples acquired in a given location
    Interval<Natural> range_of_num_samples_in(DiscreteLocation const& location) const;
    //! \brief The range of number of samples that are acquired when in \a source going into \a target
    Interval<Natural> range_of_num_samples_between(DiscreteLocation const& source, DiscreteLocation const& target) const;
    //! \brief The destination for \a location, with the likelihood to be taken
    List<RobotDestinationLikelihood> destination_likelihoods(DiscreteLocation const& location) const;
  private:
    //! \brief Find the index of the sample to update given the current \a timestamp
    SizeType _update_index(TimestampType const& timestamp) const;
    //! \brief The range of number of samples within a list of \a presences_in
    Interval<Natural> _range_of_num_samples_within(List<RobotLocationPresence> const& presences) const;

  private:
    std::deque<RobotLocationPresence> _location_presences;
    LocationSamplesType _location_states;
    DiscreteLocation _current_location;
    BodySamplesType _current_location_states_buffer;
    Body const* _robot;
};

}

#endif //OPERA_BODY_HPP
