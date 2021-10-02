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

#include "body.hpp"

#include "test.hpp"

using namespace Opera;

class TestBody {
public:
    void test() {
        OPERA_TEST_CALL(test_body_creation())
        OPERA_TEST_CALL(test_bodysegment_creation())
        OPERA_TEST_CALL(test_bodysegment_update())
        OPERA_TEST_CALL(test_bodysegment_intersection())
        OPERA_TEST_CALL(test_spherical_approximation())
    }

    void test_body_creation() {
        Human h("h0", {{3,2},{1,0}}, {0.5,1.0});

        OPERA_TEST_PRINT(h)
        OPERA_TEST_EQUALS(h.id(),"h0")
        OPERA_TEST_EQUALS(h.num_segments(),2)
        OPERA_TEST_EQUALS(h.num_points(),4)

        Robot r("r0", 10, {{0,1}}, {0.5});
        OPERA_TEST_PRINT(r)
        OPERA_TEST_EQUALS(r.id(),"r0")
        OPERA_TEST_EQUALS(r.num_segments(),1)
        OPERA_TEST_EQUALS(r.num_points(),2)
        OPERA_TEST_EQUALS(r.packet_frequency(), 10)
    }

    void test_bodysegment_creation() {

        Robot r("r0", 10, {{3, 2},{1, 0}}, {1.0, 0.5});
        auto segment = r.segment(1);

        OPERA_TEST_EQUALS(segment.id(),1)
        OPERA_TEST_EQUALS(segment.head_id(),1)
        OPERA_TEST_EQUALS(segment.tail_id(),0)
        OPERA_TEST_EQUALS(segment.thickness(),0.5)

        auto s1 = segment.create_sample();
        OPERA_TEST_ASSERT(s1.is_empty())
    }

    void test_bodysegment_update() {

        FloatType thickness = 1.0;

        Robot r("r0", 10, {{0, 1}}, {thickness});
        auto segment = r.segment(0);

        auto s1 = segment.create_sample();
        s1.update({Point(-0.5, 1.0, 1.25)},{});
        OPERA_TEST_ASSERT(s1.is_empty())
        s1.update({},{Point(1.0, 2.5, 0.0)});
        OPERA_TEST_ASSERT(not s1.is_empty())

        auto s2 = segment.create_sample();
        s2.update({},{Point(1.0, 2.5, 0.0)});
        OPERA_TEST_ASSERT(s2.is_empty())
        s2.update({Point(-0.5, 1.0, 1.25)},{});
        OPERA_TEST_ASSERT(not s2.is_empty())

        auto s3 = segment.create_sample();
        s3.update({Point(-0.5, 1.0, 1.25)}, {Point(1.0, 2.5, 0.0)});
        OPERA_TEST_ASSERT(not s3.is_empty())

        auto s4 = segment.create_sample();
        s4.update({Point(0,0.5,1.0),Point(-0.5, 1.0, 1.25)},{Point(1.0,2.0,-1.0),Point(1.0, 2.5, 0.0)});
        auto err = s4.error();

        OPERA_TEST_EQUALS(s4.head_centre().x, -0.25)
        OPERA_TEST_EQUALS(s4.head_centre().y, 0.75)
        OPERA_TEST_EQUALS(s4.head_centre().z, 1.125)
        OPERA_TEST_EQUALS(s4.tail_centre().x, 1.0)
        OPERA_TEST_EQUALS(s4.tail_centre().y, 2.25)
        OPERA_TEST_EQUALS(s4.tail_centre().z, -0.5)

        auto bb = s4.bounding_box();
        OPERA_TEST_EQUALS(bb.xl(),s4.head_centre().x-err-thickness)
        OPERA_TEST_EQUALS(bb.xu(),s4.tail_centre().x+err+thickness)
        OPERA_TEST_EQUALS(bb.yl(),s4.head_centre().y-err-thickness)
        OPERA_TEST_EQUALS(bb.yu(),s4.tail_centre().y+err+thickness)
        OPERA_TEST_EQUALS(bb.zl(),s4.tail_centre().z-err-thickness)
        OPERA_TEST_EQUALS(bb.zu(),s4.head_centre().z+err+thickness)
    }

    void test_bodysegment_intersection() {
        FloatType thickness = 1.0;
        Robot r("r0", 10,{{0, 1}}, {thickness});
        auto segment = r.segment(0);

        auto s1 = segment.create_sample({Point(0, 0, 0)}, {Point(5, 5, 5)});
        auto s2 = segment.create_sample({Point(0, 3, 0)}, {Point(5, 5, 5)});
        auto s3 = segment.create_sample({Point(0, 3, 0)}, {Point(5, 6, 5)});
        auto s4 = segment.create_sample({Point(0, 3, 3)}, {Point(0, 8, 8)});
        auto s5 = segment.create_sample({Point(2.01, 3, 3)}, {Point(2.01, 5, 5)});
        auto s6 = segment.create_sample({Point(2, 3, 3)}, {Point(2, 5, 5)});
        auto s7 = segment.create_sample({Point(0, 8, 0)}, {Point(0, 10, 0)});

        OPERA_TEST_PRINT(s1.bounding_box())
        OPERA_TEST_PRINT(s2.bounding_box())
        OPERA_TEST_PRINT(s3.bounding_box())
        OPERA_TEST_PRINT(s4.bounding_box())
        OPERA_TEST_PRINT(s5.bounding_box())
        OPERA_TEST_PRINT(s6.bounding_box())
        OPERA_TEST_PRINT(s7.bounding_box())

        OPERA_TEST_PRINT(distance(s1,s2))
        OPERA_TEST_PRINT(distance(s1,s3))
        OPERA_TEST_PRINT(distance(s1,s4))
        OPERA_TEST_PRINT(distance(s4,s5))
        OPERA_TEST_PRINT(distance(s4,s6))

        OPERA_TEST_ASSERT(s1.intersects(s2))
        OPERA_TEST_ASSERT(s1.intersects(s3))
        OPERA_TEST_ASSERT(not s1.intersects(s4))
        OPERA_TEST_ASSERT(not s4.intersects(s5))
        OPERA_TEST_ASSERT(s4.intersects(s6))
        OPERA_TEST_ASSERT(not s1.intersects(s7))
    }

    void test_spherical_approximation() {
        Robot r("r0", 10, {{3, 2},{1, 0}}, {1.0, 0.5});
        auto robot_sample = r.segment(0).create_sample();
        robot_sample.update({Point(0,0,0)},{Point(2,0,0)});
        auto human_sample = r.segment(0).create_sample();
        human_sample.update({Point(1,5,0)},{Point(2,5,0)});

        auto human_sas = human_sample.spherical_approximation();
        OPERA_TEST_PRINT(human_sas)
        OPERA_TEST_EQUAL(human_sas.centre(),Point(1.5,5,0))
        OPERA_TEST_ASSERT(human_sas.radius()-2.062 <= 1e-3)
        OPERA_TEST_ASSERT(distance(human_sas,robot_sample)-1.938 <= 1e-3)
    }
};


int main() {
    TestBody().test();

    return OPERA_TEST_FAILURES;
}
