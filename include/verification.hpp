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

//! \brief A step (down) on the minimum distance
class MinimumDistanceStep {
  public:
    //! \brief Construct from fields
    MinimumDistanceStep(PositiveFloatType const& minimum_distance, SphericalApproximationSample const& sample, SizeType const& maximum_index);
    //! \brief The minimum distance from the spherical approximation sample
    PositiveFloatType const& minimum_distance() const;
    //! \brief The spherical approximation sample
    SphericalApproximationSample const& sample() const;
    //! \brief The maximum index for which this minimum distance holds
    SizeType const& maximum_index() const;

    //! \brief Increase the maximum index
    void increase_maximum_index();

  private:
    PositiveFloatType const _minimum_distance;
    SphericalApproximationSample const _sample;
    SizeType _maximum_index;
};

//! \brief The full trace of verification information, in the continuous case
class ContinuousVerificationTrace {
  public:
    //! \brief Construct with an initial step
    ContinuousVerificationTrace(MinimumDistanceStep const& initial);

    //! \brief The steps
    List<MinimumDistanceStep> const& steps() const;
    //! \brief Add a step
    void add_step(MinimumDistanceStep const& step);

    //! \brief The minimum distance by the latest step
    PositiveFloatType const& current_minimum_distance() const;
    //! \brief The current index verified by the latest step
    SizeType const& current_index() const;

    //! \brief Increase maximum index of the latest step
    void increase_maximum_index();

  private:
    List<MinimumDistanceStep> _steps;
};

}

#endif //OPERA_VERIFICATION_HPP
