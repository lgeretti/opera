/***************************************************************************
 *            test_packet.cpp
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

#include "packet.hpp"

using namespace Opera;

class TestPacket {
public:
    void test() {
        ARIADNE_TEST_CALL(test_human_presentation_packet_create())
        ARIADNE_TEST_CALL(test_robot_presentation_packet_create())
        ARIADNE_TEST_CALL(test_human_state_packet_create())
        ARIADNE_TEST_CALL(test_robot_state_packet_create())
        ARIADNE_TEST_CALL(test_notification_packet_create())
    }

    void test_human_presentation_packet_create() {
        BodyPresentationPacket p("h0",{{0,1},{1,2}},{FloatType(1.0,dp),FloatType(0.5,dp)});
        ARIADNE_TEST_EQUALS(p.id(),"h0")
        ARIADNE_TEST_ASSERT(p.is_human())
        ARIADNE_TEST_EQUALS(p.point_ids().size(),2)
        ARIADNE_TEST_EQUALS(p.thicknesses().size(),2)
    }

    void test_robot_presentation_packet_create() {
        BodyPresentationPacket p("r0",10,{{0,1},{1,2}},{FloatType(1.0,dp),FloatType(0.5,dp)});
        ARIADNE_TEST_EQUALS(p.id(),"r0")
        ARIADNE_TEST_ASSERT(not p.is_human())
        ARIADNE_TEST_EQUALS(p.packet_frequency(),10)
        ARIADNE_TEST_EQUALS(p.point_ids().size(),2)
        ARIADNE_TEST_EQUALS(p.thicknesses().size(),2)
    }

    void test_human_state_packet_create() {
        BodyStatePacket p("h0",{{Point(0,0,0)},{Point(0,2,0)}},3e8);
        ARIADNE_TEST_EQUALS(p.id(),"h0")
        ARIADNE_TEST_ASSERT(p.location().values().empty())
        ARIADNE_TEST_EQUALS(p.points().size(),2)
        ARIADNE_TEST_EQUALS(p.timestamp(),3e8)
    }

    void test_robot_state_packet_create() {
        DiscreteLocation loc(StringVariable("r0")|"first");
        BodyStatePacket p("r0",loc,{{Point(0,0,0)},{Point(0,2,0)},{Point(0,4,0)}},2e8);
        ARIADNE_TEST_EQUALS(p.id(),"r0")
        ARIADNE_TEST_EQUALS(p.location(),loc)
        ARIADNE_TEST_EQUALS(p.points().size(),3)
        ARIADNE_TEST_EQUALS(p.timestamp(),2e8)
    }

    void test_notification_packet_create() {
        DiscreteLocation loc(StringVariable("r0")|"first");
        CollisionNotificationPacket p("h0",1,"r0",4,loc,2e8,3e8);

        ARIADNE_TEST_EQUALS(p.human_id(),"h0")
        ARIADNE_TEST_EQUALS(p.human_segment_id(),1)
        ARIADNE_TEST_EQUALS(p.robot_id(),"r0")
        ARIADNE_TEST_EQUALS(p.robot_segment_id(),4)
        ARIADNE_TEST_EQUALS(p.discrete_state(),loc)
        ARIADNE_TEST_EQUALS(p.lower_collision_time(),2e8)
        ARIADNE_TEST_EQUALS(p.upper_collision_time(),3e8)
    }
};


int main() {
    TestPacket().test();

    return ARIADNE_TEST_FAILURES;
}
