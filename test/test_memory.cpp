/***************************************************************************
 *            test_memory.cpp
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
#include "memory.hpp"
#include "mqtt.hpp"

using namespace Opera;

class TestMemory {
public:
    void test() {
        ARIADNE_TEST_CALL(test_broker())
    }

    void test_broker() {
        BodyPresentationPacket hp("human1", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)});
        BodyPresentationPacket rp("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)});
        BodyStatePacket hs("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290);
        BodyStatePacket rs("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);
        CollisionNotificationPacket cn("h0",0,"r0",3,DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301, cast_positive(FloatType(0.5,dp)));

        BrokerAccess access = MqttBrokerAccess("localhost",1883);

        List<BodyPresentationPacket> bp_received;
        List<BodyStatePacket> bs_received;
        List<CollisionNotificationPacket> cn_received;

        auto bp_subscriber = access.body_presentation_subscriber();
        auto bs_subscriber = access.body_state_subscriber();
        auto cn_subscriber = access.collision_notification_subscriber();
        bp_subscriber->loop_get([&](auto p){ bp_received.push_back(p); });
        bs_subscriber->loop_get([&](auto p){ bs_received.push_back(p); });
        cn_subscriber->loop_get([&](auto p){ cn_received.push_back(cn); });

        auto bp_publisher = access.body_presentation_publisher();
        auto bs_publisher = access.body_state_publisher();
        auto cn_publisher = access.collision_notification_publisher();

        bp_publisher->put(hp);
        bp_publisher->put(rp);
        bs_publisher->put(hs);
        bs_publisher->put(rs);
        cn_publisher->put(cn);

        delete bp_publisher;
        delete bs_publisher;
        delete cn_publisher;

        SizeType i=0;
        while (bp_received.size() != 2 or bs_received.size() != 2 or cn_received.size() != 1) {
            ++i;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        ARIADNE_PRINT_TEST_COMMENT("Took " << i << " ms to acknowledge the reception")

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        delete bp_subscriber;
        delete bs_subscriber;
        delete cn_subscriber;
    }
};


int main() {
    TestMemory().test();
    return ARIADNE_TEST_FAILURES;
}
