/***************************************************************************
 *            test_discrete_state.cpp
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

#include "discrete_state.hpp"

#include "test.hpp"

using namespace Opera;

class TestDiscreteState {
  public:
    void test() {
        OPERA_TEST_CALL(test_construction())
        OPERA_TEST_CALL(test_comparison())
    }

    void test_construction() {
        DiscreteState state1;
        OPERA_TEST_ASSERT(state1.is_empty())
        OPERA_TEST_PRINT(state1)
        DiscreteState state2({"robot","first"});
        OPERA_TEST_ASSERT(not state2.is_empty())
        OPERA_TEST_PRINT(state2)
        DiscreteState state3({{"phase","preparing"},{"source","table"}});
        OPERA_TEST_EQUAL(state3.values().size(),2)
        OPERA_TEST_PRINT(state3)
    }

    void test_comparison() {
        DiscreteState state1({"robot","first"});
        DiscreteState state2({"robot","first"});
        DiscreteState state3({"robot","second"});
        DiscreteState state4({"other","first"});
        OPERA_TEST_EQUAL(state1,state2)
        OPERA_TEST_ASSERT(state1 < state3)
        OPERA_TEST_ASSERT(state4 < state3)
        OPERA_TEST_ASSERT(not (state1 == state3))
        try {
            bool result = (state4 == state2);
            OPERA_PRINT_TEST_COMMENT("Error: expected exception, got " << result)
            ++OPERA_TEST_FAILURES;
        } catch (...) {
            OPERA_PRINT_TEST_COMMENT("Exception caught as expected")
        }
        try {
            bool result = (state2 == state4);
            OPERA_PRINT_TEST_COMMENT("Error: expected exception, got " << result)
            ++OPERA_TEST_FAILURES;
        } catch (...) {
            OPERA_PRINT_TEST_COMMENT("Exception caught as expected")
        }
    }
};

int main() {
    TestDiscreteState().test();
    return OPERA_TEST_FAILURES;
}
