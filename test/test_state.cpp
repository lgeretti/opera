/***************************************************************************
 *            test_state.cpp
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

#include "state.hpp"

using namespace Opera;

class TestState {
public:
    void test() {
        ARIADNE_TEST_CALL(test_human_state_instance())
        ARIADNE_TEST_CALL(test_robot_discrete_state_trace_creation())
        ARIADNE_TEST_CALL(test_robot_discrete_state_trace_next_locations())
        ARIADNE_TEST_CALL(test_robot_state_history_basics())
        ARIADNE_TEST_CALL(test_robot_state_history_analytics())
    }

    void test_human_state_instance() {
        Human h("h0", {{3,2},{1,0}}, {FloatType(0.5,Ariadne::dp),FloatType(1.0,Ariadne::dp)});
        HumanStateInstance instance(h,{{Point(0,0,0)},{Point(4,4,4)},{Point(0,2,0)},{Point(1,0,3)}},5e8);

        ARIADNE_TEST_EQUALS(instance.samples().size(),2)
        ARIADNE_TEST_EQUALS(instance.timestamp(),5e8)
    }

    void test_robot_discrete_state_trace_creation() {
        Ariadne::StringVariable robot("robot");
        DiscreteLocation first(robot|"first"), second(robot|"second"), third(robot|"third");

        RobotDiscreteTrace trace;
        trace.push_front(second).push_back(first).push_back(second).push_front(third);
        ARIADNE_TEST_EQUALS(trace.size(),4)

        List<DiscreteLocation> locations = {third,second,first,second};

        ARIADNE_TEST_EQUALS(trace.at(0),third)
        ARIADNE_TEST_EQUALS(trace.at(1),second)
        ARIADNE_TEST_EQUALS(trace.at(2),first)
        ARIADNE_TEST_EQUALS(trace.at(3),second)
        ARIADNE_TEST_EQUALS(trace.probability(),pa_one)

        RobotDiscreteTrace trace2;
        trace2 = trace;
        ARIADNE_TEST_EQUALS(trace2,trace)
    }

    void test_robot_discrete_state_trace_next_locations() {
        Ariadne::StringVariable r("r");
        DiscreteLocation a(r|"a"), b(r|"b"), c(r|"c"), d(r|"d");

        // abcabd -> *****d -> {}
        auto next1 = RobotDiscreteTrace().push_back(a).push_back(b).push_back(c).push_back(a).push_back(b).
                push_back(d).next_locations();
        ARIADNE_TEST_EQUALS(next1.size(),0)

        // abcabdacbcabcdac -> ******ac******ac -> {b}
        auto next2 = RobotDiscreteTrace().push_back(a).push_back(b).push_back(c).push_back(a).push_back(b).
                push_back(d).push_back(a).push_back(c).push_back(b).push_back(c).push_back(a).
                push_back(b).push_back(c).push_back(d).push_back(a).push_back(c).next_locations();
        ARIADNE_TEST_EQUALS(next2.size(),1)
        ARIADNE_TEST_ASSERT(next2.has_key(b))
        ARIADNE_TEST_EQUALS(next2.at(b),cast_positive(FloatType(1.0,dp)))

        // abcbacabcbacbacb -> **cb*****cb*cb*cb -> {a}
        auto next3 = RobotDiscreteTrace().push_back(a).push_back(b).push_back(c).push_back(b).push_back(a).
                push_back(c).push_back(a).push_back(b).push_back(c).push_back(b).push_back(a).
                push_back(c).push_back(b).push_back(a).push_back(c).push_back(b).next_locations();
        ARIADNE_TEST_EQUALS(next3.size(),1)
        ARIADNE_TEST_ASSERT(next3.has_key(a))
        ARIADNE_TEST_EQUALS(next3.at(a),cast_positive(FloatType(1.0,dp)))

        // abdabcabcdabadbc -> ****bc*bc*****bc -> {a,d}
        auto next4 = RobotDiscreteTrace().push_back(a).push_back(b).push_back(d).push_back(a).push_back(b).
                push_back(c).push_back(a).push_back(b).push_back(c).push_back(d).push_back(a).
                push_back(b).push_back(a).push_back(d).push_back(b).push_back(c).next_locations();
        ARIADNE_TEST_EQUALS(next4.size(),2)
        ARIADNE_TEST_ASSERT(next4.has_key(a))
        ARIADNE_TEST_ASSERT(next4.has_key(d))
        ARIADNE_TEST_EQUALS(next4.at(a),cast_positive(FloatType(0.5,dp)))
        ARIADNE_TEST_EQUALS(next4.at(d),cast_positive(FloatType(0.5,dp)))

        // dcbadcbdcbdcbcdcb -> dcb*dcbdcbdcb*dcb -> {a(0.25),c(0.25),d(0.5)}
        auto trace5 = RobotDiscreteTrace().push_back(d).push_back(c).push_back(b).push_back(a).push_back(d).
                push_back(c).push_back(b).push_back(d).push_back(c).push_back(b).push_back(d).
                push_back(c).push_back(b).push_back(c).push_back(d).push_back(c).push_back(b);
        auto next5 = trace5.next_locations();
        ARIADNE_TEST_EQUALS(next5.size(),3)
        ARIADNE_TEST_ASSERT(next5.has_key(a))
        ARIADNE_TEST_ASSERT(next5.has_key(c))
        ARIADNE_TEST_ASSERT(next5.has_key(d))
        ARIADNE_TEST_EQUALS(next5.at(a),cast_positive(FloatType(0.25,dp)))
        ARIADNE_TEST_EQUALS(next5.at(c),cast_positive(FloatType(0.25,dp)))
        ARIADNE_TEST_EQUALS(next5.at(d),cast_positive(FloatType(0.5,dp)))

        // adding each next state
        auto trace5a = trace5;
        trace5a.push_back(a,cast_positive(FloatType(0.25,dp)));
        auto trace5c = trace5;
        trace5c.push_back(c,cast_positive(FloatType(0.25,dp)));
        auto trace5d = trace5;
        trace5d.push_back(d,cast_positive(FloatType(0.5,dp)));
        auto next5a = trace5a.next_locations();
        auto next5c = trace5c.next_locations();
        auto next5d = trace5d.next_locations();
        ARIADNE_TEST_EQUALS(next5a.size(),1)
        ARIADNE_TEST_EQUALS(next5c.size(),1)
        ARIADNE_TEST_EQUALS(next5d.size(),1)
        ARIADNE_TEST_ASSERT(next5a.has_key(d))
        ARIADNE_TEST_ASSERT(next5c.has_key(d))
        ARIADNE_TEST_ASSERT(next5d.has_key(c))
        ARIADNE_TEST_EQUALS(next5a.at(d),cast_positive(FloatType(0.25,dp)))
        ARIADNE_TEST_EQUALS(next5c.at(d),cast_positive(FloatType(0.25,dp)))
        ARIADNE_TEST_EQUALS(next5d.at(c),cast_positive(FloatType(0.5,dp)))

        // adding each next state
        auto trace5ad = trace5a.push_back(d);
        auto trace5cd = trace5c.push_back(d);
        auto trace5dc = trace5d.push_back(c);
        auto next5ad = trace5ad.next_locations();
        auto next5cd = trace5cd.next_locations();
        auto next5dc = trace5dc.next_locations();
        ARIADNE_TEST_EQUALS(next5ad.size(),1)
        ARIADNE_TEST_EQUALS(next5cd.size(),1)
        ARIADNE_TEST_EQUALS(next5dc.size(),1)
        ARIADNE_TEST_ASSERT(next5ad.has_key(c))
        ARIADNE_TEST_ASSERT(next5cd.has_key(c))
        ARIADNE_TEST_ASSERT(next5dc.has_key(b))

        // adding each next state
        auto trace5adc = trace5ad.push_back(c);
        auto trace5cdc = trace5cd.push_back(c);
        auto trace5dcb = trace5dc.push_back(b);
        auto next5adc = trace5adc.next_locations();
        auto next5cdc = trace5cdc.next_locations();
        auto next5dcb = trace5dcb.next_locations();
        ARIADNE_TEST_EQUALS(next5adc.size(),1)
        ARIADNE_TEST_EQUALS(next5cdc.size(),1)
        ARIADNE_TEST_EQUALS(next5dcb.size(),2)
        ARIADNE_TEST_ASSERT(next5adc.has_key(b))
        ARIADNE_TEST_ASSERT(next5cdc.has_key(b))
        ARIADNE_TEST_ASSERT(next5dcb.has_key(c))
        ARIADNE_TEST_ASSERT(next5dcb.has_key(d))
        ARIADNE_TEST_EQUALS(next5dcb.at(c),cast_positive(FloatType(0.25,dp)))
        ARIADNE_TEST_EQUALS(next5dcb.at(d),cast_positive(FloatType(0.25,dp)))
     }

    void test_robot_state_history_basics() {
        Ariadne::StringVariable robot("robot");
        Robot r("r0", 10, {{3, 2},{1, 0}}, {FloatType(1.0, Ariadne::dp), FloatType(0.5, Ariadne::dp)});
        RobotStateHistory history(&r);

        DiscreteLocation empty_location;
        ARIADNE_TEST_ASSERT(history.current_location().values().empty())
        ARIADNE_TEST_ASSERT(history.presences_in(empty_location).empty())
        ARIADNE_TEST_ASSERT(history.presences_exiting_into(empty_location).empty())
        ARIADNE_TEST_FAIL(history.samples(empty_location))

        DiscreteLocation first(robot|"first"), second(robot|"second");

        history.acquire(first,{{Point(0,0,0)},{Point(4,4,4)},{Point(0,2,0)},{Point(1,0,3)}},5e8);
        ARIADNE_TEST_FAIL(history.samples(first))
        ARIADNE_TEST_ASSERT(not history.has_samples(first))
        ARIADNE_TEST_EQUALS(history.current_location(),first)
        auto entrances = history.presences_exiting_into(first);
        ARIADNE_TEST_EQUAL(entrances.size(),1)
        ARIADNE_TEST_ASSERT(entrances.back().location().values().empty())
        ARIADNE_TEST_EQUALS(entrances.back().to(), 5e8)

        history.acquire(first,{{Point(0,0,1)},{Point(4,4,5)},{Point(0,3,0)},{Point(1,1,3)}},6e8);
        history.acquire(second,{{Point(0,0,1.5)},{Point(4,4,5.5)},{Point(0,3.5,0)},{Point(1,1.5,3)}},7e8);
        ARIADNE_TEST_EQUALS(history.current_location(),second)
        ARIADNE_TEST_ASSERT(not history.has_samples(second))
        ARIADNE_TEST_EQUALS(history.presences_in(first).size(), 1)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(second).size(), 1)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(second).back().location(), first)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(second).back().from(), 5e8)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(second).back().to(), 7e8)
        ARIADNE_TEST_EQUALS(history.range_of_num_samples_in(first), Interval<Natural>(2u, 2u))
        ARIADNE_TEST_ASSERT(history.has_samples(first))
        auto samples = history.samples(first);
        ARIADNE_TEST_EQUALS(samples.size(),2)
        ARIADNE_TEST_PRINT(history.samples(first))
        ARIADNE_TEST_ASSERT(decide(history.samples(first).at(0).at(0).error() == 0))

        history.acquire(first,{{Point(0,0,2),Point(0,0.1,2)},{Point(4,4,6)},{Point(0,4,0)},{Point(1,2,3),Point(1.1,2,3)}},8e8);
        ARIADNE_TEST_PRINT(history.presences_exiting_into(first))
        ARIADNE_TEST_ASSERT(history.has_samples(second))
        ARIADNE_TEST_EQUALS(history.presences_in(second).size(), 1)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(first).size(), 2)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(first).back().location(), second)
        ARIADNE_TEST_EQUALS(history.samples(first).at(0).size(),2)
        ARIADNE_TEST_EQUALS(history.samples(second).at(0).size(),1)
        ARIADNE_TEST_PRINT(history.samples(second))
        history.acquire(first,{{Point(1,0,2)},{Point(5,4,6)},{Point(1,4,0)},{Point(2,2,3)}},9e8);

        history.acquire(second,{{Point(1,0,1.5)},{Point(5,4,5.5)},{Point(1,3.5,0)},{Point(2,1.5,3)}},10e8);
        ARIADNE_TEST_EQUALS(history.samples(first).at(0).size(),2)
        ARIADNE_TEST_EQUALS(history.presences_in(first).size(), 2)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(second).size(), 2)
        ARIADNE_TEST_PRINT(history.samples(first))
        ARIADNE_TEST_ASSERT(decide(history.samples(first).at(0).at(0).error() > 0))
        auto history_trace = history.discrete_trace();
        ARIADNE_TEST_PRINT(history_trace)
        auto expected_trace = RobotDiscreteTrace().push_back(first).push_back(second).push_back(first).push_back(second);
        ARIADNE_TEST_EQUALS(history_trace,expected_trace)
    }

    void test_robot_state_history_analytics() {
        Ariadne::StringVariable robot("robot");
        Robot r("r0", 10, {{0,1}}, {FloatType(1.0, Ariadne::dp)});
        RobotStateHistory history(&r);

        DiscreteLocation first(robot|"first"), second(robot|"second"), third(robot|"third"), fourth(robot|"fourth"), fifth(robot|"fifth");

        TimestampType ts = 0u;
        history.acquire(first,{{Point(0,0,0)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(first,{{Point(1,0,0)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(second,{{Point(1,1,0)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(second,{{Point(1,2,0)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(second,{{Point(1,3,0)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(third,{{Point(1,3,1)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(third,{{Point(1,3,2)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(second,{{Point(1,4,2)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(second,{{Point(1,5,2)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(first,{{Point(2,5,2)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(first,{{Point(3,5,2)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(first,{{Point(4,5,2)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(third,{{Point(4,5,3)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(second,{{Point(4,6,3)},{Point(4,4,4)}},ts); ts+= 1e8;
        history.acquire(fourth,{{Point(4,6,3)},{Point(5,4,4)}},ts); ts+= 1e8;

        ARIADNE_TEST_EQUALS(history.current_location(),fourth)

        ARIADNE_TEST_EQUALS(history.presences_in(first).size(), 2)
        ARIADNE_TEST_EQUALS(history.presences_in(second).size(), 3)
        ARIADNE_TEST_EQUALS(history.presences_in(third).size(), 2)
        ARIADNE_TEST_EQUALS(history.presences_in(fourth).size(), 0)
        ARIADNE_TEST_EQUALS(history.presences_in(fifth).size(), 0)

        ARIADNE_TEST_EQUALS(history.presences_exiting_into(first).size(),2)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(second).size(),3)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(third).size(),2)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(fourth).size(),1)
        ARIADNE_TEST_EQUALS(history.presences_exiting_into(fifth).size(),0)

        ARIADNE_TEST_EQUALS(history.range_of_num_samples_in(first), Interval<Natural>(2u, 3u))
        ARIADNE_TEST_EQUALS(history.range_of_num_samples_in(second), Interval<Natural>(1u, 3u))
        ARIADNE_TEST_EQUALS(history.range_of_num_samples_in(third), Interval<Natural>(1u, 2u))
        ARIADNE_TEST_EQUALS(history.range_of_num_samples_in(fourth), Interval<Natural>(0u, 0u))
        ARIADNE_TEST_EQUALS(history.range_of_num_samples_in(fifth), Interval<Natural>(0u, 0u))

        ARIADNE_TEST_EQUALS(history.presences_between(first,third).size(),1)
        ARIADNE_TEST_EQUALS(history.presences_between(first,second).size(),1)
        ARIADNE_TEST_EQUALS(history.presences_between(second,third).size(),1)
        ARIADNE_TEST_EQUALS(history.presences_between(third,second).size(),2)
        ARIADNE_TEST_EQUALS(history.presences_between(third,first).size(),0)
        ARIADNE_TEST_EQUALS(history.presences_between(second,fourth).size(),1)

        ARIADNE_TEST_EQUALS(history.range_of_num_samples_between(third,first),Interval<Natural>(0u, 0u))
        ARIADNE_TEST_EQUALS(history.range_of_num_samples_between(first,second),Interval<Natural>(2u, 2u))
        ARIADNE_TEST_EQUALS(history.range_of_num_samples_between(first,third),Interval<Natural>(3u, 3u))
        ARIADNE_TEST_EQUALS(history.range_of_num_samples_between(third,second),Interval<Natural>(1u, 2u))
    }
};


int main() {
    TestState().test();

    return ARIADNE_TEST_FAILURES;
}
