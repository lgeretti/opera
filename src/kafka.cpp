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

KafkaBodyPresentationConsumer::KafkaBodyPresentationConsumer(int partition, std::string brokers, int start_offset) :
        KafkaConsumerBase(OPERA_PRESENTATION_TOPIC, partition, brokers, start_offset) { }

List<BodyPresentationPacket> KafkaBodyPresentationConsumer::get() {
    List<BodyPresentationPacket> result;
    List<std::string> msg_list = _get();
    for (auto msg : msg_list)
        result.append(BodyPresentationPacketDeserialiser(msg.c_str()).make());
    return result;
}

KafkaBodyStateConsumer::KafkaBodyStateConsumer(int partition, std::string brokers, int start_offset) :
        KafkaConsumerBase(OPERA_STATE_TOPIC, partition, brokers, start_offset) { }

List<BodyStatePacket> KafkaBodyStateConsumer::get() {
    List<BodyStatePacket> result;
    List<std::string> msg_list = _get();
    for (auto msg : msg_list)
        result.append(BodyStatePacketDeserialiser(msg.c_str()).make());
    return result;
}

KafkaCollisionNotificationConsumer::KafkaCollisionNotificationConsumer(int partition, std::string brokers, int start_offset)
    : KafkaConsumerBase(OPERA_COLLISION_NOTIFICATION_TOPIC, partition, brokers, start_offset) { }

List<CollisionNotificationPacket> KafkaCollisionNotificationConsumer::get() {
    List<CollisionNotificationPacket> result;
    List<std::string> msg_list = _get();
    for (auto msg : msg_list)
        result.append(CollisionNotificationPacketDeserialiser(msg.c_str()).make());
    return result;
}

KafkaBodyPresentationProducer::KafkaBodyPresentationProducer(std::string const& brokers)
    : KafkaProducerBase(OPERA_PRESENTATION_TOPIC, brokers) { }


KafkaBodyStateProducer::KafkaBodyStateProducer(std::string const& brokers)
    : KafkaProducerBase(OPERA_STATE_TOPIC, brokers) { }

KafkaCollisionNotificationProducer::KafkaCollisionNotificationProducer(std::string const& brokers)
    : KafkaProducerBase(OPERA_COLLISION_NOTIFICATION_TOPIC, brokers) { }

void KafkaBodyPresentationProducer::put(const BodyPresentationPacket &p) {
    _put(BodyPresentationPacketSerialiser(p).to_string());
}

void KafkaBodyStateProducer::put(const BodyStatePacket &p) {
    _put(BodyStatePacketSerialiser(p).to_string());
}

void KafkaCollisionNotificationProducer::put(const CollisionNotificationPacket &p) {
    _put(CollisionNotificationPacketSerialiser(p).to_string());
}

KafkaBroker::KafkaBroker(int partition, std::string brokers, int start_offset) :
    _body_presentation_c(partition,brokers,start_offset),
    _body_state_c(partition,brokers,start_offset),
    _collision_notification_c(partition,brokers,start_offset),
    _body_presentation_p(brokers),
    _body_state_p(brokers),
    _collision_notification_p(brokers)
{ }

BrokerKind KafkaBroker::kind() const {
    return BrokerKind::KAFKA;
}

void KafkaBroker::send(BodyPresentationPacket const& p) {
    _body_presentation_p.put(p);
}

void KafkaBroker::send(BodyStatePacket const& p) {
    _body_state_p.put(p);
}

void KafkaBroker::send(CollisionNotificationPacket const& p) {
    _collision_notification_p.put(p);
}

void KafkaBroker::receive(std::deque<BodyPresentationPacket>& packets) {
    auto packets_got = _body_presentation_c.get();
    for (auto& p : packets_got) packets.push_back(p);
}

void KafkaBroker::receive(std::deque<BodyStatePacket>& packets) {
    auto packets_got = _body_state_c.get();
    for (auto& p : packets_got) packets.push_back(p);
}

void KafkaBroker::receive(std::deque<CollisionNotificationPacket>& packets) {
    auto packets_got = _collision_notification_c.get();
    for (auto& p : packets_got) packets.push_back(p);
}

}