/***************************************************************************
 *            test_geometry.cpp
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

class TestGeometry {
public:
    void test() {
        OPERA_TEST_CALL(test_construct_point())
        OPERA_TEST_CALL(test_segment_segment_distance())
        OPERA_TEST_CALL(test_point_segment_distance())
        OPERA_TEST_CALL(test_point_point_distance())
        OPERA_TEST_CALL(test_centre())
        OPERA_TEST_CALL(test_hull())
        OPERA_TEST_CALL(test_circle_radius())
    }

    void test_construct_point() {
        Point p(1.0,-2.1,0);
    }

    void test_segment_segment_distance() {
        OPERA_TEST_EQUALS(distance(Point(1,2,3),Point(1,2,3),Point(1,2,3),Point(1,2,3)),0)
        OPERA_TEST_EQUALS(distance(Point(1,2,3),Point(3,4,5),Point(1,2,3),Point(3,4,5)),0)
        OPERA_TEST_EQUALS(distance(Point(1,2,3),Point(0,0,0),Point(0,0,0),Point(2,2,2)),0)
        OPERA_TEST_EQUALS(distance(Point(0,0,0),Point(0,3,0),Point(0,4,0),Point(0,2,0)),0)
        OPERA_TEST_EQUALS(distance(Point(1,0,0),Point(3,0,0),Point(1,1,0),Point(3,1,0)),1)
        OPERA_TEST_EQUALS(distance(Point(1,0,0),Point(3,0,0),Point(0,0,0),Point(0,2,0)),1)

        OPERA_TEST_EXECUTE(distance(Point(-0.5073,-0.3273,-0.6143),Point(-0.8391,0.8633,-0.1950),Point(-0.2479,-0.6319,0.2624),Point(0.3919,-0.1700,0.8694)))
        OPERA_TEST_EXECUTE(distance(Point(0.7283,0.4762,0.1598),Point(0.6620,-0.07481,0.08877),Point(-0.6654,-0.6032,-0.9962),Point(-0.6910,-0.8980,-0.5835)))
        OPERA_TEST_EXECUTE(distance(Point(0.9076,-0.5889,-0.3511),Point(0.9236,0.6130,-0.9872),Point(-0.8805,-0.2538,0.6383),Point(-0.2311,-0.5325,0.9485)))
        OPERA_TEST_EXECUTE(distance(Point(-0.9097,-0.4835,0.3973),Point(-0.2489,-0.1628,-0.5455),Point(0.3303,0.9305,-0.1387),Point(0.7753,0.3848,0.9415)))
    }

    void test_point_segment_distance() {
        auto d1 = distance(Point(1.308,-2.690,1.567),Point(1.308,-2.690,1.567),Point(-1.174,4.631,-0.1193),Point(-4.892,-2.183,-3.825));
        OPERA_TEST_ASSERT(decide(distance(Point(1.308,-2.690,1.567),Point(-1.174,4.631,-0.1193),Point(-4.892,-2.183,-3.825))-d1 < 1e-8))
        auto d2 = distance(Point(-0.1053,-0.1488,-2.390),Point(-0.1053,-0.1488,-2.390),Point(2.964,-1.106,0.4021),Point(2.887,-3.345,2.290));
        OPERA_TEST_ASSERT(decide(distance(Point(-0.1053,-0.1488,-2.390),Point(2.964,-1.106,0.4021),Point(2.887,-3.345,2.290))-d2 < 1e-8))
        auto d3 = distance(Point(-1.560,3.773,-4.831),Point(-1.560,3.773,-4.831),Point(1.941,-1.352,3.894),Point(-4.736,0.9957,0.6373));
        OPERA_TEST_ASSERT(decide(distance(Point(-1.560,3.773,-4.831),Point(1.941,-1.352,3.894),Point(-4.736,0.9957,0.6373))-d3 < 1e-8))
    }

    void test_point_point_distance() {
        OPERA_TEST_EQUALS(distance(Point(1,2,3),Point(1,2,3)),0)
        OPERA_TEST_EQUALS(distance(Point(1,2,3),Point(-1,2,3)),2)
        OPERA_TEST_EQUALS(distance(Point(1,2,3),Point(4,-2,3)),5)
    }

    void test_centre() {
        Point p1(1.0,3.0,-2.0);
        Point p2(4.0,1.2,0);

        auto c = centre(p1, p2);

        OPERA_TEST_EQUALS(c.x,2.5)
        OPERA_TEST_EQUALS(c.y,2.1)
        OPERA_TEST_EQUALS(c.z,-1.0)
    }

    void test_hull() {
        Point p1(4.0,3.0,-2.0);
        Point p2(4.0,1.2,0);

        auto h = hull(p1,p2);

        OPERA_TEST_EQUALS(h[0].lower_bound(),4.0)
        OPERA_TEST_EQUALS(h[0].upper_bound(),4.0)
        OPERA_TEST_EQUALS(h[1].lower_bound(),1.2)
        OPERA_TEST_EQUALS(h[1].upper_bound(),3.0)
        OPERA_TEST_EQUALS(h[2].lower_bound(),-2.0)
        OPERA_TEST_EQUALS(h[2].upper_bound(),0)
    }

    void test_circle_radius() {
        BoundingType bb({FloatIntervalType(FloatType(1, dp), FloatType(2, dp)),
                         FloatIntervalType(FloatType(-1, dp), FloatType(2, dp)),
                         FloatIntervalType(FloatType(4, dp), FloatType(6, dp))});
        OPERA_TEST_ASSERT(decide(circle_radius(bb) - 1.8708 < 1e-3))
    }
};


int main() {
    TestGeometry().test();

    return OPERA_TEST_FAILURES;
}
