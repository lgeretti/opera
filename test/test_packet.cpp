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
        ARIADNE_TEST_CALL(test_human_packet_create())
        ARIADNE_TEST_CALL(test_robot_packet_create())
    }

    void test_human_packet_create() {
        HumanStatePacket h("h0",{{Point(0,0,0)},{Point(0,2,0)}},1e8);
        ARIADNE_TEST_EQUALS(h.id(),"h0")
        ARIADNE_TEST_EQUALS(h.points().size(),2)
        ARIADNE_TEST_EQUALS(h.timestamp(),1e8)
    }

    void test_robot_packet_create() {
        DiscreteLocation loc(StringVariable("r0")|"first");
        RobotStatePacket r("r0",loc,{{Point(0,0,0)},{Point(0,2,0)},{Point(0,4,0)}},2e8);
        ARIADNE_TEST_EQUALS(r.id(),"r0")
        ARIADNE_TEST_EQUALS(r.location(),loc)
        ARIADNE_TEST_EQUALS(r.points().size(),3)
        ARIADNE_TEST_EQUALS(r.timestamp(),2e8)
    }
};


int main() {
    TestPacket().test();

    return ARIADNE_TEST_FAILURES;
}
