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
        ARIADNE_TEST_CALL(test_verification_trace_creation())
    }

    void test_verification_trace_creation() {
        TimestampType const initial_time = 349234;
        StringVariable robot("robot");
        DiscreteLocation const initial_location(robot|"first");
        VerificationTrace trace = SimpleVerificationTrace("h0",2u,"r0",3u,initial_time,initial_location);
        ARIADNE_TEST_EQUALS(trace.human_id(),"h0")
        ARIADNE_TEST_EQUALS(trace.human_segment_id(),2u)
        ARIADNE_TEST_EQUALS(trace.robot_id(),"r0")
        ARIADNE_TEST_EQUALS(trace.robot_segment_id(),3u)
        ARIADNE_TEST_EQUALS(trace.initial_time(),initial_time)
        ARIADNE_TEST_EQUALS(trace.initial_robot_discrete_state(),initial_location)
    }
};

int main() {
    TestVerification().test();
    return ARIADNE_TEST_FAILURES;
}
