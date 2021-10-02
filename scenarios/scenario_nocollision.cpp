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
        //acquire_human_nocollision_samples();
        //check_robot_traces();
        acquire_robot_10Hz_samples();
    }

    void acquire_human_nocollision_samples() {
        BodyPresentationPacket p0 = Deserialiser<BodyPresentationPacket>(Resources::path("json/scenarios/human/presentation.json")).make();
        Human human(p0.id(),p0.point_ids(),p0.thicknesses());
        OPERA_ASSERT_EQUAL(human.num_points(),18)

        List<BodyStatePacket> human_packets;
        SizeType idx = 1;
        while (true) {
            auto filepath = Resources::path("json/scenarios/human/nocollision/"+std::to_string(idx++)+".json");
            if (not exists(filepath)) break;
            human_packets.push_back(Deserialiser<BodyStatePacket>(filepath).make());
        }

        List<HumanStateInstance> instances;
        for (auto pkt : human_packets) {
            instances.push_back(HumanStateInstance(human,pkt.points(),pkt.timestamp()));
        }
    }

    void check_robot_traces() {
        BodyPresentationPacket p0 = Deserialiser<BodyPresentationPacket>(Resources::path("json/scenarios/robot/10hz/presentation.json")).make();
        Robot robot(p0.id(),p0.packet_frequency(),p0.point_ids(),p0.thicknesses());
        OPERA_ASSERT_EQUAL(robot.num_points(),9)

        List<RobotDiscreteTrace> traces;
        TimestampType current_timestamp = 0;
        for (SizeType folder=1; folder<=10; ++folder) {
            SizeType file = 1;
            RobotStateHistory history(&robot);
            while (true) {
                auto filepath = Resources::path("json/scenarios/robot/10hz/"+std::to_string(folder)+"/"+std::to_string(file++)+".json");
                if (not exists(filepath)) break;
                auto pkt = Deserialiser<BodyStatePacket>(filepath).make();
                OPERA_ASSERT(pkt.timestamp() > current_timestamp)
                current_timestamp = pkt.timestamp();
                history.acquire(pkt.location(),pkt.points(),pkt.timestamp());
            }
            auto trace = history.discrete_trace();
            traces.push_back(trace);
            std::ofstream txt;
            txt.open((std::to_string(folder)+".txt").c_str(), std::ios::out | std::ios::binary);
            txt << trace;
            txt.close();
        }

        for (SizeType i=0; i<9; ++i) {
            for (SizeType j=i+1; j<=9; ++j) {
                if (not (traces.at(i) == traces.at(j))) std::cout << "Traces " << i+1 << " and " << j+1 << " differ." << std::endl;
            }
        }
    }

    void acquire_robot_10Hz_samples() {

        int speedup = 100;

        BodyPresentationPacket p0 = Deserialiser<BodyPresentationPacket>(Resources::path("json/scenarios/robot/10hz/presentation.json")).make();
        Robot robot(p0.id(),p0.packet_frequency(),p0.point_ids(),p0.thicknesses());
        RobotStateHistory history(&robot);

        for (SizeType folder=1; folder<=1; ++folder) {
            SizeType file = 1;
            while (true) {
                auto filepath = Resources::path("json/scenarios/robot/10hz/"+std::to_string(folder)+"/"+std::to_string(file++)+".json");
                if (not exists(filepath)) break;
                auto pkt = Deserialiser<BodyStatePacket>(filepath).make();
                history.acquire(pkt.location(),pkt.points(),pkt.timestamp());
            }
        }

        std::deque<BodyStatePacket> human_packets;
        SizeType idx = 1;
        while (true) {
            auto filepath = Resources::path("json/scenarios/human/nocollision/"+std::to_string(idx++)+".json");
            if (not exists(filepath)) break;
            human_packets.push_back(Deserialiser<BodyStatePacket>(filepath).make());
        }

        std::deque<BodyStatePacket> robot_packets;
        idx = 1;
        while (true) {
            auto filepath = Resources::path("json/scenarios/robot/10hz/10/"+std::to_string(idx++)+".json");
            if (not exists(filepath)) break;
            robot_packets.push_back(Deserialiser<BodyStatePacket>(filepath).make());
        }

        BrokerAccess access = MqttBrokerAccess("localhost",1883);

        Thread human_production([&]{
            auto* publisher = access.make_body_state_publisher();
            while (not human_packets.empty()) {
                auto& p = human_packets.front();
                OPERA_LOG_PRINTLN("Human packet sent at " << p.timestamp())
                publisher->put(p);
                human_packets.pop_front();
                std::this_thread::sleep_for(std::chrono::microseconds(66667/speedup));
            }
            delete publisher;
        },"hu_p");

        Thread robot_production([&]{
            auto* publisher = access.make_body_state_publisher();
            while (not robot_packets.empty()) {
                BodyStatePacket p = robot_packets.front();
                OPERA_LOG_PRINTLN("Robot packet sent at " << p.timestamp())
                publisher->put(p);
                robot_packets.pop_front();
                std::this_thread::sleep_for(std::chrono::microseconds(100000/speedup));
            }
            delete publisher;
        },"rb_p");

        auto* subscriber = access.make_body_state_subscriber([](auto p){ OPERA_LOG_PRINTLN("State packet received at " << p.timestamp()) });

        while(not robot_packets.empty())
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        OPERA_LOG_PRINTLN("All done")

        delete subscriber;
    }

};

int main(int argc, const char* argv[])
{
    if (not CommandLineInterface::instance().acquire(argc,argv)) return -1;
    NoCollisionScenario().run();
}
