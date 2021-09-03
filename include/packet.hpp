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
using Ariadne::Pair;
using Ariadne::DiscreteLocation;
using Ariadne::SizeType;
using Ariadne::String;

//! \brief A representation of an inbound packet for the presentation of a body
class BodyPresentationPacket {
  public:
    //! \brief Construct for a human
    BodyPresentationPacket(BodyIdType const& id, List<Pair<IdType,IdType>> const& point_ids, List<FloatType> const& thicknesses);
    //! \brief Construct for a robot
    BodyPresentationPacket(BodyIdType const& id, SizeType const& packet_frequency, List<Pair<IdType,IdType>> const& point_ids, List<FloatType> const& thicknesses);
    //! \brief The id of the related body
    BodyIdType const& id() const;
    //! \brief Whether this is a human
    bool const& is_human() const;
    //! \brief The packet sending frequency in Hz
    //! \details We use 0 when not known (i.e., for a human)
    SizeType const& packet_frequency() const;
    //! \brief The points for each segment
    List<Pair<IdType,IdType>> const& point_ids() const;
    //! \brief The thicknesses for each segment
    List<FloatType> const& thicknesses() const;
  private:
    BodyIdType const _id;
    bool const _is_human;
    SizeType const _packet_frequency;
    List<Pair<IdType,IdType>> const _point_ids;
    List<FloatType> const _thicknesses;
};

//! \brief A representation of an inbound packet for the state of a body
class BodyStatePacket {
  public:
    //! \brief Construct from an id, a location, a list of samples for each point, and a \a timestamp
    BodyStatePacket(BodyIdType const& id, DiscreteLocation const& location, List<List<Point>> const& points, TimestampType const& timestamp);
    //! \brief Construct without a location
    BodyStatePacket(BodyIdType const& id, List<List<Point>> const& points, TimestampType const& timestamp);
    //! \brief The id of the related body
    BodyIdType const& id() const;
    //! \brief The location
    DiscreteLocation const& location() const;
    //! \brief The samples for each point
    List<List<Point>> const& points() const;
    //! \brief The timestamp associated with the packet
    TimestampType const& timestamp() const;
  private:
    BodyIdType const _id;
    DiscreteLocation const _location;
    List<List<Point>> const _points;
    TimestampType const _timestamp;
};

//! \brief A representation of an outbound packet for a detected collision
class CollisionNotificationPacket {
  public:
    //! \brief Construct from fields
    CollisionNotificationPacket(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
                                DiscreteLocation const& discrete_state, TimestampType const& lower_collision_time, TimestampType const& upper_collision_time);

    //! \brief The identifier of the human
    BodyIdType const& human_id() const;
    //! \brief The identifier of the segment for the human
    IdType const& human_segment_id() const;
    //! \brief The identifier of the robot
    BodyIdType const& robot_id() const;
    //! \brief The identifier of the segment for the robot
    IdType const& robot_segment_id() const;
    //! \brief The discrete state of the robot
    DiscreteLocation const& discrete_state() const;
    //! \brief The lower bound on the collision time
    TimestampType const& lower_collision_time() const;
    //! \brief The upper bound on the collision time
    TimestampType const& upper_collision_time() const;

  private:
    BodyIdType const _human_id;
    IdType const _human_segment_id;
    BodyIdType const _robot_id;
    IdType const _robot_segment_id;
    DiscreteLocation const _discrete_state;
    TimestampType const _lower_collision_time;
    TimestampType const _upper_collision_time;
};

}

#endif //OPERA_PACKET_HPP
