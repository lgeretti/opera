/***************************************************************************
 *            test_kafka.cpp
 *
 *  Copyright  2021  Andrea Gagliardo, Sandro Ferrari, Luca Geretti
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
#include "kafka.hpp"
#include "serialisation.hpp"
#include "deserialisation.hpp"
#include "packet.hpp"
#include<unistd.h>
#include<sys/wait.h>

using namespace Opera;

class TestKafka{
public: 
    void test(){
        ARIADNE_TEST_CALL(test_presentation())
        ARIADNE_TEST_CALL(test_state())
        ARIADNE_TEST_CALL(test_notification())
    }

    void test_presentation(){
        ConsumerPresentation consumer_pres(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");

        List<BodyPresentationPacket> ps;
        ps.append(BodyPresentationPacket("human1", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)}));
        ps.append(BodyPresentationPacket("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)}));

        std::thread cpt([&]{ consumer_pres.check_new_message();} );

        send_presentation(ps.at(0), producer);
        send_presentation(ps.at(1), producer);

        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        ARIADNE_TEST_EQUAL(consumer_pres.number_new_msgs(),2)
            
        for (auto p : ps) {
            BodyPresentationPacket p_received = consumer_pres.get_pkt();
            ARIADNE_TEST_EQUAL(p_received.id(), p.id())
            ARIADNE_TEST_EQUAL(p_received.is_human(), p.is_human())
            ARIADNE_TEST_EQUAL(p_received.packet_frequency(), p.packet_frequency())
            for(int i = 0; i<p.point_ids().size(); i++){
                ARIADNE_TEST_EQUAL(p_received.point_ids()[i].first, p.point_ids()[i].first)
                ARIADNE_TEST_EQUAL(p_received.point_ids()[i].second, p.point_ids()[i].second)
            }
            for(int i = 0; i<p.thicknesses().size(); i++){
                ARIADNE_TEST_EQUAL(p_received.thicknesses()[i], p.thicknesses()[i])
            }
        }

        consumer_pres.set_run(false);
        cpt.join();
        delete producer;
    }

    void test_state(){

        ConsumerState consumer_st(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");

        List<BodyStatePacket> ps;
        ps.append(BodyStatePacket("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290));
        ps.append(BodyStatePacket("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230));

        std::thread cpt([&]{consumer_st.check_new_message();} );

        send_state(ps.at(0), producer);
        send_state(ps.at(1), producer);

        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        ARIADNE_TEST_EQUAL(consumer_st.number_new_msgs(),2)

        for (auto p : ps) {
            BodyStatePacket p_received = consumer_st.get_pkt();
            consumer_st.set_run(false);
            ARIADNE_TEST_EQUAL(p_received.id(), p.id())
            ARIADNE_TEST_EQUAL(p_received.location(), p.location())
            ARIADNE_TEST_EQUAL(p_received.timestamp(), p.timestamp())
            for(int i = 0; i<p.points().size(); i++){
                ARIADNE_TEST_EQUAL(p_received.points().at(i), p.points().at(i))
            }
        }
        
        cpt.join();
        delete producer;
    }

    void test_notification(){

        ConsumerCollisionNotification consumer_ntf(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");

        CollisionNotificationPacket p("h0",0,"r0",3,DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301, cast_positive(FloatType(0.5,dp)));

        std::thread cpt([&]{ consumer_ntf.check_new_message();} );
                
        send_collision_notification(p, producer);

        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        ARIADNE_TEST_ASSERT(consumer_ntf.number_new_msgs() > 0)
        
        CollisionNotificationPacket p_received = consumer_ntf.get_pkt();
        
        consumer_ntf.set_run(false);
        
        ARIADNE_TEST_EQUAL(p_received.human_id(), p.human_id())
        ARIADNE_TEST_EQUAL(p_received.robot_id(), p.robot_id())
        ARIADNE_TEST_EQUAL(p_received.human_segment_id(), p.human_segment_id())
        ARIADNE_TEST_EQUAL(p_received.robot_segment_id(), p.robot_segment_id())
        ARIADNE_TEST_EQUAL(p_received.upper_collision_time(), p.upper_collision_time())
        ARIADNE_TEST_EQUAL(p_received.lower_collision_time(), p.lower_collision_time())
        ARIADNE_TEST_EQUAL(p_received.likelihood().get_d(), p.likelihood().get_d())

        cpt.join();
        delete producer;
    }

};

int main() {
    int id = fork();
    if (id == 0){
        int idchild = fork();
        if(idchild>0){
            std::cout << "Starting Zookeeper server..." << std::endl;
            std::string command = std::string("cd ") + RESOURCES_PATH + std::string("kafka; zookeeper-server-start zookeeper.properties>>/dev/null 2>>/dev/null");
            system(command.c_str());
            std::cout << "Stopped Zookeeper server." << std::endl;
            exit(0);
        }
        else if(idchild ==0){
            std::this_thread::sleep_for(std::chrono::milliseconds(30000));
            std::cout << "Starting Kafka server..." << std::endl;
            std::string command = std::string("cd ") + RESOURCES_PATH + std::string("kafka; kafka-server-start server.properties>>/dev/null 2>>/dev/null");
            system(command.c_str());
            std::cout << "Stopped Kafka server." << std::endl;
            exit(0);
        }
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(40000));
        std::cout << "Creating topics..." << std::endl;
        system("kafka-topics --create --topic opera-presentation --bootstrap-server localhost:9092");
        system("kafka-topics --create --topic opera-state --bootstrap-server localhost:9092");
        system("kafka-topics --create --topic opera-collision-notification --bootstrap-server localhost:9092");
        TestKafka().test();
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        std::cout << "Deleting topics..." << std::endl;
        system("kafka-topics --delete --topic opera-presentation --bootstrap-server localhost:9092");
        system("kafka-topics --delete --topic opera-state --bootstrap-server localhost:9092");
        system("kafka-topics --delete --topic opera-collision-notification --bootstrap-server localhost:9092");
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        std::cout << "Stopping Kafka server..." << std::endl;
        system("kafka-server-stop");
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        std::cout << "Stopping Zookeeper server..." << std::endl;
        system("zookeeper-server-stop");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return ARIADNE_TEST_FAILURES;
    }
}
