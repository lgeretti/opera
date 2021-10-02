/***************************************************************************
 *            test_discrete_state.cpp
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
        OPERA_TEST_FAIL(state4 == state2)
        OPERA_TEST_FAIL(state2 == state4)
    }
};

int main() {
    TestDiscreteState().test();
    return OPERA_TEST_FAILURES;
}
