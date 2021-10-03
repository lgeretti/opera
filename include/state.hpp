/***************************************************************************
 *            state.hpp
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

#ifndef OPERA_STATE_HPP
#define OPERA_STATE_HPP

#include <deque>
#include "body.hpp"
#include "utility.hpp"
#include "interval.hpp"
#include "discrete_state.hpp"

namespace Opera {

using IdType = unsigned int;
using BodyIdType = std::string;
using TimestampType = long unsigned int; // Expressed in nanoseconds

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
    RobotLocationPresence(DiscreteState const& location, DiscreteState const& exit_destination, TimestampType const& from, TimestampType const& to);
    //! \brief The location of presence
    DiscreteState const& location() const;
    //! \brief The destination location after exiting
    DiscreteState const& exit_destination() const;
    //! \brief The timestamp of entrance
    TimestampType const& from() const;
    //! \brief The timestamp of exit
    TimestampType const& to() const;

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, RobotLocationPresence const& p);
  private:
    DiscreteState const _location;
    DiscreteState const _exit_destination;
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

    //! \brief The locations in the trace at \a idx
    DiscreteState at(SizeType const& idx) const;

    //! \brief The probability of this trace
    PositiveFloatType const& probability() const;

    //! \brief The next locations with their probability, which accounts for the probability of this trace
    //! \details This is computed lazily
    Map<DiscreteState,PositiveFloatType> const& next_locations() const;

    //! \brief Add to the head of the trace
    RobotDiscreteTrace& push_front(DiscreteState const& location);
    //! \brief Add to the tail of the trace
    RobotDiscreteTrace& push_back(DiscreteState const& location, PositiveFloatType const& probability = 1.0);

    //! \brief The number of locations
    SizeType size() const;

    //! \brief Equality operator
    bool operator==(RobotDiscreteTrace const& other) const;
    //! \brief Copy assignment
    RobotDiscreteTrace& operator=(RobotDiscreteTrace const& other);

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, RobotDiscreteTrace const& t);

  private:
    std::deque<DiscreteState> _locations;
    PositiveFloatType _probability;
    mutable Map<DiscreteState,PositiveFloatType> _next_locations;
};

//! \brief Holds the states reached by a robot up to now
class RobotStateHistory {
    typedef List<BodySegmentSample> SegmentTemporalSamplesType;
    typedef List<SegmentTemporalSamplesType> BodySamplesType;
    typedef Map<DiscreteState,BodySamplesType> LocationSamplesType;
  public:
    RobotStateHistory(Robot const* robot);
    RobotStateHistory(RobotStateHistory const& other) = delete;
  public:
    //! \brief Acquire the \a state to be ultimately held into the hystory
    //! \details Hystory will not be effectively updated till the location changes
    void acquire(DiscreteState const& location, List<List<Point>> const& points, TimestampType const& timestamp);

    //! \brief The current location
    DiscreteState const& current_location() const;

    //! \brief The discrete trace
    RobotDiscreteTrace discrete_trace() const;

    //! \brief Whether there are samples registered for the \a location
    //! \details Until the location changes, samples acquired are not registered
    bool has_samples(DiscreteState const& location) const;

    //! \brief The samples in a given \a location
    BodySamplesType const& samples(DiscreteState const& location) const;

    //! \brief The presences in a given \a location
    List<RobotLocationPresence> presences_in(DiscreteState const& location) const;
    //! \brief The presences exiting into a given \a location
    List<RobotLocationPresence> presences_exiting_into(DiscreteState const& location) const;
    //! \brief The presences between a \a source and \a destination locations
    List<RobotLocationPresence> presences_between(DiscreteState const& source, DiscreteState const& destination) const;
    //! \brief The range of number of samples acquired in a given location
    Interval<SizeType> range_of_num_samples_in(DiscreteState const& location) const;
    //! \brief The range of number of samples that are acquired when in \a source going into \a target
    Interval<SizeType> range_of_num_samples_between(DiscreteState const& source, DiscreteState const& target) const;
  private:
    //! \brief Find the index of the sample to update given the current \a timestamp
    SizeType _update_index(TimestampType const& timestamp) const;
    //! \brief The range of number of samples within a list of \a presences_in
    Interval<SizeType> _range_of_num_samples_within(List<RobotLocationPresence> const& presences) const;

  private:
    std::deque<RobotLocationPresence> _location_presences;
    LocationSamplesType _location_states;
    DiscreteState _current_location;
    BodySamplesType _current_location_states_buffer;
    Robot const* _robot;
};

}

#endif //OPERA_STATE_HPP
