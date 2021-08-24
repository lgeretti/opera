/***************************************************************************
 *            verification.hpp
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

#ifndef OPERA_VERIFICATION_HPP
#define OPERA_VERIFICATION_HPP

#include "body.hpp"

namespace Opera {

class MinimumDistanceBarrierTrace;

//! \brief A barrier on the minimum distance
class MinimumDistanceBarrier {
    friend MinimumDistanceBarrierTrace;
  protected:
    //! \brief Construct from fields
    MinimumDistanceBarrier(PositiveFloatType const& minimum_distance, SphericalApproximationSample const& sample, SizeType const& maximum_index);

    //! \brief Increase the maximum index
    void increase_maximum_index();
  public:
    //! \brief The minimum distance from the spherical approximation sample
    PositiveFloatType const& minimum_distance() const;
    //! \brief The spherical approximation sample
    SphericalApproximationSample const& sample() const;
    //! \brief The maximum index for which this minimum distance holds
    SizeType const& maximum_index() const;

    //! \brief Whether this barrier can be used also for the \a other spherical approximation sample
    bool includes(SphericalApproximationSample const& other) const;

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrier const& s);

  private:
    PositiveFloatType const _minimum_distance;
    SphericalApproximationSample const _sample;
    SizeType _maximum_index;
};

//! \brief The full trace of minimum distance barriers
class MinimumDistanceBarrierTrace {
  public:
    //! \brief Construct from a starting next index
    MinimumDistanceBarrierTrace(SizeType const& starting_next_index);

    //! \brief The barriers
    List<MinimumDistanceBarrier> const& barriers() const;
    //! \brief Add a barrier
    void add_barrier(PositiveFloatType const& minimum_distance, SphericalApproximationSample const& sample);
    //! \brief Apply by checking a \a spherical_sample with a \a segment_sample
    //! \returns True if something was applied (update to the index or new barrier), false otherwise
    bool try_apply(SphericalApproximationSample const& spherical_sample, BodySegmentSample const& segment_sample);

    //! \brief The minimum distance by the latest barrier
    PositiveFloatType const& current_minimum_distance() const;
    //! \brief The next index to apply
    SizeType const& next_index() const;
    //! \brief The index from which we can resume checking the spherical \a sample
    //! \details If the whole trace still holds, then the result is the current index +1.
    //! A result of zero instead means starting from scratch with respect
    //! to the samples used to generate this trace
    SizeType resume_index(SphericalApproximationSample const& sample) const;
    //! \brief Whether the trace is currently empty
    bool is_empty() const;

    //! \brief Remove all barriers
    void clear();

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrierTrace const& t);

  private:
    SizeType _next_index;
    List<MinimumDistanceBarrier> _barriers;
};

}

#endif //OPERA_VERIFICATION_HPP
