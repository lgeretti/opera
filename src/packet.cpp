/***************************************************************************
 *            packet.cpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of Opera, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "handle.hpp"
#include "packet.hpp"

namespace Opera {

BodyPresentationPacket::BodyPresentationPacket(BodyIdType const& id, List<Pair<IdType,IdType>> const& point_ids, List<FloatType> const& thicknesses) :
    _id(id), _is_human(true), _packet_frequency(0), _point_ids(point_ids), _thicknesses(thicknesses) { }

BodyPresentationPacket::BodyPresentationPacket(BodyIdType const& id,  SizeType const& packet_frequency, List<Pair<IdType,IdType>> const& point_ids, List<FloatType> const& thicknesses) :
    _id(id), _is_human(false), _packet_frequency(packet_frequency), _point_ids(point_ids), _thicknesses(thicknesses) { }

BodyIdType const& BodyPresentationPacket::id() const {
    return _id;
}

bool const& BodyPresentationPacket::is_human() const {
    return _is_human;
}

SizeType const& BodyPresentationPacket::packet_frequency() const {
    return _packet_frequency;
}

List<Pair<IdType,IdType>> const& BodyPresentationPacket::point_ids() const {
    return _point_ids;
}

List<FloatType> const& BodyPresentationPacket::thicknesses() const {
    return _thicknesses;
}

BodyStatePacket::BodyStatePacket(BodyIdType const& id, DiscreteState const& location, List<List<Point>> const& points, TimestampType const& timestamp) :
    _id(id), _location(location), _points(points), _timestamp(timestamp) { }

BodyStatePacket::BodyStatePacket(BodyIdType const& id, List<List<Point>> const& points, TimestampType const& timestamp) :
    BodyStatePacket(id,DiscreteState({}),points,timestamp) { }

BodyIdType const& BodyStatePacket::id() const {
    return _id;
}

DiscreteState const& BodyStatePacket::location() const {
    return _location;
}

List<List<Point>> const& BodyStatePacket::points() const {
    return _points;
}

TimestampType const& BodyStatePacket::timestamp() const {
    return _timestamp;
}

CollisionNotificationPacket::CollisionNotificationPacket(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
                            DiscreteState const& discrete_state, TimestampType const& lower_collision_time, TimestampType const& upper_collision_time, PositiveFloatType const& likelihood) :
    _human_id(human_id), _human_segment_id(human_segment_id), _robot_id(robot_id), _robot_segment_id(robot_segment_id), _discrete_state(discrete_state),
    _lower_collision_time(lower_collision_time), _upper_collision_time(upper_collision_time), _likelihood(likelihood) { }

BodyIdType const& CollisionNotificationPacket::human_id() const {
    return _human_id;
}

IdType const& CollisionNotificationPacket::human_segment_id() const {
    return _human_segment_id;
}

BodyIdType const& CollisionNotificationPacket::robot_id() const {
    return _robot_id;
}

IdType const& CollisionNotificationPacket::robot_segment_id() const {
    return _robot_segment_id;
}

DiscreteState const& CollisionNotificationPacket::discrete_state() const {
    return _discrete_state;
}

TimestampType const& CollisionNotificationPacket::lower_collision_time() const {
    return _lower_collision_time;
}

TimestampType const& CollisionNotificationPacket::upper_collision_time() const {
    return _upper_collision_time;
}

PositiveFloatType const& CollisionNotificationPacket::likelihood() const {
    return _likelihood;
}

}