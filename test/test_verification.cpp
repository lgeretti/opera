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
        ARIADNE_TEST_CALL(test_barrier_trace_create())
        ARIADNE_TEST_CALL(test_barrier())
        ARIADNE_TEST_CALL(test_barrier_trace_populate())
        ARIADNE_TEST_CALL(test_barrier_trace_resume_index())
    }

    void test_barrier_trace_create() {
        Human h(5, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto sa = h.segment(0).create_sample(Point(0,0,0),Point(2,2,2)).spherical_approximation();
        PositiveFloatType distance(FloatType(0.5,dp));
        MinimumDistanceBarrierTrace trace(sa,5u);
        ARIADNE_TEST_EQUALS(trace.barriers().size(),0)
        ARIADNE_TEST_EQUALS(trace.next_index(),5)
        ARIADNE_TEST_EQUALS(trace.current_minimum_distance(),pa_infty)
    }

    void test_barrier() {
        Human h(5, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto sa = h.segment(0).create_sample(Point(0,0,0),Point(2,2,2)).spherical_approximation();
        PositiveFloatType distance(FloatType(0.5,dp));
        MinimumDistanceBarrierTrace trace(sa,0u);
        trace.add_barrier(distance);
        auto barrier = trace.barriers().back();
        ARIADNE_TEST_ASSERT(decide(barrier.minimum_distance() == distance))
        ARIADNE_TEST_EQUALS(barrier.maximum_index(),0)
        ARIADNE_TEST_PRINT(barrier)
    }

    void test_barrier_trace_populate() {
        Robot r(0, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h(1, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});

        auto human_spherical_sample = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0)).spherical_approximation();

        MinimumDistanceBarrierTrace trace(human_spherical_sample,0u);
        List<BodySegmentSample> robot_samples;

        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,4,0),Point(1,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,3,0),Point(2,3,0)));
        for (auto s : robot_samples) if (not trace.try_update_with(s)) break;
        ARIADNE_TEST_ASSERT(not trace.try_update_with(robot_samples.at(4)))
        ARIADNE_TEST_EQUALS(trace.barriers().size(),4)
        ARIADNE_TEST_EQUALS(trace.next_index(),4)

        trace.clear();
        robot_samples.clear();
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-3,6,0),Point(-2,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,5,0),Point(-1,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,4,0),Point(0,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,3,0),Point(1,3,0)));
        for (auto s : robot_samples) if (not trace.try_update_with(s)) break;
        ARIADNE_TEST_EQUALS(trace.barriers().size(),4)
        ARIADNE_TEST_EQUALS(trace.next_index(),5)

        trace.clear();
        robot_samples.clear();
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,5,0),Point(1,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,5,0),Point(2,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(2,5,0),Point(3,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(3,5,0),Point(4,5,0)));
        for (auto s : robot_samples) if (not trace.try_update_with(s)) break;
        ARIADNE_TEST_EQUALS(trace.barriers().size(),4)
        ARIADNE_TEST_EQUALS(trace.next_index(),7)
        ARIADNE_TEST_ASSERT(decide(trace.current_minimum_distance()>0))
        ARIADNE_TEST_PRINT(trace)
    }

    void test_barrier_trace_resume_index() {
        Robot r(0, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h(1, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});

        auto hs1 = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0)).spherical_approximation();

        MinimumDistanceBarrierTrace trace(hs1,0u);
        List<BodySegmentSample> robot_samples;
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,4,0),Point(1,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,4,0),Point(1,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,3,0),Point(2,3,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,3,0),Point(2,3,0)));
        for (auto s : robot_samples) if (not trace.try_update_with(s)) break;

        ARIADNE_TEST_PRINT(trace)

        auto hs2 = h.segment(0).create_sample(Point(0,0.1,0),Point(2,0.1,0)).spherical_approximation();
        auto idx = trace.resume_index(hs2);
        ARIADNE_TEST_PRINT(idx)
        ARIADNE_TEST_EQUALS(idx,trace.next_index())

        auto hs3 = h.segment(0).create_sample(Point(0,1,0),Point(2,1,0)).spherical_approximation();
        idx = trace.resume_index(hs3);
        ARIADNE_TEST_PRINT(idx)
        ARIADNE_TEST_ASSERT(idx < trace.next_index())

        auto hs4 = h.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)).spherical_approximation();
        idx = trace.resume_index(hs4);
        ARIADNE_TEST_PRINT(idx)
        ARIADNE_TEST_EQUALS(idx,0u)
    }
};

int main() {
    TestVerification().test();
    return ARIADNE_TEST_FAILURES;
}
