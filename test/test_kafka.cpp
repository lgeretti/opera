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
        ARIADNE_TEST_CALL(test_broker())
    }

    void test_presentation(){

        KafkaBodyPresentationConsumer consumer(0, "localhost:9092", RdKafka::Topic::OFFSET_BEGINNING);
        KafkaBodyPresentationProducer producer("localhost:9092");

        List<BodyPresentationPacket> sent;
        sent.append(BodyPresentationPacket("human1", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)}));
        sent.append(BodyPresentationPacket("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)}));

        bool stop = false;
        List<BodyPresentationPacket> received;
        std::thread cpt([&]{
            while(not stop) {
                received.append(consumer.get());
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

        producer.put(sent.at(0));
        producer.put(sent.at(1));

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        ARIADNE_TEST_EQUAL(received.size(),2)
            
        for (SizeType i=0; i<received.size(); ++i) {
            auto const& s = sent.at(i);
            auto const& r = received.at(i);
            ARIADNE_TEST_EQUAL(r.id(), s.id())
            ARIADNE_TEST_EQUAL(r.is_human(), s.is_human())
            ARIADNE_TEST_EQUAL(r.packet_frequency(), s.packet_frequency())
            for(SizeType j = 0; j<s.point_ids().size(); j++){
                ARIADNE_TEST_EQUAL(r.point_ids()[j].first, s.point_ids()[j].first)
                ARIADNE_TEST_EQUAL(r.point_ids()[j].second, s.point_ids()[j].second)
            }
            for(SizeType j = 0; j<s.thicknesses().size(); j++){
                ARIADNE_TEST_EQUAL(r.thicknesses()[j], s.thicknesses()[j])
            }
        }

        stop = true;
        cpt.join();
    }

    void test_state(){

        KafkaBodyStateConsumer consumer(0, "localhost:9092", RdKafka::Topic::OFFSET_BEGINNING);
        KafkaBodyStateProducer producer("localhost:9092");

        List<BodyStatePacket> sent;
        sent.append(BodyStatePacket("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290));
        sent.append(BodyStatePacket("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230));

        bool stop = false;
        List<BodyStatePacket> received;
        std::thread cpt([&]{
            while(not stop) {
                received.append(consumer.get());
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

        producer.put(sent.at(0));
        producer.put(sent.at(1));

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        ARIADNE_TEST_EQUAL(received.size(),2)

        for (SizeType i=0; i<received.size(); ++i) {
            auto const& s = sent.at(i);
            auto const& r = received.at(i);
            ARIADNE_TEST_EQUAL(s.id(), r.id())
            ARIADNE_TEST_EQUAL(s.location(), r.location())
            ARIADNE_TEST_EQUAL(s.timestamp(), r.timestamp())
            for(int j = 0; j<s.points().size(); j++){
                ARIADNE_TEST_EQUAL(s.points().at(j), r.points().at(j))
            }
        }

        stop = true;
        cpt.join();
    }

    void test_notification(){

        KafkaCollisionNotificationConsumer consumer(0, "localhost:9092", RdKafka::Topic::OFFSET_BEGINNING);
        KafkaCollisionNotificationProducer producer("localhost:9092");

        CollisionNotificationPacket s("h0",0,"r0",3,DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301, cast_positive(FloatType(0.5,dp)));

        bool stop = false;
        List<CollisionNotificationPacket> received;
        std::thread cpt([&]{
            while(not stop) {
                received.append(consumer.get());
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

        producer.put(s);

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        ARIADNE_TEST_EQUAL(received.size(),1)
        auto const& r = received.at(0);
        
        ARIADNE_TEST_EQUAL(r.human_id(), s.human_id())
        ARIADNE_TEST_EQUAL(r.robot_id(), s.robot_id())
        ARIADNE_TEST_EQUAL(r.human_segment_id(), s.human_segment_id())
        ARIADNE_TEST_EQUAL(r.robot_segment_id(), s.robot_segment_id())
        ARIADNE_TEST_EQUAL(r.upper_collision_time(), s.upper_collision_time())
        ARIADNE_TEST_EQUAL(r.lower_collision_time(), s.lower_collision_time())
        ARIADNE_TEST_EQUAL(r.likelihood().get_d(), s.likelihood().get_d())

        stop = true;
        cpt.join();
    }

    void test_broker() {

        KafkaBroker broker(0, "localhost:9092", RdKafka::Topic::OFFSET_END);

        ARIADNE_TEST_ASSERT(broker.kind() == BrokerKind::KAFKA)

        BodyPresentationPacket bp_sent("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)});
        BodyStatePacket bs_sent("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);
        CollisionNotificationPacket cn_sent("h0",0,"r0",3,DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301, cast_positive(FloatType(0.5,dp)));

        std::deque<BodyPresentationPacket> bp_received;
        std::deque<BodyStatePacket> bs_received;
        std::deque<CollisionNotificationPacket> cn_received;

        bool stop = false;
        std::thread cpt([&]{
            while(not stop) {
                broker.receive(bp_received);
                broker.receive(bs_received);
                broker.receive(cn_received);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        broker.send(bp_sent);
        broker.send(bs_sent);
        broker.send(cn_sent);

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        ARIADNE_TEST_EQUAL(bp_received.size(),1)
        ARIADNE_TEST_EQUAL(bs_received.size(),1)
        ARIADNE_TEST_EQUAL(cn_received.size(),1)

        stop = true;
        cpt.join();
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
