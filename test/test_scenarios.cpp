/***************************************************************************
 *            test_scenarios.cpp
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

#include <thread>
#include <mutex>
#include <condition_variable>

#include "utility.hpp"
#include "packet.hpp"
#include "deserialisation.hpp"
#include "barrier.hpp"

using namespace Opera;

class TestScenarios {
  public:
    void test() {
        //ARIADNE_TEST_CALL(test_acquire_human_nocollision_samples())
        //ARIADNE_TEST_CALL(test_check_robot_traces())
        ARIADNE_TEST_CALL(test_acquire_robot_10Hz_samples())
    }

    void test_acquire_human_nocollision_samples() {
        BodyPresentationPacket p0 = BodyPresentationPacketDeserialiser(Resources::path("json/scenarios/human/presentation.json")).make();
        Human human(p0.id(),p0.point_ids(),p0.thicknesses());
        ARIADNE_TEST_EQUALS(human.num_points(),18)

        List<BodyStatePacket> human_packets;
        SizeType idx = 1;
        while (true) {
            auto filepath = Resources::path("json/scenarios/human/nocollision/"+std::to_string(idx++)+".json");
            if (not exists(filepath)) break;
            human_packets.append(BodyStatePacketDeserialiser(filepath).make());
        }

        List<HumanStateInstance> instances;
        SizeType i=0;
        for (auto pkt : human_packets) {
            instances.append(HumanStateInstance(human,pkt.points(),pkt.timestamp()));
        }
    }

    void test_check_robot_traces() {
        BodyPresentationPacket p0 = BodyPresentationPacketDeserialiser(Resources::path("json/scenarios/robot/10hz/presentation.json")).make();
        Robot robot(p0.id(),p0.packet_frequency(),p0.point_ids(),p0.thicknesses());
        ARIADNE_TEST_EQUALS(robot.num_points(),9)

        List<RobotDiscreteTrace> traces;
        TimestampType current_timestamp = 0;
        for (SizeType folder=1; folder<=10; ++folder) {
            SizeType file = 1;
            RobotStateHistory history(&robot);
            while (true) {
                auto filepath = Resources::path("json/scenarios/robot/10hz/"+std::to_string(folder)+"/"+std::to_string(file++)+".json");
                if (not exists(filepath)) break;
                auto pkt = BodyStatePacketDeserialiser(filepath).make();
                ARIADNE_ASSERT(pkt.timestamp() > current_timestamp)
                current_timestamp = pkt.timestamp();
                history.acquire(pkt.location(),pkt.points(),pkt.timestamp());
            }
            auto trace = history.discrete_trace();
            traces.append(trace);
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

    void test_acquire_robot_10Hz_samples() {

        int speedup = 100;

        BodyPresentationPacket p0 = BodyPresentationPacketDeserialiser(Resources::path("json/scenarios/robot/10hz/presentation.json")).make();
        Robot robot(p0.id(),p0.packet_frequency(),p0.point_ids(),p0.thicknesses());
        RobotStateHistory history(&robot);

        for (SizeType folder=1; folder<=9; ++folder) {
            SizeType file = 1;
            while (true) {
                auto filepath = Resources::path("json/scenarios/robot/10hz/"+std::to_string(folder)+"/"+std::to_string(file++)+".json");
                if (not exists(filepath)) break;
                auto pkt = BodyStatePacketDeserialiser(filepath).make();
                history.acquire(pkt.location(),pkt.points(),pkt.timestamp());
            }
        }

        List<BodyStatePacket> human_packets;
        SizeType idx = 1;
        while (true) {
            auto filepath = Resources::path("json/scenarios/human/nocollision/"+std::to_string(idx++)+".json");
            if (not exists(filepath)) break;
            human_packets.append(BodyStatePacketDeserialiser(filepath).make());
        }
        List<BodyStatePacket> robot_packets;
        idx = 1;
        while (true) {
            auto filepath = Resources::path("json/scenarios/robot/10hz/10/"+std::to_string(idx++)+".json");
            if (not exists(filepath)) break;
            robot_packets.append(BodyStatePacketDeserialiser(filepath).make());
        }

        std::thread human_consumption([&]{
            std::this_thread::sleep_for(std::chrono::seconds(1));
            for (auto p : human_packets) {
                std::cout << "Human packet received at " << p.timestamp() << std::endl;

                std::this_thread::sleep_for(std::chrono::microseconds(66667/speedup));
            }
        });

        std::thread robot_consumption([&]{
            std::this_thread::sleep_for(std::chrono::seconds(1));
            for (auto p : robot_packets) {
                std::cout << "Robot packet received at " << p.timestamp() << std::endl;
                history.acquire(p.location(),p.points(),p.timestamp());
                std::this_thread::sleep_for(std::chrono::microseconds(100000/speedup));
            }
        });

        human_consumption.join();
        robot_consumption.join();
    }

};

int main() {
    TestScenarios().test();
    return ARIADNE_TEST_FAILURES;
}
