/***************************************************************************
 *            test_barrier.cpp
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

#include "barrier.hpp"

using namespace Opera;

class TestBarrier {
  public:
    void test() {

        OPERA_TEST_CALL(test_barrier_trace_create())
        OPERA_TEST_CALL(test_barrier_trace_add_remove())
        OPERA_TEST_CALL(test_barrier_trace_populate())
        OPERA_TEST_CALL(test_barrier_trace_resume_element())
        OPERA_TEST_CALL(test_barrier_trace_reset())
    }

    void test_barrier_trace_create() {
        Human h("h0", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(1.0, Ariadne::dp)});
        auto hs = h.segment(1).create_sample(Point(0,0,0),Point(2,2,2));
        PositiveFloatType distance(FloatType(0.5,dp));
        MinimumDistanceBarrierTrace trace(hs,3);
        OPERA_TEST_PRINT(trace)
        OPERA_TEST_EQUALS(trace.human_segment_id(),1)
        OPERA_TEST_EQUALS(trace.robot_segment_id(),3)
        OPERA_TEST_EQUALS(trace.size(),0)
        OPERA_TEST_ASSERT(trace.is_empty())
        OPERA_TEST_EQUALS(trace.next_index(),0)
        OPERA_TEST_EQUALS(trace.current_minimum_distance(),pa_infty)
    }

    void test_barrier_trace_add_remove() {
        Human h("r0", {{0, 1}}, {FloatType(1.0, Ariadne::dp)});
        auto hs = h.segment(0).create_sample(Point(0,0,0),Point(2,2,2));
        PositiveFloatType distance(FloatType(0.5,dp));
        DiscreteLocation loc(StringVariable(h.id())|"first");
        MinimumDistanceBarrierTrace trace(hs,0);
        trace.add_barrier(loc,distance);
        OPERA_TEST_ASSERT(not trace.is_empty())
        auto barrier = trace.barrier(0);
        OPERA_TEST_ASSERT(decide(barrier.minimum_distance() == distance))
        OPERA_TEST_EQUALS(barrier.farthest_location(),loc)
        OPERA_TEST_EQUALS(barrier.maximum_index(),0)
        OPERA_TEST_PRINT(barrier)
        trace.remove_barrier();
        OPERA_TEST_ASSERT(trace.is_empty())
        OPERA_TEST_FAIL(trace.remove_barrier())
    }

    void test_barrier_trace_populate() {
        Robot r("r0", 10, {{0, 1}}, {FloatType(1.0, Ariadne::dp)});
        Human h("h0", {{0, 1}}, {FloatType(1.0, Ariadne::dp)});

        auto hs = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0));

        DiscreteLocation loc(StringVariable(r.id())|"first");
        MinimumDistanceBarrierTrace trace1(hs,0);
        List<BodySegmentSample> robot_samples;

        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,4,0),Point(1,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,3,0),Point(2,3,0)));
        for (auto s : robot_samples) if (not trace1.try_update_with(loc,s)) break;
        OPERA_TEST_ASSERT(not trace1.try_update_with(loc,robot_samples.at(4)))
        OPERA_TEST_EQUALS(trace1.size(),4)
        OPERA_TEST_EQUALS(trace1.next_index(),4)
        OPERA_TEST_PRINT(trace1)

        MinimumDistanceBarrierTrace trace2(hs,0);
        robot_samples.clear();
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-3,6,0),Point(-2,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,5,0),Point(-1,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,4,0),Point(0,4,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,3,0),Point(1,3,0)));
        for (auto s : robot_samples) if (not trace2.try_update_with(loc,s)) break;
        OPERA_TEST_EQUALS(trace2.size(),4)
        OPERA_TEST_EQUALS(trace2.next_index(),5)
        OPERA_TEST_PRINT(trace2)

        MinimumDistanceBarrierTrace trace3(hs,0);
        robot_samples.clear();
        robot_samples.append(r.segment(0).create_sample(Point(-3,7,0),Point(-2,7,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-2,6,0),Point(-1,6,0)));
        robot_samples.append(r.segment(0).create_sample(Point(-1,5,0),Point(0,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(0,5,0),Point(1,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(1,5,0),Point(2,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(2,5,0),Point(3,5,0)));
        robot_samples.append(r.segment(0).create_sample(Point(3,5,0),Point(4,5,0)));
        for (auto s : robot_samples) if (not trace3.try_update_with(loc,s)) break;
        OPERA_TEST_EQUALS(trace3.size(),4)
        OPERA_TEST_EQUALS(trace3.next_index(),7)
        OPERA_TEST_ASSERT(decide(trace3.current_minimum_distance()>0))
        OPERA_TEST_PRINT(trace3)
    }

    void test_barrier_trace_resume_element() {
        Robot r("r0", 10, {{0, 1}}, {FloatType(1.0, Ariadne::dp)});
        Human h("h0", {{0, 1}}, {FloatType(1.0, Ariadne::dp)});

        auto hs1 = h.segment(0).create_sample(Point(4,5,0),Point(5,5,0));

        DiscreteLocation first(StringVariable(r.id())|"first");
        DiscreteLocation second(StringVariable(r.id())|"second");
        DiscreteLocation third(StringVariable(r.id())|"third");
        MinimumDistanceBarrierTrace trace(hs1,0);
        RobotStateHistory history(&r);
        history.acquire(first,{{Point(-3,7,0)},{Point(-2,7,0)}},0);
        history.acquire(first,{{Point(-2,6,0)},{Point(-1,6,0)}},1e8);
        history.acquire(first,{{Point(-1,5,0)},{Point(0,5,0)}},2e8);
        history.acquire(first,{{Point(-2,6,0)},{Point(0,5,0)}},3e8);
        history.acquire(first,{{Point(-1,5,0)},{Point(0,5,0)}},4e8);
        history.acquire(first,{{Point(0,4,0)},{Point(1,4,0)}},5e8);
        history.acquire(first,{{Point(0,4,0)},{Point(1,4,0)}},6e8);
        history.acquire(first,{{Point(1,3,0)},{Point(2,3,0)}},7e8);
        history.acquire(first,{{Point(1,3,0)},{Point(2,3,0)}},8e8);
        history.acquire(second,{{Point(1,3,0)},{Point(2,3,0)}},9e8);
        history.acquire(third,{{Point(2,3,0)},{Point(3,3,0)}},10e8);
        for (auto s : history.samples(first).at(trace.robot_segment_id())) if (not trace.try_update_with(first,s)) break;
        for (auto s : history.samples(second).at(trace.robot_segment_id())) if (not trace.try_update_with(second,s)) break;
        OPERA_TEST_PRINT(trace)

        auto hs2 = h.segment(0).create_sample(Point(4.1,5,0),Point(5,5,0)).spherical_approximation();
        auto element = trace.resume_element(hs2);
        OPERA_TEST_EQUALS(element,(int)trace.size()-1)

        auto hs3 = h.segment(0).create_sample(Point(5,5,0),Point(5,5,0)).spherical_approximation();
        element = trace.resume_element(hs3);
        OPERA_TEST_ASSERT(element < (int)trace.size()-1)

        auto hs4 = h.segment(0).create_sample(Point(10,10,0),Point(10,10,0)).spherical_approximation();
        element = trace.resume_element(hs4);
        OPERA_TEST_EQUALS(element,-1)
    }

    void test_barrier_trace_reset() {
        Robot r("r0", 10, {{0, 1}}, {FloatType(1.0, Ariadne::dp)});
        Human h("h0", {{0, 1}}, {FloatType(1.0, Ariadne::dp)});

        auto hs1 = h.segment(0).create_sample(Point(4,5,0),Point(5,5,0));

        DiscreteLocation first(StringVariable(r.id())|"first");
        DiscreteLocation second(StringVariable(r.id())|"second");
        DiscreteLocation third(StringVariable(r.id())|"third");
        MinimumDistanceBarrierTrace trace1(hs1,0);
        RobotStateHistory history(&r);
        history.acquire(first,{{Point(-3,7,0)},{Point(-2,7,0)}},0);
        history.acquire(first,{{Point(-2,6,0)},{Point(-1,6,0)}},1e8);
        history.acquire(first,{{Point(-1,5,0)},{Point(0,5,0)}},2e8);
        history.acquire(first,{{Point(-2,6,0)},{Point(0,5,0)}},3e8);
        history.acquire(first,{{Point(-1,5,0)},{Point(0,5,0)}},4e8);
        history.acquire(first,{{Point(0,4,0)},{Point(1,4,0)}},5e8);
        history.acquire(first,{{Point(0,4,0)},{Point(1,4,0)}},6e8);
        history.acquire(first,{{Point(1,3,0)},{Point(2,3,0)}},7e8);
        history.acquire(first,{{Point(1,3,0)},{Point(2,3,0)}},8e8);
        history.acquire(second,{{Point(1,3,0)},{Point(2,3,0)}},9e8);
        history.acquire(third,{{Point(2,3,0)},{Point(3,3,0)}},10e8);
        for (auto s : history.samples(first).at(trace1.robot_segment_id())) if (not trace1.try_update_with(first,s)) break;
        for (auto s : history.samples(second).at(trace1.robot_segment_id())) if (not trace1.try_update_with(second,s)) break;

        MinimumDistanceBarrierTrace trace2 = trace1;
        MinimumDistanceBarrierTrace trace3 = trace1;

        auto hs1_new = h.segment(0).create_sample(Point(4.1,5,0),Point(5,5,0));
        OPERA_TEST_PRINT(trace1)
        trace1.reset(hs1_new,history);
        OPERA_TEST_PRINT(trace1)
        OPERA_TEST_EQUALS(trace1.barrier(trace1.size()-1).farthest_location(),second)
        OPERA_TEST_EQUALS(trace1.barrier(trace1.size()-1).maximum_index(),0)

        auto hs2_new = h.segment(0).create_sample(Point(5,5,0),Point(5,5,0));
        trace2.reset(hs2_new,history);
        OPERA_TEST_PRINT(trace2)
        OPERA_TEST_EQUALS(trace2.barrier(trace2.size()-1).farthest_location(),first)

        auto hs3_new = h.segment(0).create_sample(Point(10,10,0),Point(10,10,0));
        trace3.reset(hs3_new,history);
        OPERA_TEST_ASSERT(trace3.is_empty())
    }
};

int main() {
    TestBarrier().test();
    return OPERA_TEST_FAILURES;
}
