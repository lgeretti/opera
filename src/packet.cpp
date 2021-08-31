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

#include "packet.hpp"

namespace Opera {

BodyStatePacket::BodyStatePacket(BodyIdType const& id, List<List<Point>> const& points, TimestampType const& timestamp) :
    _id(id), _points(points), _timestamp(timestamp) {}

BodyIdType const& BodyStatePacket::id() const {
    return _id;
}

List<List<Point>> const& BodyStatePacket::points() const {
    return _points;
}

TimestampType const& BodyStatePacket::timestamp() const {
    return _timestamp;
}

RobotStatePacket::RobotStatePacket(BodyIdType const& id, DiscreteLocation const& location, List<List<Point>> const& points, TimestampType const& timestamp) :
    BodyStatePacket(id,points,timestamp), _location(location) {}

DiscreteLocation const& RobotStatePacket::location() const {
    return _location;
}

}