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
        ARIADNE_TEST_CALL(test_comunication_presentation())
        ARIADNE_TEST_CALL(test_comunication_state())
        ARIADNE_TEST_CALL(test_comunication_notification())
        
        std::cout<<"\n\nTEST TERMINATED\n\n";

    }

    void test_comunication_presentation(){

        ConsumerPresentation * consumer_pres;
        consumer_pres = new ConsumerPresentation(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");
        
        BodyPresentationPacket p("human1", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)});
        
        BodyPresentationPacket p2("human2", {{10, 11},{13, 12}}, {FloatType(11.0, Ariadne::dp),FloatType(10.5, Ariadne::dp)});

        // possible problem with following packet related to json decoding.... 
        //BodyPresentationPacket p("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)});

        std::thread cpt(consumer_prs_thread, consumer_pres);

        ARIADNE_TEST_CALL(send_presentation(p, producer));

        ARIADNE_TEST_CALL(send_presentation(p2, producer));


        usleep(3000000);   //needed to let kafka handle msgs

        
        if(consumer_pres->number_new_msgs() == 0){
            std::cout<<"\nNo messages in the queue to read\n";
        }

        else{
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

        }

        if(consumer_pres->number_new_msgs() == 0){
            std::cout<<"\nNo messages in the queue to read\n";
        }

        else{
            BodyPresentationPacket p_recived = consumer_pres->get_pkg();
            
            consumer_pres->set_run(false);
            
            ARIADNE_TEST_EQUAL(p_recived.id(), p2.id());

            ARIADNE_TEST_EQUAL(p_recived.is_human(), p2.is_human());

            ARIADNE_TEST_EQUAL(p_recived.packet_frequency(), p2.packet_frequency());

            for(int i = 0; i<p2.point_ids().size(); i++){
                ARIADNE_TEST_EQUAL(p_recived.point_ids()[i].first, p2.point_ids()[i].first);
                ARIADNE_TEST_EQUAL(p_recived.point_ids()[i].second, p2.point_ids()[i].second);
            }
                    
            for(int i = 0; i<p2.thicknesses().size(); i++){
                ARIADNE_TEST_EQUAL(p_recived.thicknesses()[i], p2.thicknesses()[i]);
            }

        }
        cpt.join();
        delete consumer_pres;
        delete producer;


    }

    void test_comunication_state(){

        ConsumerState * consumer_st;
        consumer_st = new ConsumerState(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");

        // BodyStatePacket p("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290);
        
        BodyStatePacket p("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230);

        std::thread cpt(consumer_st_thread, consumer_st);              

        ARIADNE_TEST_CALL(send_state(p, producer));

        usleep(300000); //needed to let kafka handle msgs

        if(consumer_st->number_new_msgs() == 0){
            std::cout<<"\nNo messages in the queue to read\n";
        }

        else{
            BodyStatePacket p_recived = consumer_st->get_pkg();
            
            consumer_st->set_run(false);
            
            ARIADNE_TEST_EQUAL(p_recived.id(), p.id());

            ARIADNE_TEST_EQUAL(p_recived.location(), p.location());

            ARIADNE_TEST_EQUAL(p_recived.timestamp(), p.timestamp());
                    
            for(int i = 0; i<p.points().size(); i++){
                ARIADNE_TEST_EQUAL(p_recived.points().at(i), p.points().at(i));
            }
        }

        
        cpt.join();
        delete consumer_st;
        delete producer;
    }

    void test_comunication_notification(){

        ConsumerCollisionNotification * consumer_ntf;
        consumer_ntf = new ConsumerCollisionNotification(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");

        CollisionNotificationPacket p("h0",0,"r0",3,DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}), 328903284232, 328905923301, cast_positive(FloatType(0.5,dp)));

        std::thread cpt(consumer_ntf_thread, consumer_ntf);
                
        ARIADNE_TEST_CALL(send_collision_notification(p, producer));

        usleep(300000); //needed to let kafka handle msgs
        
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
        delete consumer_ntf;
        delete producer;
    }

};

int main() {

    int id = fork();
    std::string command;

    if (id == 0){
        //child

        int idchild = fork();
        if(idchild>0){
            //launching zookeeper

            std::cout<<"\nZookeper started\n";
            //to get the output remove the last part: >>/dev/null 2>>/dev/null
            command = std::string("cd ") + RESOURCES_PATH + std::string("kafka; zookeeper-server-start.sh zookeeper.properties>>/dev/null 2>>/dev/null");
            system(command.c_str());
            wait(NULL);
            exit(0);
        }
        else if(idchild ==0){
            /*
                20 seconds are needed for the zookeeper to resolve/clean old session if the 
                previous session had problems. 
                https://github.com/wurstmeister/kafka-docker/issues/389#issuecomment-800814529
            */
            usleep(20000000);   

            std::cout<<"\nKafka server started\n";
            //to get the output remove the last part: >>/dev/null 2>>/dev/null
            command = std::string("cd ") + RESOURCES_PATH + std::string("kafka; kafka-server-start.sh server.properties>>/dev/null 2>>/dev/null");
            system(command.c_str());
            exit(0);
        }

    }

    if(id>0){
        //parent
        usleep(21000000);
        system("kafka-topics.sh --create --topic opera-presentation --bootstrap-server localhost:9092");
        system("kafka-topics.sh --create --topic opera-state --bootstrap-server localhost:9092");
        system("kafka-topics.sh --create --topic opera-collision-notification --bootstrap-server localhost:9092");
        TestKafka().test();

        std::cout<<"Deleting topics\n";
        system("kafka-topics.sh --delete --topic opera-presentation --bootstrap-server localhost:9092");
        system("kafka-topics.sh --delete --topic opera-state --bootstrap-server localhost:9092");
        system("kafka-topics.sh --delete --topic opera-collision-notification --bootstrap-server localhost:9092");
        std::cout<<"Closing kafka-server\n";
        system("kafka-server-stop.sh");
        std::cout<<"Closing zookeper-server\n";
        system("zookeeper-server-stop.sh");
        wait(NULL);
        return ARIADNE_TEST_FAILURES;
    }
    
    
}