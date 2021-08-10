/***************************************************************************
 *            test_body.cpp
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

#include "body.hpp"

using namespace Opera;

class TestBody {
public:
    void test() {
        ARIADNE_TEST_CALL(test_bodysegment_creation())
        ARIADNE_TEST_CALL(test_bodysegment_intersection())
        ARIADNE_TEST_CALL(test_body())
    }

    void test_bodysegment_creation() {

        FloatType thickness(0.5,Ariadne::dp);
        BodySegment segment(15,0,1,thickness);

        ARIADNE_TEST_EQUALS(segment.id(),15)
        ARIADNE_TEST_EQUALS(segment.head_id(),0)
        ARIADNE_TEST_EQUALS(segment.tail_id(),1)
        ARIADNE_TEST_EQUALS(segment.thickness(),0.5)

        Point head(0,0.5,1.0);
        Point tail(1.0,2.0,-1.0);

        auto s = segment.create_state(head, tail, 32490234);

        auto hp = s.head_position();
        auto tp = s.tail_position();
        auto ts = s.timestamp();
        auto bb = s.bounding_box();

        ARIADNE_TEST_EQUALS(hp.x,0)
        ARIADNE_TEST_EQUALS(hp.y,0.5)
        ARIADNE_TEST_EQUALS(hp.z,1)
        ARIADNE_TEST_EQUALS(tp.x,1)
        ARIADNE_TEST_EQUALS(tp.y,2)
        ARIADNE_TEST_EQUALS(tp.z,-1)
        ARIADNE_TEST_EQUALS(ts,32490234)
        ARIADNE_TEST_EQUALS(bb.dimension(),3)
        ARIADNE_TEST_EQUALS(bb[0].lower_bound(),-0.5)
        ARIADNE_TEST_EQUALS(bb[0].upper_bound(),1.5)
        ARIADNE_TEST_EQUALS(bb[1].lower_bound(),0.0)
        ARIADNE_TEST_EQUALS(bb[1].upper_bound(),2.5)
        ARIADNE_TEST_EQUALS(bb[2].lower_bound(),-1.5)
        ARIADNE_TEST_EQUALS(bb[2].upper_bound(),1.5)
    }

    void test_bodysegment_intersection() {
        FloatType thickness(1.0,Ariadne::dp);
        BodySegment segment(0,0,1,thickness);

        auto s1 = segment.create_state(Point(0, 0, 0), Point(5, 5, 5), 32490234);
        auto s2 = segment.create_state(Point(0, 3, 0), Point(5, 5, 5), 32490234);
        auto s3 = segment.create_state(Point(0, 3, 0), Point(5, 6, 5), 32490234);
        auto s4 = segment.create_state(Point(0, 3, 3), Point(0, 8, 8), 32490234);
        auto s5 = segment.create_state(Point(2.01, 3, 3), Point(2.01, 5, 5), 32490234);
        auto s6 = segment.create_state(Point(2, 3, 3), Point(2, 5, 5), 32490234);
        auto s7 = segment.create_state(Point(0, 8, 0), Point(0, 10, 0), 32490234);

        ARIADNE_TEST_PRINT(s1.bounding_box())
        ARIADNE_TEST_PRINT(s2.bounding_box())
        ARIADNE_TEST_PRINT(s3.bounding_box())
        ARIADNE_TEST_PRINT(s4.bounding_box())
        ARIADNE_TEST_PRINT(s5.bounding_box())
        ARIADNE_TEST_PRINT(s6.bounding_box())
        ARIADNE_TEST_PRINT(s7.bounding_box())

        ARIADNE_TEST_PRINT(distance(s1,s2))
        ARIADNE_TEST_PRINT(distance(s1,s3))
        ARIADNE_TEST_PRINT(distance(s1,s4))
        ARIADNE_TEST_PRINT(distance(s4,s5))
        ARIADNE_TEST_PRINT(distance(s4,s6))

        ARIADNE_TEST_ASSERT(s1.intersects(s2))
        ARIADNE_TEST_ASSERT(s1.intersects(s3))
        ARIADNE_TEST_ASSERT(not s1.intersects(s4))
        ARIADNE_TEST_ASSERT(not s4.intersects(s5))
        ARIADNE_TEST_ASSERT(s4.intersects(s6))
        ARIADNE_TEST_ASSERT(not s1.intersects(s7))
    }

    void test_body() {

    }
};


int main() {
    TestBody().test();

    return ARIADNE_TEST_FAILURES;
}
