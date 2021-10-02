/***************************************************************************
 *            test_interval.cpp
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

#include "interval.hpp"

using namespace Opera;

class TestInterval {
public:
    void test() {
        OPERA_TEST_CALL(test_construct())
        OPERA_TEST_CALL(test_equality())
    }

    void test_construct() {
        Interval<FloatType> interval(0.1,2.0);
        OPERA_TEST_EQUAL(interval.lower(),0.1)
        OPERA_TEST_EQUAL(interval.upper(),2.0)
        OPERA_TEST_PRINT(interval)
    }

    void test_equality() {
        Interval<FloatType> i1(0.1,2.0);
        Interval<FloatType> i2(0.0,1.0);
        OPERA_TEST_ASSERT(i1 != i2)
        i2.set_lower(0.1);
        i2.set_upper(2.0);
        OPERA_TEST_ASSERT(i1 == i2)
    }
};


int main() {
    TestInterval().test();

    return OPERA_TEST_FAILURES;
}
