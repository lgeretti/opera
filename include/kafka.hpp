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

#include "broker.hpp"
#include "serialisation.hpp"
#include "deserialisation.hpp"

namespace Opera {

static const std::string OPERA_PRESENTATION_TOPIC = "opera-presentation";
static const std::string OPERA_STATE_TOPIC = "opera-state";
static const std::string OPERA_COLLISION_NOTIFICATION_TOPIC = "opera-collision-notification";

template<class T> class KafkaConsumerBase {
  protected:
    KafkaConsumerBase(std::string topic_string, int partition, std::string brokers, int start_offset);
  public:
    virtual List<T> get() = 0;

    ~KafkaConsumerBase();

  protected:
    List<std::string> _get();
  private:
    int _partition;
    RdKafka::Consumer* _consumer;
    RdKafka::Topic* _topic;
};

class KafkaBodyPresentationConsumer : public KafkaConsumerBase<BodyPresentationPacket> {
  public:
    KafkaBodyPresentationConsumer(int partition, std::string brokers, int start_offset);
    List<BodyPresentationPacket> get() override;
};

class KafkaBodyStateConsumer : public KafkaConsumerBase<BodyStatePacket> {
  public:
    KafkaBodyStateConsumer(int partition, std::string brokers, int start_offset);
    List<BodyStatePacket> get() override;
};

class KafkaCollisionNotificationConsumer : public KafkaConsumerBase<CollisionNotificationPacket> {
  public:
    KafkaCollisionNotificationConsumer(int partition, std::string brokers, int start_offset);
    List<CollisionNotificationPacket> get() override;
};

template<class T> class KafkaProducerBase {
  protected:
    KafkaProducerBase(std::string const& topic_string, std::string const& brokers);

    virtual void put(T const& p) = 0;

    ~KafkaProducerBase();

  protected:
    void _put(std::string const& serialised_packet);
  private:
    std::string const _topic_string;
    RdKafka::Producer* _producer;
};

class KafkaBodyPresentationProducer : public KafkaProducerBase<BodyPresentationPacket> {
public:
    KafkaBodyPresentationProducer(std::string const& brokers);
    void put(BodyPresentationPacket const& p) override;
};

class KafkaBodyStateProducer : public KafkaProducerBase<BodyStatePacket> {
public:
    KafkaBodyStateProducer(std::string const& brokers);
    void put(BodyStatePacket const& p) override;
};

class KafkaCollisionNotificationProducer : public KafkaProducerBase<CollisionNotificationPacket> {
public:
    KafkaCollisionNotificationProducer(std::string const& brokers);
    void put(CollisionNotificationPacket const& p) override;
};

//! \brief A broker to handle packets using Kafka
class KafkaBroker : public BrokerInterface {
  public:
    //! \brief Construct from consumer/producer required arguments
    KafkaBroker(int partition, std::string brokers, int start_offset);

    BrokerKind kind() const override;

    void send(BodyPresentationPacket const& p) override;
    void send(BodyStatePacket const& p) override;
    void send(CollisionNotificationPacket const& p) override;

    void receive(List<BodyPresentationPacket>& packets) override;
    void receive(List<BodyStatePacket>& packets) override;
    void receive(List<CollisionNotificationPacket>& packets) override;

  private:
    KafkaBodyPresentationConsumer _body_presentation_c;
    KafkaBodyStateConsumer _body_state_c;
    KafkaCollisionNotificationConsumer _collision_notification_c;
    KafkaBodyPresentationProducer _body_presentation_p;
    KafkaBodyStateProducer _body_state_p;
    KafkaCollisionNotificationProducer _collision_notification_p;
};

template<class T> KafkaConsumerBase<T>::KafkaConsumerBase(std::string topic_string, int partition, std::string brokers, int start_offset) :
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

template<class T> KafkaConsumerBase<T>::~KafkaConsumerBase() {
    _consumer->stop(_topic, _partition);
    delete _topic;
    delete _consumer;
}

template<class T> List<std::string> KafkaConsumerBase<T>::_get(){
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

template<class T> KafkaProducerBase<T>::KafkaProducerBase(std::string const& topic_string, std::string const& brokers)
    : _topic_string(topic_string)
{
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;
    conf->set("metadata.broker.list", brokers, errstr);
    _producer = RdKafka::Producer::create(conf, errstr);
    ARIADNE_ASSERT_MSG(_producer,"Failed to create producer: " << errstr)
}

template<class T> KafkaProducerBase<T>::~KafkaProducerBase() {
    delete _producer;
}

template<class T> void KafkaProducerBase<T>::_put(std::string const& serialised_packet) {
    _producer->produce(_topic_string, 0,
    RdKafka::Producer::RK_MSG_COPY,
    const_cast<char *>(serialised_packet.c_str()),
    serialised_packet.size(),NULL, 0, 0, NULL);
}

}

#endif // OPERA_KAFKA_HPP