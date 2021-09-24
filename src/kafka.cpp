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

PresentationConsumer::PresentationConsumer(int partition, std::string brokers, int start_offset) :
    ConsumerBase(OPERA_PRESENTATION_TOPIC, partition, brokers, start_offset) { }

List<BodyPresentationPacket> PresentationConsumer::get() {
    List<BodyPresentationPacket> result;
    List<std::string> msg_list = _get();
    for (auto msg : msg_list)
        result.append(BodyPresentationPacketDeserialiser(msg.c_str()).make());
    return result;
}

StateConsumer::StateConsumer(int partition, std::string brokers, int start_offset) :
    ConsumerBase(OPERA_STATE_TOPIC, partition, brokers, start_offset) { }

List<BodyStatePacket> StateConsumer::get() {
    List<BodyStatePacket> result;
    List<std::string> msg_list = _get();
    for (auto msg : msg_list)
        result.append(BodyStatePacketDeserialiser(msg.c_str()).make());
    return result;
}

CollisionNotificationConsumer::CollisionNotificationConsumer(int partition, std::string brokers, int start_offset)
    : ConsumerBase(OPERA_COLLISION_NOTIFICATION_TOPIC, partition, brokers, start_offset) { }

List<CollisionNotificationPacket> CollisionNotificationConsumer::get() {
    List<CollisionNotificationPacket> result;
    List<std::string> msg_list = _get();
    for (auto msg : msg_list)
        result.append(CollisionNotificationPacketDeserialiser(msg.c_str()).make());
    return result;
}

PresentationProducer::PresentationProducer(std::string const& brokers)
    : ProducerBase(OPERA_PRESENTATION_TOPIC, brokers) { }


StateProducer::StateProducer(std::string const& brokers)
    : ProducerBase(OPERA_STATE_TOPIC, brokers) { }

CollisionNotificationProducer::CollisionNotificationProducer(std::string const& brokers)
    : ProducerBase(OPERA_COLLISION_NOTIFICATION_TOPIC, brokers) { }

void PresentationProducer::put(const BodyPresentationPacket &p) {
    _put(BodyPresentationPacketSerialiser(p).to_string());
}

void StateProducer::put(const BodyStatePacket &p) {
    _put(BodyStatePacketSerialiser(p).to_string());
}

void CollisionNotificationProducer::put(const CollisionNotificationPacket &p) {
    _put(CollisionNotificationPacketSerialiser(p).to_string());
}

}