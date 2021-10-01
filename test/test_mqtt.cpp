/***************************************************************************
 *            test_mqtt.cpp
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
#include "test_broker_access.hpp"
#include "mqtt.hpp"

using namespace Opera;

class TestBrokerFailure {
  public:
    void test() {
        OPERA_TEST_CALL(test_failed_connection())
    }

    void test_failed_connection() {
        BrokerAccess access = MqttBrokerAccess("localhost",1900);
        OPERA_TEST_FAIL(access.make_body_presentation_publisher())
        OPERA_TEST_FAIL(access.make_body_presentation_subscriber([](auto){}))
    }
};

int main() {
    TestBrokerFailure().test();

    BrokerAccess access = MqttBrokerAccess("localhost",1883);
    TestBrokerAccess(access).test();
    return OPERA_TEST_FAILURES;
}
