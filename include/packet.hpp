/***************************************************************************
 *            packet.hpp
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

#ifndef OPERA_PACKET_HPP
#define OPERA_PACKET_HPP

#include <ariadne/utility/container.hpp>
#include "geometry.hpp"

namespace Opera {

using IdType = unsigned int;
using BodyIdType = Ariadne::String;
using TimestampType = long unsigned int; // Expressed in nanoseconds
using Ariadne::List;
using Ariadne::DiscreteLocation;
using Ariadne::SizeType;
using Ariadne::String;

//! \brief A representation of an inbound packet for the state of a body
class BodyStatePacket {
  public:
    //! \brief Construct from an id, list of samples for each point, and a \a timestamp
    BodyStatePacket(BodyIdType const& id, List<List<Point>> const& points, TimestampType const& timestamp);
    //! \brief The id of the related body
    BodyIdType const& id() const;
    //! \brief The points for each segment
    //! \details These will be always even, at least two (head and tail) but multiple points may be present
    List<List<Point>> const& points() const;
    //! \brief The timestamp associated with the packet
    TimestampType const& timestamp() const;
  private:
    BodyIdType const _id;
    List<List<Point>> const _points;
    TimestampType const _timestamp;
};

//! \brief A representation of an inbound packet for the state of a human body
using HumanStatePacket = BodyStatePacket;

//! \brief A representation of an inbound packet for the state of a robot body
class RobotStatePacket : public BodyStatePacket {
  public:
    //! \brief Construct from an \a id, a \a location, a list of samples for each point, and a \a timestamp
    RobotStatePacket(BodyIdType const& id, DiscreteLocation const& location, List<List<Point>> const& points, TimestampType const& timestamp);
    //! \brief The location
    DiscreteLocation const& location() const;
  private:
    DiscreteLocation const _location;
};

}

#endif //OPERA_PACKET_HPP
