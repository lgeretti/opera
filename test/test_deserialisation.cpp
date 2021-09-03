/***************************************************************************
 *            test_deserialisation.cpp
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
#include "deserialisation.hpp"

using namespace Opera;

class TestDeserialisation {
public:
    void test() {
        ARIADNE_TEST_CALL(test_bodypresentationpacket_make_human())
        ARIADNE_TEST_CALL(test_bodypresentationpacket_make_robot())
        ARIADNE_TEST_CALL(test_bodystatepacket_make())
        ARIADNE_TEST_CALL(test_collisiondetectionpacket_make())
    }

    void test_bodypresentationpacket_make_human() {
        BodyPresentationPacketDeserialiser d1(Resources::path("json/examples/presentation/human0.json"));
        auto p1 = d1.make();
        ARIADNE_TEST_ASSERT(p1.is_human())
        BodyPresentationPacketDeserialiser d2("{\n"
                                   "  \"id\": \"h0\",\n"
                                   "  \"isHuman\": true,\n"
                                   "  \"pointIds\": [[14,12],[11,12],[5,7],[6,8],[7,9],[8,10],[1,2],[0,1],[0,2],[1,3],[2,4],[3,5],[4,6],[17,0],[17,5],[17,6],[17,11],[17,12]],\n"
                                   "  \"thicknesses\": [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]\n"
                                   "}");
        auto p2 = d2.make();
        ARIADNE_TEST_EQUALS(p1.id(),p2.id())
    }
    void test_bodypresentationpacket_make_robot() {
        BodyPresentationPacketDeserialiser d1(Resources::path("json/examples/presentation/robot0.json"));
        auto p1 = d1.make();
        BodyPresentationPacketDeserialiser d2("{\n"
                            "  \"id\": \"r0\",\n"
                            "  \"isHuman\": false,\n"
                            "  \"packetFrequency\": 10,\n"
                            "  \"pointIds\": [[0,1],[1,2],[2,3],[3,4],[4,5],[5,6],[6,7]],\n"
                            "  \"thicknesses\": [1,1,1,1,1,1,1]\n"
                            "}");
        auto p2 = d2.make();
        ARIADNE_TEST_EQUALS(p1.id(),p2.id())
    }

    void test_bodystatepacket_make() {
        DiscreteLocation loc({{"origin","3"},{"destination","2"},{"phase","pre"}});
        BodyStatePacketDeserialiser d(Resources::path("json/examples/state/robot0.json"));
        auto p = d.make();
        ARIADNE_TEST_EQUALS(p.id(),"r0")
        ARIADNE_TEST_EQUALS(p.location(),loc)
        ARIADNE_TEST_PRINT(p.points())
        ARIADNE_TEST_EQUALS(p.points().size(),8)
        ARIADNE_TEST_EQUALS(p.points().at(0).size(),1)
        ARIADNE_TEST_EQUALS(p.points().at(1).size(),1)
        ARIADNE_TEST_EQUALS(p.points().at(2).size(),2)
        ARIADNE_TEST_EQUALS(p.points().at(3).size(),1)
        ARIADNE_TEST_EQUALS(p.points().at(4).size(),1)
        ARIADNE_TEST_EQUALS(p.points().at(5).size(),1)
        ARIADNE_TEST_EQUALS(p.points().at(6).size(),1)
        ARIADNE_TEST_EQUALS(p.points().at(7).size(),1)
        ARIADNE_TEST_EQUALS(p.timestamp(),328903284232)
    }

    void test_collisiondetectionpacket_make() {
        DiscreteLocation loc({{"origin","3"},{"destination","2"},{"phase","pre"}});
        CollisionNotificationPacketDeserialiser d(Resources::path("json/examples/notification/notification0.json"));
        auto p = d.make();
        ARIADNE_TEST_EQUALS(p.human_id(),"h0")
        ARIADNE_TEST_EQUALS(p.human_segment_id(),0)
        ARIADNE_TEST_EQUALS(p.robot_id(),"r0")
        ARIADNE_TEST_EQUALS(p.robot_segment_id(),3)
        ARIADNE_TEST_EQUALS(p.discrete_state(),loc)
        ARIADNE_TEST_EQUALS(p.lower_collision_time(),328903284232)
        ARIADNE_TEST_EQUALS(p.upper_collision_time(),328905923301)
    }
};

int main() {
    TestDeserialisation().test();
    return ARIADNE_TEST_FAILURES;
}
