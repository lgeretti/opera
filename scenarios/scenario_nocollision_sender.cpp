/***************************************************************************
 *            scenario_nocollision.cpp
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

#include "thread.hpp"
#include "utility.hpp"
#include "packet.hpp"
#include "deserialisation.hpp"
#include "mqtt.hpp"
#include "memory.hpp"
#include "barrier.hpp"
#include "logging.hpp"
#include "command_line_interface.hpp"

using namespace Opera;

class NoCollisionScenario {
  public:
    void run() {
        acquire_robot_10Hz_samples();
    }

    void acquire_robot_10Hz_samples() {

        int speedup = 10;

        BrokerAccess access = MqttBrokerAccess("localhost",1883);

        Thread robot_production([&]{
            auto* publisher = access.make_body_state_publisher();
            SizeType i=0;
            while (true) {
                BodyStatePacket p("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);
                OPERA_LOG_PRINTLN("Robot packet " << i++ << " sent");
                publisher->put(p);
                std::this_thread::sleep_for(std::chrono::microseconds(1000000/speedup));
            }
            delete publisher;
        },"rb_p");
    }

};

int main(int argc, const char* argv[])
{
    if (not CommandLineInterface::instance().acquire(argc,argv)) return -1;
    NoCollisionScenario().run();
}
