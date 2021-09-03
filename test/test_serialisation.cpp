/***************************************************************************
 *            test_serialisation.cpp
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
#include "utility.hpp"
#include "serialisation.hpp"

using namespace Opera;

class TestSerialisation {
public:
    void test() {
        ARIADNE_TEST_CALL(test_bodypresentationpacket_human())
        ARIADNE_TEST_CALL(test_bodypresentationpacket_robot())
        ARIADNE_TEST_CALL(test_bodystatepacket_human())
        ARIADNE_TEST_CALL(test_bodystatepacket_robot())
        ARIADNE_TEST_CALL(test_collisionnotificationpacket())
    }

    void test_bodypresentationpacket_human() {
        BodyPresentationPacket p("human1", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)});
        BodyPresentationPacketSerialiser serialiser(p);
        serialiser.to_file(Resources::path("json/examples/presentation/" + p.id() + ".tmp.json"));
        ARIADNE_TEST_EQUALS(serialiser.to_string(),"{\"id\":\"human1\",\"isHuman\":true,\"pointIds\":[[0,1],[3,2]],\"thicknesses\":[1.0,0.5]}")
    }

    void test_bodypresentationpacket_robot() {
        BodyPresentationPacket p("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)});
        BodyPresentationPacketSerialiser serialiser(p);
        serialiser.to_file(Resources::path("json/examples/presentation/" + p.id() + ".tmp.json"));
        ARIADNE_TEST_EQUALS(serialiser.to_string(),"{\"id\":\"robot1\",\"isHuman\":false,\"pointIds\":[[0,1],[3,2],[4,2]],\"thicknesses\":[1.0,0.5,0.5]}")
    }

    void test_bodystatepacket_human() {
        BodyStatePacket p("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290);
        BodyStatePacketSerialiser serialiser(p);
        serialiser.to_file(Resources::path("json/examples/state/" + p.id() + ".tmp.json"));
        ARIADNE_TEST_EQUALS(serialiser.to_string(),"{\"bodyId\":\"human0\",\"continuousState\":[[[0.4,2.1,0.2]],[[0.0,-1.0,0.1],[0.3,3.1,-1.2]],[[0.4,0.1,1.2]],[[0.0,0.0,1.0]]],\"timestamp\":3423235253290}")
    }

    void test_bodystatepacket_robot() {
        BodyStatePacket p("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);
        BodyStatePacketSerialiser serialiser(p);
        serialiser.to_file(Resources::path("json/examples/state/" + p.id() + ".tmp.json"));
        ARIADNE_TEST_EQUALS(serialiser.to_string(),"{\"bodyId\":\"robot0\",\"discreteState\":{\"destination\":\"2\",\"origin\":\"3\",\"phase\":\"pre\"},\"continuousState\":[[],[[0.0,-1.0,0.1],[0.3,3.1,-1.2]],[]],\"timestamp\":93249042230}")
    }

    void test_collisionnotificationpacket() {
        CollisionNotificationPacket p("h0",0,"r0",3,DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301);
        CollisionNotificationPacketSerialiser serialiser(p);
        serialiser.to_file(Resources::path("json/examples/notification/notification0.tmp.json"));
        ARIADNE_TEST_EQUALS(serialiser.to_string(),"{\"human\":{\"bodyId\":\"h0\",\"segmentId\":0},\"robot\":{\"bodyId\":\"r0\",\"segmentId\":3},\"discreteState\":{\"destination\":\"2\",\"origin\":\"3\",\"phase\":\"pre\"},\"collisionTime\":{\"lower\":328903284232,\"upper\":328905923301}}")
    }
};


int main() {
    TestSerialisation().test();

    return ARIADNE_TEST_FAILURES;
}
