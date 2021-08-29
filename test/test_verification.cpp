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
        ARIADNE_TEST_CALL(test_barrier_trace_reset())
    }

    void test_barrier_trace_create() {
        Human h("h0", {0, 1, 3, 2}, {FloatType(1.0, Ariadne::dp),FloatType(1.0, Ariadne::dp)});
        auto hs = h.segment(1).create_sample(Point(0,0,0),Point(2,2,2));
        PositiveFloatType distance(FloatType(0.5,dp));
        MinimumDistanceBarrierTrace trace(hs,3);
        ARIADNE_TEST_PRINT(trace)
        ARIADNE_TEST_EQUALS(trace.human_segment_id(),1)
        ARIADNE_TEST_EQUALS(trace.robot_segment_id(),3)
        ARIADNE_TEST_EQUALS(trace.barriers().size(),0)
        ARIADNE_TEST_EQUALS(trace.next_index(),0)
        ARIADNE_TEST_EQUALS(trace.current_minimum_distance(),pa_infty)
    }

    void test_barrier() {
        Human h("r0", {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto hs = h.segment(0).create_sample(Point(0,0,0),Point(2,2,2));
        PositiveFloatType distance(FloatType(0.5,dp));
        DiscreteLocation loc(StringVariable(h.id())|"first");
        MinimumDistanceBarrierTrace trace(hs,0);
        trace.add_barrier(loc,distance);
        auto barrier = trace.barriers().back();
        ARIADNE_TEST_ASSERT(decide(barrier.minimum_distance() == distance))
        ARIADNE_TEST_EQUALS(barrier.farthest_location(),loc)
        ARIADNE_TEST_EQUALS(barrier.maximum_index(),0)
        ARIADNE_TEST_PRINT(barrier)
    }

    void test_barrier_trace_populate() {
        Robot r("r0", 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h("h0", {0, 1}, {FloatType(1.0, Ariadne::dp)});

        auto hs = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0));

        DiscreteLocation loc(StringVariable(r.id())|"first");
        MinimumDistanceBarrierTrace trace(hs,0);
        List<BodySegmentSample> robot_samples;

        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,4,0),Point(1,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,3,0),Point(2,3,0)));
        for (auto s : robot_samples) if (not trace.try_update_with(loc,s)) break;
        ARIADNE_TEST_ASSERT(not trace.try_update_with(loc,robot_samples.at(4)))
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
        for (auto s : robot_samples) if (not trace.try_update_with(loc,s)) break;
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
        for (auto s : robot_samples) if (not trace.try_update_with(loc,s)) break;
        ARIADNE_TEST_EQUALS(trace.barriers().size(),4)
        ARIADNE_TEST_EQUALS(trace.next_index(),7)
        ARIADNE_TEST_ASSERT(decide(trace.current_minimum_distance()>0))
        ARIADNE_TEST_PRINT(trace)
    }

    void test_barrier_trace_resume_index() {
        Robot r("r0", 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h("h0", {0, 1}, {FloatType(1.0, Ariadne::dp)});

        auto hs1 = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0));

        /*
                DiscreteLocation first(StringVariable(r.id())|"first");
        DiscreteLocation second(StringVariable(r.id())|"second");
                 auto history = r.make_history();
        history.acquire(RobotStatePackage(first,{{Point(-3,7,0)},{Point(-2,7,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(-2,6,0)},{Point(-1,6,0)}},1e8));
        history.acquire(RobotStatePackage(first,{{Point(-1,5,0)},{Point(0,5,0)}},2e8));
        history.acquire(RobotStatePackage(first,{{Point(-2,6,0)},{Point(0,5,0)}},3e8));
        history.acquire(RobotStatePackage(first,{{Point(-1,5,0)},{Point(0,5,0)}},4e8));
        history.acquire(RobotStatePackage(first,{{Point(0,4,0)},{Point(1,4,0)}},5e8));
        history.acquire(RobotStatePackage(first,{{Point(0,4,0)},{Point(1,4,0)}},6e8));
        history.acquire(RobotStatePackage(first,{{Point(1,3,0)},{Point(2,3,0)}},7e8));
        history.acquire(RobotStatePackage(first,{{Point(1,3,0)},{Point(2,3,0)}},8e8));
        history.acquire(RobotStatePackage(second,{{Point(1,3,0)},{Point(2,3,0)}},9e8));
        ARIADNE_TEST_PRINT(history.samples(first))
        for (auto s : history.samples(first)) if (not trace.try_update_with(first,s.at(trace.robot_segment_id()))) break;
         */

        DiscreteLocation first(StringVariable(r.id())|"first");
        DiscreteLocation second(StringVariable(r.id())|"second");
        MinimumDistanceBarrierTrace trace(hs1,0);
        auto history = r.make_history();
        history.acquire(RobotStatePackage(first,{{Point(-3,7,0)},{Point(-2,7,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(-2,6,0)},{Point(-1,6,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(-1,5,0)},{Point(0,5,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(-2,6,0)},{Point(0,5,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(-1,5,0)},{Point(0,5,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(0,4,0)},{Point(1,4,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(0,4,0)},{Point(1,4,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(1,3,0)},{Point(2,3,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(1,3,0)},{Point(2,3,0)}},0));
        history.acquire(RobotStatePackage(second,{{Point(1,3,0)},{Point(2,3,0)}},0));
        for (auto s : history.samples(first).at(trace.robot_segment_id())) if (not trace.try_update_with(first,s)) break;

        auto hs2 = h.segment(0).create_sample(Point(0,0.1,0),Point(2,0.1,0)).spherical_approximation();
        auto element = trace.resume_element(hs2);
        ARIADNE_TEST_EQUALS(trace.barriers().at(element).maximum_index()+1,trace.next_index())

        auto hs3 = h.segment(0).create_sample(Point(0,1,0),Point(2,1,0)).spherical_approximation();
        element = trace.resume_element(hs3);
        ARIADNE_TEST_ASSERT(trace.barriers().at(element).maximum_index()+1 < trace.next_index())

        auto hs4 = h.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)).spherical_approximation();
        element = trace.resume_element(hs4);
        ARIADNE_TEST_EQUALS(element,-1)
    }

    void test_barrier_trace_reset() {
        Robot r("r0", 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h("h0", {0, 1}, {FloatType(1.0, Ariadne::dp)});

        auto hs1 = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0));

        DiscreteLocation first(StringVariable(r.id())|"first");
        DiscreteLocation second(StringVariable(r.id())|"second");
        MinimumDistanceBarrierTrace trace(hs1,0);
        auto history = r.make_history();
        history.acquire(RobotStatePackage(first,{{Point(-3,7,0)},{Point(-2,7,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(-2,6,0)},{Point(-1,6,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(-1,5,0)},{Point(0,5,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(-2,6,0)},{Point(0,5,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(-1,5,0)},{Point(0,5,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(0,4,0)},{Point(1,4,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(0,4,0)},{Point(1,4,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(1,3,0)},{Point(2,3,0)}},0));
        history.acquire(RobotStatePackage(first,{{Point(1,3,0)},{Point(2,3,0)}},0));
        history.acquire(RobotStatePackage(second,{{Point(1,3,0)},{Point(2,3,0)}},0));
        for (auto s : history.samples(first).at(trace.robot_segment_id())) if (not trace.try_update_with(first,s)) break;

        auto hs2 = h.segment(0).create_sample(Point(0,1,0),Point(2,1,0));
        trace.reset(hs2,history);
        ARIADNE_TEST_ASSERT(trace.next_index()>0)
    }
};

int main() {
    TestVerification().test();
    return ARIADNE_TEST_FAILURES;
}
