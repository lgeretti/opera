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

using namespace Opera;

class TestKafka{
public: 
    void test(){
        ARIADNE_TEST_CALL(test_comunication_presentation())
        ARIADNE_TEST_CALL(test_comunication_state())
        ARIADNE_TEST_CALL(test_comunication_notification())
    }

    void test_comunication_presentation(){

        ConsumerPresentation * consumer_pres;
        consumer_pres = new ConsumerPresentation(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");
        
        BodyPresentationPacket p("human1", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)});
        
        // possible problem with following packet related to json decoding.... 
        //BodyPresentationPacket p("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)});

        std::thread cpt(consumer_prs_thread, consumer_pres);

        ARIADNE_TEST_CALL(send_presentation(p, producer));

        usleep(1000000);   //needed to let kafka handle msgs

        BodyPresentationPacket p_recived = consumer_pres->get_pkg();
        
        consumer_pres->set_run(false);
        
        ARIADNE_TEST_EQUAL(p_recived.id(), p.id());

        ARIADNE_TEST_EQUAL(p_recived.is_human(), p.is_human());

        ARIADNE_TEST_EQUAL(p_recived.packet_frequency(), p.packet_frequency());

        for(int i = 0; i<p.point_ids().size(); i++){
            ARIADNE_TEST_EQUAL(p_recived.point_ids()[i].first, p.point_ids()[i].first);
            ARIADNE_TEST_EQUAL(p_recived.point_ids()[i].second, p.point_ids()[i].second);
        }
                 
        for(int i = 0; i<p.thicknesses().size(); i++){
            ARIADNE_TEST_EQUAL(p_recived.thicknesses()[i], p.thicknesses()[i]);
        }

        cpt.join();
    }

    void test_comunication_state(){

        ConsumerState * consumer_st;
        consumer_st = new ConsumerState(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");

        // BodyStatePacket p("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290);
        
        BodyStatePacket p("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);

        std::thread cpt(consumer_st_thread, consumer_st);              

        ARIADNE_TEST_CALL(send_state(p, producer));

        usleep(100000); //needed to let kafka handle msgs

        BodyStatePacket p_recived = consumer_st->get_pkg();
        
        consumer_st->set_run(false);
        
        ARIADNE_TEST_EQUAL(p_recived.id(), p.id());

        ARIADNE_TEST_EQUAL(p_recived.location(), p.location());

        ARIADNE_TEST_EQUAL(p_recived.timestamp(), p.timestamp());
                 
        for(int i = 0; i<p.points().size(); i++){
            ARIADNE_TEST_EQUAL(p_recived.points().at(i), p.points().at(i));
        }

        cpt.join();
    }

    void test_comunication_notification(){

        ConsumerCollisionNotification * consumer_ntf;
        consumer_ntf = new ConsumerCollisionNotification(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");

        CollisionNotificationPacket p("h0",0,"r0",3,DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301, cast_positive(FloatType(0.5,dp)));

        std::thread cpt(consumer_ntf_thread, consumer_ntf);
                
        ARIADNE_TEST_CALL(send_collision_notification(p, producer));

        usleep(100000); //needed to let kafka handle msgs
        
        CollisionNotificationPacket p_recived = consumer_ntf->get_pkg();
        
        consumer_ntf->set_run(false);

        
        
        ARIADNE_TEST_EQUAL(p_recived.human_id(), p.human_id());

        ARIADNE_TEST_EQUAL(p_recived.robot_id(), p.robot_id());

        ARIADNE_TEST_EQUAL(p_recived.human_segment_id(), p.human_segment_id());

        ARIADNE_TEST_EQUAL(p_recived.robot_segment_id(), p.robot_segment_id());

        ARIADNE_TEST_EQUAL(p_recived.upper_collision_time(), p.upper_collision_time());

        ARIADNE_TEST_EQUAL(p_recived.lower_collision_time(), p.lower_collision_time());

        ARIADNE_TEST_EQUAL(p_recived.likelihood().get_d(), p.likelihood().get_d());


        cpt.join();
    }

};

int main() {
    //system("cd $(find ~ -path '*opera/resources/kafka'); zookeeper-server-start.sh zookeeper.properties");
    //system("cd $(find ~ -path '*opera/resources/kafka'); kafka-server-start.sh server.properties");
    //system("kafka-topics.sh --create --topic opera-presentation --bootstrap-server localhost:9092");
    //system("kafka-topics.sh --create --topic opera-state --bootstrap-server localhost:9092");
    //system("kafka-topics.sh --create --topic opera-collision-notification --bootstrap-server localhost:9092");
    system("./../resources/kafka/kafka_launch.sh");
    TestKafka().test();
    system("./../resources/kafka/delete_topics.sh");
    return ARIADNE_TEST_FAILURES;
}