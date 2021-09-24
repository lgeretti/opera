/***************************************************************************
 *            kafka.cpp
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

#include "kafka.hpp"

namespace Opera {

PresentationConsumer::PresentationConsumer(int partition, std::string brokers, std::string errstr, int start_offset) :
    ConsumerBase(OPERA_PRESENTATION_TOPIC, partition, brokers, errstr, start_offset) { }

BodyPresentationPacket PresentationConsumer::get_pkt(){
    std::string prs_str = _str_list.front();
    _str_list.pop_front();
    BodyPresentationPacketDeserialiser d(prs_str.c_str());
    return d.make();    
}

StateConsumer::StateConsumer(int partition, std::string brokers, std::string errstr, int start_offset) :
    ConsumerBase(OPERA_STATE_TOPIC, partition, brokers, errstr, start_offset) { }

BodyStatePacket StateConsumer::get_pkt() {
    std::string st_str = _str_list.front();
    _str_list.pop_front();
    BodyStatePacketDeserialiser d(st_str.c_str());
    return d.make();
}

CollisionNotificationConsumer::CollisionNotificationConsumer(int partition, std::string brokers, std::string errstr, int start_offset)
    : ConsumerBase(OPERA_COLLISION_NOTIFICATION_TOPIC, partition, brokers, errstr, start_offset) { }

CollisionNotificationPacket CollisionNotificationConsumer::get_pkt(){
    std::string ntf_str = _str_list.front();
    _str_list.pop_front();
    CollisionNotificationPacketDeserialiser d(ntf_str.c_str());
    return d.make();
}

RdKafka::Producer * create_producer(std::string brokers){
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;
    conf->set("metadata.broker.list", brokers, errstr);
    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    ARIADNE_ASSERT_MSG(producer,"Failed to create producer: " << errstr)
    return producer;
}

void send_presentation(BodyPresentationPacket p , RdKafka::Producer * producer){
    BodyPresentationPacketSerialiser serialiser(p);
    std::string msg_to_send = serialiser.to_string();

	producer->produce(OPERA_PRESENTATION_TOPIC, 0,
						RdKafka::Producer::RK_MSG_COPY,
						const_cast<char *>(msg_to_send.c_str()),
						msg_to_send.size(),NULL, 0, 0, NULL);
}

void send_state(BodyStatePacket p, RdKafka::Producer * producer){
	BodyStatePacketSerialiser serialiser(p);
    std::string msg_to_send = serialiser.to_string();
	
	producer->produce(OPERA_STATE_TOPIC, 0,
						RdKafka::Producer::RK_MSG_COPY,
						const_cast<char *>(msg_to_send.c_str()),
						msg_to_send.size(),NULL, 0, 0, NULL);
}

void send_collision_notification(CollisionNotificationPacket p, RdKafka::Producer * producer){
    CollisionNotificationPacketSerialiser serialiser(p);
    std::string msg_to_send = serialiser.to_string();

	producer->produce(OPERA_COLLISION_NOTIFICATION_TOPIC, 0,
						RdKafka::Producer::RK_MSG_COPY,
						const_cast<char *>(msg_to_send.c_str()),
						msg_to_send.size(),NULL, 0, 0, NULL);
}

}