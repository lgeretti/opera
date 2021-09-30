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

        int speedup = 100;

        BrokerAccess access = MqttBrokerAccess("localhost",1883);
        //BrokerAccess access = MemoryBrokerAccess();

        auto* subscriber = access.make_body_state_subscriber([](auto p){ std::cout << "received" << std::endl; });

        std::this_thread::sleep_for(std::chrono::milliseconds(10000));

        std::cout << "done" << std::endl;

        delete subscriber;
    }

};

int main(int argc, const char* argv[])
{
    if (not CommandLineInterface::instance().acquire(argc,argv)) return -1;
    //Logger::instance().use_blocking_scheduler();
    NoCollisionScenario().run();
}
