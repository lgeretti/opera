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
    //! \brief Construct empty
    MinimumDistanceBarrierTrace();

    //! \brief The barriers
    List<MinimumDistanceBarrier> const& barriers() const;
    //! \brief Add a barrier
    void add_barrier(PositiveFloatType const& minimum_distance, SphericalApproximationSample const& sample);

    //! \brief The minimum distance by the latest barrier
    PositiveFloatType const& current_minimum_distance() const;
    //! \brief The current index verified by the latest barrier
    SizeType const& current_index() const;

    //! \brief Increase index of the latest barrier
    void increase_index();

    //! \brief Whether the trace is currently empty
    bool empty() const;

    //! \brief Print on the standard output
    friend std::ostream& operator<<(std::ostream& os, MinimumDistanceBarrierTrace const& t);

  private:
    List<MinimumDistanceBarrier> _barriers;
};

//! \brief Return the trace of barriers for a history of \a robot_samples of a segment with respect to a \a human_sample of another segment
MinimumDistanceBarrierTrace populate_barrier_trace(BodySegmentSample const& human_sample, List<BodySegmentSample> const& robot_samples);

}

#endif //OPERA_VERIFICATION_HPP
