/***************************************************************************
 *            packet.cpp
 *
 *  Copyright  2021  Luca Geretti
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

#include <ariadne/utility/handle.hpp>
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