/***************************************************************************
 *            state.hpp
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

#ifndef OPERA_STATE_HPP
#define OPERA_STATE_HPP

#include <ariadne/utility/container.hpp>
#include <ariadne/utility/handle.hpp>
#include <ariadne/utility/string.hpp>
#include "body.hpp"

namespace Opera {

using IdType = unsigned int;
using BodyIdType = Ariadne::String;
using TimestampType = long unsigned int; // Expressed in nanoseconds
using Ariadne::List;
using Ariadne::Map;
using Ariadne::DiscreteLocation;
using Ariadne::SizeType;
using Ariadne::String;

//! \brief Holds the state of a human
class HumanStateInstance {
  public:
    //! \brief Construct from a human, points and timestamp
    HumanStateInstance(Human const& human, List<List<Point>> const& points, TimestampType const& timestamp);

    //! \brief The timestamp of the instance
    TimestampType const& timestamp() const;
    //! \brief The samples for each segment
    List<BodySegmentSample> const& samples() const;
  private:
    TimestampType const _timestamp;
    List<BodySegmentSample> _samples;
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

//! \brief A trace of discrete states, enriched by the probability that this trace is followed
class RobotDiscreteTrace {
  public:
    //! \brief Default constructor has empty trace with 1.0 probability
    RobotDiscreteTrace();
    //! \brief Copy constructor
    RobotDiscreteTrace(RobotDiscreteTrace const& other);

    //! \brief The locations in the trace
    List<DiscreteLocation> locations() const;
    //! \brief The next locations with their probability
    //! \details Computed lazily
    Map<DiscreteLocation,PositiveFloatType> const& next_locations() const;

    //! \brief Add to the head of the trace
    RobotDiscreteTrace& push_front(DiscreteLocation const& location);
    //! \brief Add to the tail of the trace
    RobotDiscreteTrace& push_back(DiscreteLocation const& location, PositiveFloatType const& probability = pa_one);

    //! \brief The number of locations
    SizeType size() const;

    //! \brief Equality operator
    bool operator==(RobotDiscreteTrace const& other) const;
    //! \brief Copy assignment
    RobotDiscreteTrace& operator=(RobotDiscreteTrace const& other);

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, RobotDiscreteTrace const& t);

  private:
    std::deque<DiscreteLocation> _locations;
    PositiveFloatType _probability;
    mutable Map<DiscreteLocation,PositiveFloatType> _next_locations;
};

//! \brief Holds the states reached by a robot up to now
class RobotStateHistory {
    typedef List<BodySegmentSample> SegmentTemporalSamplesType;
    typedef List<SegmentTemporalSamplesType> BodySamplesType;
    typedef Ariadne::Map<DiscreteLocation,BodySamplesType> LocationSamplesType;
  public:
    RobotStateHistory(Robot const* robot);
    RobotStateHistory(RobotStateHistory const& other) = delete;
  public:
    //! \brief Acquire the \a state to be ultimately held into the hystory
    //! \details Hystory will not be effectively updated till the location changes
    void acquire(DiscreteLocation const& location, List<List<Point>> const& points, TimestampType const& timestamp);

    //! \brief The current location
    DiscreteLocation const& current_location() const;

    //! \brief The discrete trace
    RobotDiscreteTrace discrete_trace() const;

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
    Robot const* _robot;
};

}

#endif //OPERA_STATE_HPP
