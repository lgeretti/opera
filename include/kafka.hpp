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

static const std::string OPERA_PRESENTATION_TOPIC = "opera-presentation";
static const std::string OPERA_STATE_TOPIC = "opera-state";
static const std::string OPERA_COLLISION_NOTIFICATION_TOPIC = "opera-collision-notification";

template<class T> class ConsumerBase {
  protected:
    ConsumerBase(std::string topic_string, int partition, std::string brokers, std::string errstr, int start_offset);
  public:
    void check_new_message();
    void set_run(bool run_value);

    virtual T get_pkt() = 0;

    int number_new_msgs();

    ~ConsumerBase();

  private:
    int partition;
    bool run;
    RdKafka::Consumer *consumer;
    RdKafka::Topic *topic;
  protected:
    std::list<std::string> _str_list;
};

class PresentationConsumer : public ConsumerBase<BodyPresentationPacket> {
  public:
    PresentationConsumer(int partition, std::string brokers, std::string errstr, int start_offset);
    BodyPresentationPacket get_pkt() override;
};

class StateConsumer : public ConsumerBase<BodyStatePacket> {
  public:
    StateConsumer(int partition, std::string brokers, std::string errstr, int start_offset);
    BodyStatePacket get_pkt() override;
};

class CollisionNotificationConsumer : public ConsumerBase<CollisionNotificationPacket> {
  public:
    CollisionNotificationConsumer(int partition, std::string brokers, std::string errstr, int start_offset);
    CollisionNotificationPacket get_pkt() override;
};

RdKafka::Producer * create_producer(std::string brokers);

void send_presentation(BodyPresentationPacket p, RdKafka::Producer * producer);

void send_state(BodyStatePacket p, RdKafka::Producer * producer);

void send_collision_notification(CollisionNotificationPacket p, RdKafka::Producer * producer);

template<class T> ConsumerBase<T>::ConsumerBase(std::string topic_string, int partition, std::string brokers, std::string errstr, int start_offset) :
        partition(partition), run(true) {

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    conf->set("metadata.broker.list", brokers, errstr);

    consumer = RdKafka::Consumer::create(conf, errstr);
    ARIADNE_ASSERT_MSG(consumer,"Failed to create consumer: " << errstr)

    topic = RdKafka::Topic::create(consumer, topic_string, tconf, errstr);
    ARIADNE_ASSERT_MSG(topic,"Failed to create topic: " << errstr)

    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);
    ARIADNE_ASSERT_MSG(resp == RdKafka::ERR_NO_ERROR,"Failed to start consumer: " << RdKafka::err2str(resp))
}

template<class T> ConsumerBase<T>::~ConsumerBase() {
    consumer->stop(topic,0);
    delete topic;
    delete consumer;
}

template<class T> void ConsumerBase<T>::check_new_message(){
    while (run) {
        RdKafka::Message *msg = consumer->consume(topic, partition, 1000);
        if(msg->err() == RdKafka::ERR_NO_ERROR){
            std::string prs_str = static_cast<const char *> (msg->payload());
            _str_list.push_back(prs_str);
        } else {
            ARIADNE_ASSERT_MSG(msg->err() == RdKafka::ERR__TIMED_OUT,"Consume failed: " << msg->errstr())
        }

        delete msg;
        consumer->poll(0);  // interroga l'handler degli eventi di Kafka
    }
}

template<class T> void ConsumerBase<T>::set_run(bool run_val){
    run = run_val;
}

template<class T> int ConsumerBase<T>::number_new_msgs() {
    return _str_list.size();
}

}

#endif // OPERA_KAFKA_HPP