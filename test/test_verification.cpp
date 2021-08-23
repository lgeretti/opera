/***************************************************************************
 *            test_verification.cpp
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

#include "test.hpp"

#include "verification.hpp"

using namespace Opera;

class TestVerification {
  public:
    void test() {
        ARIADNE_TEST_CALL(test_minimum_distance_step())
        ARIADNE_TEST_CALL(test_continuous_verification_trace())
    }

    void test_minimum_distance_step() {
        Human h(5, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto sa = h.segment(0).create_sample(Point(0,0,0),Point(2,2,2)).spherical_approximation();
        PositiveFloatType distance(FloatType(0.5,dp));
        MinimumDistanceStep step(distance,sa,0);
        ARIADNE_TEST_ASSERT(decide(step.minimum_distance() == distance))
        ARIADNE_TEST_EQUALS(step.sample().centre(),Point(1,1,1))
        ARIADNE_TEST_EQUALS(step.maximum_index(),0)
        step.increase_maximum_index();
        ARIADNE_TEST_EQUALS(step.maximum_index(),1)
    }

    void test_continuous_verification_trace() {
        Human h(5, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto sa = h.segment(0).create_sample(Point(0,0,0),Point(2,2,2)).spherical_approximation();
        PositiveFloatType distance(FloatType(0.5,dp));
        MinimumDistanceStep step(distance,sa,3);
        ContinuousVerificationTrace trace(step);
        ARIADNE_TEST_EQUALS(trace.steps().size(),1)
        trace.increase_maximum_index();
        ARIADNE_TEST_EQUALS(trace.steps().back().maximum_index(),4)
        trace.add_step(MinimumDistanceStep(cast_positive(FloatType(0.25,dp)),sa,5));
        ARIADNE_TEST_EQUALS(trace.steps().size(),2)
    }

};


int main() {
    TestVerification().test();

    return ARIADNE_TEST_FAILURES;
}
