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
        ARIADNE_TEST_CALL(test_body_creation())
        ARIADNE_TEST_CALL(test_bodysegment_creation())
        ARIADNE_TEST_CALL(test_bodysegment_update())
        ARIADNE_TEST_CALL(test_bodysegment_intersection())
        ARIADNE_TEST_CALL(test_human_state_instance())
        ARIADNE_TEST_CALL(test_robot_state_history())
    }

    void test_body_creation() {
        Human h(5, 10, {3,2,1,0}, {FloatType(0.5,Ariadne::dp),FloatType(1.0,Ariadne::dp)});

        ARIADNE_TEST_PRINT(h)
        ARIADNE_TEST_EQUALS(h.id(),5)
        ARIADNE_TEST_EQUALS(h.package_frequency(), 10)
        ARIADNE_TEST_EQUALS(h.num_segments(),2)

        ARIADNE_TEST_CONSTRUCT(Robot,r,(5, 10, {3,2,1,0}, {FloatType(0.5,Ariadne::dp),FloatType(1.0,Ariadne::dp)}))
    }

    void test_bodysegment_creation() {

        Robot r(5, 10, {3, 2, 1, 0}, {FloatType(1.0, Ariadne::dp), FloatType(0.5, Ariadne::dp)});
        auto segment = r.segment(1);

        ARIADNE_TEST_EQUALS(segment.id(),1)
        ARIADNE_TEST_EQUALS(segment.head_id(),1)
        ARIADNE_TEST_EQUALS(segment.tail_id(),0)
        ARIADNE_TEST_EQUALS(segment.thickness(),0.5)

        Point head(0,0.5,1.0);
        Point tail(1.0,2.0,-1.0);

        auto s1 = segment.create_sample();
        ARIADNE_TEST_ASSERT(s1.is_empty())

        auto s2 = segment.create_sample(head, tail);

        auto hp = s2.head_centre();
        auto tp = s2.tail_centre();
        auto bb = s2.bounding_box();

        ARIADNE_TEST_EQUALS(hp.x,0)
        ARIADNE_TEST_EQUALS(hp.y,0.5)
        ARIADNE_TEST_EQUALS(hp.z,1)
        ARIADNE_TEST_EQUALS(tp.x,1)
        ARIADNE_TEST_EQUALS(tp.y,2)
        ARIADNE_TEST_EQUALS(tp.z,-1)
        ARIADNE_TEST_EQUALS(bb.dimension(),3)
        ARIADNE_TEST_EQUALS(bb[0].lower_bound(),-0.5)
        ARIADNE_TEST_EQUALS(bb[0].upper_bound(),1.5)
        ARIADNE_TEST_EQUALS(bb[1].lower_bound(),0.0)
        ARIADNE_TEST_EQUALS(bb[1].upper_bound(),2.5)
        ARIADNE_TEST_EQUALS(bb[2].lower_bound(),-1.5)
        ARIADNE_TEST_EQUALS(bb[2].upper_bound(),1.5)
    }

    void test_bodysegment_update() {

        FloatType thickness(0.5,Ariadne::dp);

        Robot r(5, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto segment = r.segment(0);

        auto s1 = segment.create_sample();
        List<Point> heads;
        List<Point> tails;
        s1.update(heads,tails);
        ARIADNE_TEST_ASSERT(s1.is_empty())
        heads.push_back(Point(-0.5, 1.0, 1.25));
        s1.update(heads,tails);
        ARIADNE_TEST_ASSERT(s1.is_empty())
        tails.push_back(Point(1.0, 2.5, 0.0));
        s1.update(heads,tails);
        ARIADNE_TEST_ASSERT(not s1.is_empty())
        heads.clear();
        tails.clear();

        auto s2 = segment.create_sample();
        tails.push_back(Point(1.0, 2.5, 0.0));
        s2.update(heads,tails);
        ARIADNE_TEST_ASSERT(s2.is_empty())
        heads.push_back(Point(-0.5, 1.0, 1.25));
        s2.update(heads,tails);
        ARIADNE_TEST_ASSERT(not s2.is_empty())

        auto s3 = segment.create_sample();
        s3.update(Point(-0.5, 1.0, 1.25), Point(1.0, 2.5, 0.0));
        ARIADNE_TEST_ASSERT(not s3.is_empty())

        Point head(0,0.5,1.0);
        Point tail(1.0,2.0,-1.0);

        auto s4 = segment.create_sample(head, tail);

        s4.update(Point(-0.5, 1.0, 1.25), Point(1.0, 2.5, 0.0));

        ARIADNE_TEST_EQUALS(s4.head_centre().x, -0.25)
        ARIADNE_TEST_EQUALS(s4.head_centre().y, 0.75)
        ARIADNE_TEST_EQUALS(s4.head_centre().z, 1.125)
        ARIADNE_TEST_EQUALS(s4.tail_centre().x, 1.0)
        ARIADNE_TEST_EQUALS(s4.tail_centre().y, 2.25)
        ARIADNE_TEST_EQUALS(s4.tail_centre().z, -0.5)

        auto bb = s4.bounding_box();
        ARIADNE_TEST_EQUALS(bb[0].lower_bound(),-1.5)
        ARIADNE_TEST_EQUALS(bb[0].upper_bound(),2.0)
        ARIADNE_TEST_EQUALS(bb[1].lower_bound(),-0.5)
        ARIADNE_TEST_EQUALS(bb[1].upper_bound(),3.5)
        ARIADNE_TEST_EQUALS(bb[2].lower_bound(),-2.0)
        ARIADNE_TEST_EQUALS(bb[2].upper_bound(),2.25)
    }

    void test_bodysegment_intersection() {
        FloatType thickness(1.0,Ariadne::dp);
        Robot r(0, 10, {0, 1}, {thickness});
        auto segment = r.segment(0);

        auto s1 = segment.create_sample(Point(0, 0, 0), Point(5, 5, 5));
        auto s2 = segment.create_sample(Point(0, 3, 0), Point(5, 5, 5));
        auto s3 = segment.create_sample(Point(0, 3, 0), Point(5, 6, 5));
        auto s4 = segment.create_sample(Point(0, 3, 3), Point(0, 8, 8));
        auto s5 = segment.create_sample(Point(2.01, 3, 3), Point(2.01, 5, 5));
        auto s6 = segment.create_sample(Point(2, 3, 3), Point(2, 5, 5));
        auto s7 = segment.create_sample(Point(0, 8, 0), Point(0, 10, 0));

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

    void test_human_state_instance() {
        Human h(5, 10, {3,2,1,0}, {FloatType(0.5,Ariadne::dp),FloatType(1.0,Ariadne::dp)});
        auto instance = h.make_instance(HumanStatePackage({{Point(0,0,0)},{Point(4,4,4)},{Point(0,2,0)},{Point(1,0,3)}},5000u));

        ARIADNE_TEST_EQUALS(instance.samples().size(),2)
        ARIADNE_TEST_EQUALS(instance.timestamp(),5000)
    }

    void test_robot_state_history() {
        Ariadne::StringVariable robot("robot");
        Robot r(5, 1, {3, 2, 1, 0}, {FloatType(1.0, Ariadne::dp), FloatType(0.5, Ariadne::dp)});
        auto history = r.make_history();

        DiscreteLocation empty_location;
        ARIADNE_TEST_ASSERT(history.current_location().values().empty())
        ARIADNE_TEST_FAIL(history.entrances(empty_location))
        ARIADNE_TEST_FAIL(history.samples(empty_location))

        DiscreteLocation first(robot|"first"), second(robot|"second");

        history.acquire(RobotStatePackage(first,{{Point(0,0,0)},{Point(4,4,4)},{Point(0,2,0)},{Point(1,0,3)}},5000u));
        ARIADNE_TEST_FAIL(history.samples(first))
        ARIADNE_TEST_ASSERT(not history.has_samples(first))
        ARIADNE_TEST_EQUALS(history.current_location(),first)
        auto entrances = history.entrances(first);
        ARIADNE_TEST_EQUAL(entrances.size(),1)
        ARIADNE_TEST_ASSERT(entrances.back().source().values().empty())
        ARIADNE_TEST_EQUALS(entrances.back().timestamp(),5000)
        history.acquire(RobotStatePackage(first,{{Point(0,0,1)},{Point(4,4,5)},{Point(0,3,0)},{Point(1,1,3)}},6000u));
        ARIADNE_TEST_EQUALS(history.entrances(first).size(),1)

        history.acquire(RobotStatePackage(second,{{Point(0,0,1.5)},{Point(4,4,5.5)},{Point(0,3.5,0)},{Point(1,1.5,3)}},7000u));
        ARIADNE_TEST_EQUALS(history.current_location(),second)
        ARIADNE_TEST_ASSERT(not history.has_samples(second))
        ARIADNE_TEST_EQUALS(history.entrances(second).size(),1)
        ARIADNE_TEST_EQUALS(history.entrances(second).back().source(),first)
        ARIADNE_TEST_ASSERT(history.has_samples(first))
        auto samples = history.samples(first);
        ARIADNE_TEST_EQUALS(samples.size(),2)
        ARIADNE_TEST_PRINT(history.samples(first))
        ARIADNE_TEST_ASSERT(decide(history.samples(first).at(0).at(0).radius() == 0))

        history.acquire(RobotStatePackage(first,{{Point(0,0,2),Point(0,0.1,2)},{Point(4,4,6)},{Point(0,4,0)},{Point(1,2,3),Point(1.1,2,3)}},8000u));
        ARIADNE_TEST_ASSERT(history.has_samples(second))
        ARIADNE_TEST_EQUALS(history.entrances(first).size(),2)
        ARIADNE_TEST_EQUALS(history.entrances(first).back().source(),second)
        ARIADNE_TEST_EQUALS(history.samples(first).at(0).size(),2)
        ARIADNE_TEST_EQUALS(history.samples(second).at(0).size(),1)
        ARIADNE_TEST_PRINT(history.samples(second))
        history.acquire(RobotStatePackage(first,{{Point(1,0,2)},{Point(5,4,6)},{Point(1,4,0)},{Point(2,2,3)}},9000u));

        history.acquire(RobotStatePackage(second,{{Point(1,0,1.5)},{Point(5,4,5.5)},{Point(1,3.5,0)},{Point(2,1.5,3)}},10000u));
        ARIADNE_TEST_EQUALS(history.samples(first).at(0).size(),2)
        ARIADNE_TEST_EQUALS(history.entrances(second).size(),2)
        ARIADNE_TEST_PRINT(history.samples(first))
        ARIADNE_TEST_ASSERT(decide(history.samples(first).at(0).at(0).radius() > 0))
    }
};


int main() {
    TestBody().test();

    return ARIADNE_TEST_FAILURES;
}
