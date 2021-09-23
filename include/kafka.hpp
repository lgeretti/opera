/***************************************************************************
 *            kafka.hpp
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

#ifndef OPERA_KAFKA_HPP
#define OPERA_KAFKA_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <tuple>
#include <thread>

#include <librdkafka/rdkafkacpp.h>

#include "packet.hpp"
#include "serialisation.hpp"
#include "deserialisation.hpp"

namespace Opera{

class Consumer{
    public:
        Consumer(int partition, std::string brokers, std::string errstr, int start_offset);

        virtual void check_new_message() = 0;

        virtual void set_run(bool run_value) = 0;
    
    protected:
        std::string brokers;
        std::string errstr;
        int partition;
        int start_offset;
        bool run;
        std::string topic_string;
        RdKafka::Consumer *consumer;
        RdKafka::Topic *topic;
};

class ConsumerPresentation : public Consumer{
    public:
        ConsumerPresentation(int partition, std::string brokers, std::string errstr, int start_offset);

        void check_new_message();

        void set_run(bool run_val);

        BodyPresentationPacket get_pkt();

        int number_new_msgs();

        ~ConsumerPresentation();
        
    private:
        std::list <std::string> _prs_str_list;
};



class ConsumerState : public Consumer{
    public:
        ConsumerState(int partition, std::string brokers, std::string errstr, int start_offset);

        void check_new_message();

        void set_run(bool run_val);

        BodyStatePacket get_pkt();

        int number_new_msgs();

        ~ConsumerState();
        
    private:
        std::list <std::string> _st_str_list;
        
};

class ConsumerCollisionNotification : public Consumer{
    public:
        ConsumerCollisionNotification(int partition, std::string brokers, std::string errstr, int start_offset);

        void check_new_message();

        void set_run(bool run_val);

        CollisionNotificationPacket get_pkt();

        int number_new_msgs();

        ~ConsumerCollisionNotification();
        
    private:
        std::list<std::string> _ntf_str_list;
};

void consumer_prs_thread(ConsumerPresentation * o);

void consumer_st_thread(ConsumerState * o);

void consumer_ntf_thread(ConsumerCollisionNotification * o);

RdKafka::Producer * create_producer(std::string brokers);

void send_presentation(BodyPresentationPacket p, RdKafka::Producer * producer);

void send_state(BodyStatePacket p, RdKafka::Producer * producer);

void send_collision_notification(CollisionNotificationPacket p, RdKafka::Producer * producer);

}

#endif // OPERA_KAFKA_HPP