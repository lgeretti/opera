/***************************************************************************
 *            test_algebra.cpp
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

#include "geometry.hpp"

using namespace Opera;

class TestAlgebra {
public:
    void test() {
        ARIADNE_TEST_CALL(test_construct_point())
        ARIADNE_TEST_CALL(test_center())
    }

    void test_construct_point() {
        Point p(1.0,-2.1,0);
    }

    void test_center() {
        Point p1(1.0,3.0,-2.0);
        Point p2(4.0,1.2,0);

        auto c = center(p1,p2);

        ARIADNE_TEST_EQUALS(c.x,2.5)
        ARIADNE_TEST_EQUALS(c.y,2.1)
        ARIADNE_TEST_EQUALS(c.z,-1.0)
    }
};


int main() {
    TestAlgebra().test();

    return ARIADNE_TEST_FAILURES;
}
