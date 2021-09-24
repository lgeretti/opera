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

namespace Opera {

static const std::string OPERA_PRESENTATION_TOPIC = "opera-presentation";
static const std::string OPERA_STATE_TOPIC = "opera-state";
static const std::string OPERA_COLLISION_NOTIFICATION_TOPIC = "opera-collision-notification";

template<class T> class ConsumerBase {
  protected:
    ConsumerBase(std::string topic_string, int partition, std::string brokers, int start_offset);
  public:
    virtual List<T> get() = 0;

    ~ConsumerBase();

  protected:
    List<std::string> _get();
  private:
    int _partition;
    RdKafka::Consumer* _consumer;
    RdKafka::Topic* _topic;
};

class PresentationConsumer : public ConsumerBase<BodyPresentationPacket> {
  public:
    PresentationConsumer(int partition, std::string brokers, int start_offset);
    List<BodyPresentationPacket> get() override;
};

class StateConsumer : public ConsumerBase<BodyStatePacket> {
  public:
    StateConsumer(int partition, std::string brokers, int start_offset);
    List<BodyStatePacket> get() override;
};

class CollisionNotificationConsumer : public ConsumerBase<CollisionNotificationPacket> {
  public:
    CollisionNotificationConsumer(int partition, std::string brokers, int start_offset);
    List<CollisionNotificationPacket> get() override;
};

template<class T> class ProducerBase {
  protected:
    ProducerBase(std::string const& topic_string, std::string const& brokers);

    virtual void put(T const& p) = 0;

    ~ProducerBase();

  protected:
    void _put(std::string const& serialised_packet);
  private:
    std::string const _topic_string;
    RdKafka::Producer* _producer;
};

class PresentationProducer : public ProducerBase<BodyPresentationPacket> {
public:
    PresentationProducer(std::string const& brokers);
    void put(BodyPresentationPacket const& p) override;
};

class StateProducer : public ProducerBase<BodyStatePacket> {
public:
    StateProducer(std::string const& brokers);
    void put(BodyStatePacket const& p) override;
};

class CollisionNotificationProducer : public ProducerBase<CollisionNotificationPacket> {
public:
    CollisionNotificationProducer(std::string const& brokers);
    void put(CollisionNotificationPacket const& p) override;
};

template<class T> ConsumerBase<T>::ConsumerBase(std::string topic_string, int partition, std::string brokers, int start_offset) :
        _partition(partition) {

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    std::string errstr;

    conf->set("metadata.broker.list", brokers, errstr);

    _consumer = RdKafka::Consumer::create(conf, errstr);
    ARIADNE_ASSERT_MSG(_consumer, "Failed to create consumer: " << errstr)

    _topic = RdKafka::Topic::create(_consumer, topic_string, tconf, errstr);
    ARIADNE_ASSERT_MSG(_topic,"Failed to create topic: " << errstr)

    RdKafka::ErrorCode resp = _consumer->start(_topic, partition, start_offset);
    ARIADNE_ASSERT_MSG(resp == RdKafka::ERR_NO_ERROR,"Failed to start consumer: " << RdKafka::err2str(resp))
}

template<class T> ConsumerBase<T>::~ConsumerBase() {
    _consumer->stop(_topic, _partition);
    delete _topic;
    delete _consumer;
}

template<class T> List<std::string> ConsumerBase<T>::_get(){
    List<std::string> result;
    while (true) {
        RdKafka::Message *msg = _consumer->consume(_topic, _partition, 100);
        if(msg->err() == RdKafka::ERR_NO_ERROR){
            result.append(static_cast<const char*>(msg->payload()));
            delete msg;
        } else {
            ARIADNE_ASSERT_MSG(msg->err() == RdKafka::ERR__TIMED_OUT,"Consume failed: " << msg->errstr())
            delete msg;
            break;
        }
        _consumer->poll(0);  // interroga l'handler degli eventi di Kafka
    }
    return result;
}

template<class T> ProducerBase<T>::ProducerBase(std::string const& topic_string, std::string const& brokers)
    : _topic_string(topic_string)
{
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;
    conf->set("metadata.broker.list", brokers, errstr);
    _producer = RdKafka::Producer::create(conf, errstr);
    ARIADNE_ASSERT_MSG(_producer,"Failed to create producer: " << errstr)
}

template<class T> ProducerBase<T>::~ProducerBase() {
    delete _producer;
}

template<class T> void ProducerBase<T>::_put(std::string const& serialised_packet) {
    _producer->produce(_topic_string, 0,
    RdKafka::Producer::RK_MSG_COPY,
    const_cast<char *>(serialised_packet.c_str()),
    serialised_packet.size(),NULL, 0, 0, NULL);
}

}

#endif // OPERA_KAFKA_HPP