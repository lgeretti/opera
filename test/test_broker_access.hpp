/***************************************************************************
 *            test_broker_access.hpp
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

#include <thread>
#include "utility.hpp"
#include "broker.hpp"

#include "test.hpp"

using namespace Opera;

class TestBrokerAccess {
  private:
    BrokerAccess const& _access;
  public:
    TestBrokerAccess(BrokerAccess const& access) : _access(access) { }

    void test() {
        OPERA_TEST_CALL(test_create_destroy())
        OPERA_TEST_CALL(test_single_transfer())
        OPERA_TEST_CALL(test_multiple_transfer())
    }

    void test_create_destroy() {
        BodyStatePacket p("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);

        OPERA_PRINT_TEST_COMMENT("Creating subscriber and removing it")
        auto* subscriber = _access.make_body_state_subscriber([](auto){});
        OPERA_TEST_EXECUTE(delete subscriber)

        OPERA_PRINT_TEST_COMMENT("Creating publisher and removing it immediately")
        auto* publisher1 = _access.make_body_state_publisher();
        OPERA_TEST_EXECUTE(delete publisher1)

        OPERA_PRINT_TEST_COMMENT("Creating publisher and removing it after publishing")
        auto* publisher2 = _access.make_body_state_publisher();
        publisher2->put(p);
        OPERA_TEST_EXECUTE(delete publisher2)
    }

    void test_single_transfer() {
        BodyPresentationPacket hp("human1", {{0, 1},{3, 2}}, {1.0,0.5});
        List<BodyPresentationPacket> bp_received;

        auto bp_subscriber = _access.make_body_presentation_subscriber([&](auto p){ bp_received.append(p); });
        auto bp_publisher = _access.make_body_presentation_publisher();
        bp_publisher->put(hp);

        SizeType i=0;
        while (bp_received.size() != 1) {
            ++i;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        OPERA_PRINT_TEST_COMMENT("Took " << i << " ms to acknowledge the reception")

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        delete bp_subscriber;
        delete bp_publisher;
    }

    void test_multiple_transfer() {
        BodyPresentationPacket hp("human1", {{0, 1},{3, 2}}, {1.0,0.5});
        BodyPresentationPacket rp("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {1.0,0.5, 0.5});
        BodyStatePacket hs("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290);
        BodyStatePacket rs("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);
        CollisionNotificationPacket cn("h0",0,"r0",3,DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301, 0.5);

        List<BodyPresentationPacket> bp_received;
        List<BodyStatePacket> bs_received;
        List<CollisionNotificationPacket> cn_received;

        auto bp_subscriber = _access.make_body_presentation_subscriber([&](auto p){ bp_received.append(p); });
        auto bs_subscriber = _access.make_body_state_subscriber([&](auto p){ bs_received.append(p); });
        auto cn_subscriber = _access.make_collision_notification_subscriber([&](auto p){ cn_received.append(p); });
        auto bp_publisher = _access.make_body_presentation_publisher();
        auto bs_publisher = _access.make_body_state_publisher();
        auto cn_publisher = _access.make_collision_notification_publisher();
        bp_publisher->put(hp);
        bp_publisher->put(rp);
        bs_publisher->put(hs);
        bs_publisher->put(rs);
        cn_publisher->put(cn);
        SizeType i=0;
        while (bp_received.size() != 2 or bs_received.size() != 2 or cn_received.size() != 1) {
            ++i;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        OPERA_PRINT_TEST_COMMENT("Took " << i << " ms to acknowledge the reception")

        delete bp_subscriber;
        delete bs_subscriber;
        delete cn_subscriber;
        delete bp_publisher;
        delete bs_publisher;
        delete cn_publisher;
    }
};
