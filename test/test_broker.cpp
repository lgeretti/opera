/***************************************************************************
 *            test_broker.cpp
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

using namespace Opera;

class TestBroker {
public:
    void test() {
        ARIADNE_TEST_CALL(test_broker_manager_population())
        ARIADNE_TEST_CALL(test_broker_manager_with_memory())
    }

    void test_broker_manager_population() {
        BrokerManager manager;

        ARIADNE_TEST_EQUALS(manager.num_brokers(),0)
        ARIADNE_TEST_ASSERT(not manager.has_broker(BrokerKind::MEMORY))

        manager.add(MemoryBroker());
        ARIADNE_TEST_EQUALS(manager.num_brokers(),1)
        ARIADNE_TEST_ASSERT(manager.has_broker(BrokerKind::MEMORY))

        manager.clear();
        ARIADNE_TEST_EQUALS(manager.num_brokers(),0)
        ARIADNE_TEST_ASSERT(not manager.has_broker(BrokerKind::MEMORY))
    }

    void test_broker_manager_with_memory() {
        BodyPresentationPacket hp("human1", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)});
        BodyPresentationPacket rp("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)});
        BodyStatePacket hs("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290);
        BodyStatePacket rs("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);
        CollisionNotificationPacket cn("h0",0,"r0",3,DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301, cast_positive(FloatType(0.5,dp)));

        BrokerManager sender_manager;
        sender_manager.add(MemoryBroker());

        std::deque<BodyPresentationPacket> bp_received;
        std::deque<BodyStatePacket> bs_received;
        std::deque<CollisionNotificationPacket> cn_received;

        bool stop = false;
        std::thread cpt([&]{
            BrokerManager receiver_manager;
            receiver_manager.add(MemoryBroker());
            while(not stop) {
                receiver_manager.receive(bp_received);
                receiver_manager.receive(bs_received);
                receiver_manager.receive(cn_received);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });

        sender_manager.send(hp);
        sender_manager.send(rp);
        sender_manager.send(hs);
        sender_manager.send(rs);
        sender_manager.send(cn);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        ARIADNE_TEST_EQUAL(bp_received.size(),2)
        ARIADNE_TEST_EQUAL(bs_received.size(),2)
        ARIADNE_TEST_EQUAL(cn_received.size(),1)

        stop = true;
        cpt.join();
    }
};


int main() {
    TestBroker().test();

    return ARIADNE_TEST_FAILURES;
}
