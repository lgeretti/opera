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
        ARIADNE_TEST_CALL(test_continuous_verification_trace_manual_creation())
        ARIADNE_TEST_CALL(test_continuous_verification_trace_creation_from_samples())
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

    void test_continuous_verification_trace_manual_creation() {
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

    void test_continuous_verification_trace_creation_from_samples() {
        Robot r(0, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h(1, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});

        BodySegmentSample human_sample = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0));

        List<BodySegmentSample> robot_samples;
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,4,0),Point(1,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,3,0),Point(2,3,0)));

        auto trace = verify_continuous(human_sample,robot_samples);
        ARIADNE_TEST_EQUALS(trace.steps().size(),5)
        ARIADNE_TEST_EQUALS(trace.steps().back().maximum_index(),4)

        robot_samples.clear();
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-3,6,0),Point(-2,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,5,0),Point(-1,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,4,0),Point(0,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,3,0),Point(1,3,0)));

        trace = verify_continuous(human_sample,robot_samples);
        ARIADNE_TEST_EQUALS(trace.steps().size(),5)
        ARIADNE_TEST_EQUALS(trace.steps().back().maximum_index(),5)

        robot_samples.clear();
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,5,0),Point(1,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,5,0),Point(2,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(2,5,0),Point(3,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(3,5,0),Point(4,5,0)));

        trace = verify_continuous(human_sample,robot_samples);
        ARIADNE_TEST_EQUALS(trace.steps().size(),4)
        ARIADNE_TEST_EQUALS(trace.steps().back().maximum_index(),6)
        ARIADNE_TEST_ASSERT(decide(trace.steps().back().minimum_distance()>0))
    }

};


int main() {
    TestVerification().test();

    return ARIADNE_TEST_FAILURES;
}
