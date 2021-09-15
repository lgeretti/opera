/***************************************************************************
 *            barrier.hpp
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

#ifndef OPERA_BARRIER_HPP
#define OPERA_BARRIER_HPP

#include "state.hpp"

namespace Opera {

class MinimumDistanceBarrierTrace;

//! \brief A barrier on the minimum distance
class MinimumDistanceBarrier {
    friend MinimumDistanceBarrierTrace;
  protected:
    //! \brief Construct from fields
    MinimumDistanceBarrier(PositiveFloatType const& minimum_distance, DiscreteLocation const& farthest_location, SizeType const& maximum_index);
    //! \brief Increase the maximum index
    void increase_maximum_index();
    //! \brief Set the maximum index to zero (used when the location changes)
    void reset_maximum_index();
    //! \brief Change the location
    void set_farthest_location(DiscreteLocation const& location);
  public:
    //! \brief The minimum distance from the spherical approximation sample
    PositiveFloatType const& minimum_distance() const;
    //! \brief The farthest location with this minimum distance
    DiscreteLocation const& farthest_location() const;
    //! \brief The maximum index in the farthest location for which this minimum distance holds
    SizeType const& maximum_index() const;

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrier const& s);

  private:
    PositiveFloatType const _minimum_distance;
    DiscreteLocation _farthest_location;
    SizeType _maximum_index;
};

//! \brief The full trace of minimum distance barriers for a human-robot segment pair
class MinimumDistanceBarrierTrace {
  public:
    //! \brief Construct from an initial \a human_sample and the \a robot_segment_id
    MinimumDistanceBarrierTrace(BodySegmentSample const& human_sample, IdType const& robot_segment_id);

    //! \brief The segment id for the human
    IdType const& human_segment_id() const;
    //! \brief The segment id for the robot
    IdType const& robot_segment_id() const;

    //! \brief The barrier at the given \a idx
    MinimumDistanceBarrier const& barrier(SizeType const& idx) const;
    //! \brief The number of barriers
    SizeType size() const;
    //! \brief Add a barrier
    void add_barrier(DiscreteLocation const& farthest_location, PositiveFloatType const& minimum_distance);
    //! \brief Check the spherical approximation with a robot \a sample in \a location, to update the last barrier or create a new one
    //! \returns True if something was updated or created, false otherwise
    bool try_update_with(DiscreteLocation const& location, BodySegmentSample const& robot_sample);

    //! \brief The minimum distance by the latest barrier
    PositiveFloatType const& current_minimum_distance() const;
    //! \brief The next index to apply
    SizeType const& next_index() const;
    //! \brief The element in the trace from which we can resume checking an \a other spherical approximation
    //! \details If the whole trace still holds, then the result is next index
    //! A result of -1 instead means starting from scratch with respect to the samples used to generate this trace
    int resume_element(SphericalApproximationSample const& other) const;
    //! \brief Whether the trace is currently empty
    bool is_empty() const;

    //! \brief Reset the trace according to resuming obtained from \a human_sample with respect to \a history
    //! \details The history is necessary in order to reconstruct the distance from the new human sample
    void reset(BodySegmentSample const& human_sample, RobotStateHistory const& history);

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrierTrace const& t);

  private:
    IdType const _human_segment_id;
    IdType const _robot_segment_id;
    SphericalApproximationSample _spherical_approximation;
    SizeType _next_index;
    std::deque<MinimumDistanceBarrier> _barriers;
};

}

#endif //OPERA_BARRIER_HPP
