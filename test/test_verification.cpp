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
        ARIADNE_TEST_CALL(test_barrier_trace_creation())
        ARIADNE_TEST_CALL(test_barrier())
        ARIADNE_TEST_CALL(test_barrier_trace_multiple_samples())
    }

    void test_barrier_trace_creation() {
        Human h(5, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto sa = h.segment(0).create_sample(Point(0,0,0),Point(2,2,2)).spherical_approximation();
        PositiveFloatType distance(FloatType(0.5,dp));
        MinimumDistanceBarrierTrace trace;
        ARIADNE_TEST_EQUALS(trace.barriers().size(),0)
        ARIADNE_TEST_FAIL(trace.current_index())
        ARIADNE_TEST_EQUALS(trace.current_minimum_distance(),pa_infty)
    }

    void test_barrier() {
        Human h(5, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto sa = h.segment(0).create_sample(Point(0,0,0),Point(2,2,2)).spherical_approximation();
        PositiveFloatType distance(FloatType(0.5,dp));
        MinimumDistanceBarrierTrace trace;
        trace.add_barrier(distance,sa);
        auto barrier = trace.barriers().back();
        ARIADNE_TEST_ASSERT(decide(barrier.minimum_distance() == distance))
        ARIADNE_TEST_EQUALS(barrier.sample().centre(),Point(1,1,1))
        ARIADNE_TEST_EQUALS(barrier.maximum_index(),0)
        ARIADNE_TEST_PRINT(barrier)
    }

    void test_barrier_trace_multiple_samples() {
        Robot r(0, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h(1, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});

        auto human_spherical_sample = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0)).spherical_approximation();

        MinimumDistanceBarrierTrace trace;
        List<BodySegmentSample> robot_samples;

        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,4,0),Point(1,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,3,0),Point(2,3,0)));
        for (auto s : robot_samples) trace.apply(human_spherical_sample,s);
        ARIADNE_TEST_EQUALS(trace.barriers().size(),4)
        ARIADNE_TEST_EQUALS(trace.current_index(),3)

        trace.clear();
        robot_samples.clear();
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-3,6,0),Point(-2,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,5,0),Point(-1,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,4,0),Point(0,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,3,0),Point(1,3,0)));
        for (auto s : robot_samples) trace.apply(human_spherical_sample,s);
        ARIADNE_TEST_EQUALS(trace.barriers().size(),4)
        ARIADNE_TEST_EQUALS(trace.current_index(),4)

        trace.clear();
        robot_samples.clear();
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,5,0),Point(1,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,5,0),Point(2,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(2,5,0),Point(3,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(3,5,0),Point(4,5,0)));
        for (auto s : robot_samples) trace.apply(human_spherical_sample,s);
        ARIADNE_TEST_EQUALS(trace.barriers().size(),4)
        ARIADNE_TEST_EQUALS(trace.current_index(),6)
        ARIADNE_TEST_ASSERT(decide(trace.current_minimum_distance()>0))
        ARIADNE_TEST_PRINT(trace)
    }

};

int main() {
    TestVerification().test();
    return ARIADNE_TEST_FAILURES;
}
