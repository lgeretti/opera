/***************************************************************************
 *            test_serialisation.cpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of Opera, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "test.hpp"
#include "utility.hpp"
#include "serialisation.hpp"

using namespace Opera;

class TestSerialisation {
public:
    void test() {
        OPERA_TEST_CALL(test_bodypresentationpacket_human())
        OPERA_TEST_CALL(test_bodypresentationpacket_robot())
        OPERA_TEST_CALL(test_bodystatepacket_human())
        OPERA_TEST_CALL(test_bodystatepacket_robot())
        OPERA_TEST_CALL(test_collisionnotificationpacket())
    }

    void test_bodypresentationpacket_human() {
        BodyPresentationPacket p("human1", {{0, 1},{3, 2}}, {1.0,0.5});
        Serialiser<BodyPresentationPacket> serialiser(p);
        serialiser.to_file(Resources::path("json/examples/presentation/" + p.id() + ".tmp.json"));
        OPERA_TEST_EQUALS(serialiser.to_string(),"{\"id\":\"human1\",\"isHuman\":true,\"pointIds\":[[0,1],[3,2]],\"thicknesses\":[1.0,0.5]}")
    }

    void test_bodypresentationpacket_robot() {
        BodyPresentationPacket p("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {1.0,0.5, 0.5});
        Serialiser<BodyPresentationPacket> serialiser(p);
        serialiser.to_file(Resources::path("json/examples/presentation/" + p.id() + ".tmp.json"));
        OPERA_TEST_EQUALS(serialiser.to_string(),"{\"id\":\"robot1\",\"isHuman\":false,\"packetFrequency\":30,\"pointIds\":[[0,1],[3,2],[4,2]],\"thicknesses\":[1.0,0.5,0.5]}")
    }

    void test_bodystatepacket_human() {
        BodyStatePacket p("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290);
        Serialiser<BodyStatePacket> serialiser(p);
        serialiser.to_file(Resources::path("json/examples/state/" + p.id() + ".tmp.json"));
        OPERA_TEST_EQUALS(serialiser.to_string(),"{\"bodyId\":\"human0\",\"continuousState\":[[[0.4,2.1,0.2]],[[0.0,-1.0,0.1],[0.3,3.1,-1.2]],[[0.4,0.1,1.2]],[[0.0,0.0,1.0]]],\"timestamp\":3423235253290}")
    }

    void test_bodystatepacket_robot() {
        BodyStatePacket p("robot0",DiscreteState({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);
        Serialiser<BodyStatePacket> serialiser(p);
        serialiser.to_file(Resources::path("json/examples/state/" + p.id() + ".tmp.json"));
        OPERA_TEST_EQUALS(serialiser.to_string(),"{\"bodyId\":\"robot0\",\"discreteState\":{\"destination\":\"2\",\"origin\":\"3\",\"phase\":\"pre\"},\"continuousState\":[[],[[0.0,-1.0,0.1],[0.3,3.1,-1.2]],[]],\"timestamp\":93249042230}")
    }

    void test_collisionnotificationpacket() {
        CollisionNotificationPacket p("h0",0,"r0",3,DiscreteState({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301, 0.5);
        Serialiser<CollisionNotificationPacket> serialiser(p);
        serialiser.to_file(Resources::path("json/examples/notification/notification0.tmp.json"));
        OPERA_TEST_EQUALS(serialiser.to_string(),"{\"human\":{\"bodyId\":\"h0\",\"segmentId\":0},\"robot\":{\"bodyId\":\"r0\",\"segmentId\":3},\"discreteState\":{\"destination\":\"2\",\"origin\":\"3\",\"phase\":\"pre\"},\"collisionTime\":{\"lower\":328903284232,\"upper\":328905923301},\"likelihood\":0.5}")
    }
};


int main() {
    TestSerialisation().test();

    return OPERA_TEST_FAILURES;
}
