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

BodyPresentationPacket PresentationConsumer::get_packet(){
    std::string prs_str = _str_list.front();
    _str_list.pop_front();
    BodyPresentationPacketDeserialiser d(prs_str.c_str());
    return d.make();
}

StateConsumer::StateConsumer(int partition, std::string brokers, std::string errstr, int start_offset) :
    ConsumerBase(OPERA_STATE_TOPIC, partition, brokers, errstr, start_offset) { }

BodyStatePacket StateConsumer::get_packet() {
    std::string st_str = _str_list.front();
    _str_list.pop_front();
    BodyStatePacketDeserialiser d(st_str.c_str());
    return d.make();
}

CollisionNotificationConsumer::CollisionNotificationConsumer(int partition, std::string brokers, std::string errstr, int start_offset)
    : ConsumerBase(OPERA_COLLISION_NOTIFICATION_TOPIC, partition, brokers, errstr, start_offset) { }

CollisionNotificationPacket CollisionNotificationConsumer::get_packet() {
    std::string ntf_str = _str_list.front();
    _str_list.pop_front();
    CollisionNotificationPacketDeserialiser d(ntf_str.c_str());
    return d.make();
}

PresentationProducer::PresentationProducer(std::string const& brokers)
    : ProducerBase(OPERA_PRESENTATION_TOPIC, brokers) { }


StateProducer::StateProducer(std::string const& brokers)
    : ProducerBase(OPERA_STATE_TOPIC, brokers) { }

CollisionNotificationProducer::CollisionNotificationProducer(std::string const& brokers)
    : ProducerBase(OPERA_COLLISION_NOTIFICATION_TOPIC, brokers) { }

void PresentationProducer::send(const BodyPresentationPacket &p) {
    _send(BodyPresentationPacketSerialiser(p).to_string());
}

void StateProducer::send(const BodyStatePacket &p) {
    _send(BodyStatePacketSerialiser(p).to_string());
}

void CollisionNotificationProducer::send(const CollisionNotificationPacket &p) {
    _send(CollisionNotificationPacketSerialiser(p).to_string());
}

}