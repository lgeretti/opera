/***************************************************************************
 *            verification.cpp
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

#include "verification.hpp"

namespace Opera {

MinimumDistanceStep::MinimumDistanceStep(PositiveFloatType const& minimum_distance, SphericalApproximationSample const& sample, SizeType const& maximum_index) :
    _minimum_distance(minimum_distance), _sample(sample), _maximum_index(maximum_index) { }

PositiveFloatType const& MinimumDistanceStep::minimum_distance() const {
    return _minimum_distance;
}

SphericalApproximationSample const& MinimumDistanceStep::sample() const {
    return _sample;
}

SizeType const& MinimumDistanceStep::maximum_index() const {
    return _maximum_index;
}

void MinimumDistanceStep::increase_maximum_index() {
    _maximum_index++;
}

std::ostream& operator<<(std::ostream& os, MinimumDistanceStep const& s) {
    return os << "(d:" << s.minimum_distance() << ",&s:" << &s.sample() << ",i:" << s.maximum_index() << ")";
}

ContinuousVerificationTrace::ContinuousVerificationTrace(MinimumDistanceStep const& step) :
    _steps(List<MinimumDistanceStep>())
{
    add_step(step);
}

List<MinimumDistanceStep> const& ContinuousVerificationTrace::steps() const {
    return _steps;
}

void ContinuousVerificationTrace::add_step(MinimumDistanceStep const& step) {
    _steps.append(step);
}

PositiveFloatType const& ContinuousVerificationTrace::current_minimum_distance() const {
    return _steps.back().minimum_distance();
}

SizeType const& ContinuousVerificationTrace::current_index() const {
    return _steps.back().maximum_index();
}

void ContinuousVerificationTrace::increase_maximum_index() {
    _steps.back().increase_maximum_index();
}

std::ostream& operator<<(std::ostream& os, ContinuousVerificationTrace const& t) {
    return os << t.steps();
}

ContinuousVerificationTrace verify_continuous(BodySegmentSample const& human_sample, List<BodySegmentSample> const& robot_samples) {

    const SphericalApproximationSample sas = human_sample.spherical_approximation();
    ContinuousVerificationTrace result(MinimumDistanceStep(distance(sas,robot_samples.at(0)),sas,0));
    PositiveFloatType minimum_distance = result.current_minimum_distance();
    SizeType i=1;
    while (decide(minimum_distance > 0) and i<robot_samples.size()) {
        auto current_distance = distance(sas,robot_samples.at(i));
        if (decide(current_distance<minimum_distance)) {
            minimum_distance = current_distance;
            result.add_step(MinimumDistanceStep(minimum_distance,sas,i));
        } else {
            result.increase_maximum_index();
        }
        ++i;
    }
    return result;
}

}
