/***************************************************************************
 *            test_verification.cpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of Opera, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "verification.hpp"

#include "test.hpp"

using namespace Opera;

class TestVerification {
  public:
    void test() {
        OPERA_TEST_CALL(test_verification_trace_creation())
    }

    void test_verification_trace_creation() {
        TimestampType const initial_time = 349234;
        DiscreteState const initial_location({"robot","first"});
        VerificationTrace trace = SimpleVerificationTrace("h0",2u,"r0",3u,initial_time,initial_location);
        OPERA_TEST_EQUALS(trace.human_id(),"h0")
        OPERA_TEST_EQUALS(trace.human_segment_id(),2u)
        OPERA_TEST_EQUALS(trace.robot_id(),"r0")
        OPERA_TEST_EQUALS(trace.robot_segment_id(),3u)
        OPERA_TEST_EQUALS(trace.initial_time(),initial_time)
        OPERA_TEST_EQUALS(trace.initial_robot_discrete_state(),initial_location)
        OPERA_TEST_EQUALS(trace.robot_discrete_trace().size(),0u)
    }
};

int main() {
    TestVerification().test();
    return OPERA_TEST_FAILURES;
}
