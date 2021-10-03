/***************************************************************************
 *            packet.hpp
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

#ifndef OPERA_PACKET_HPP
#define OPERA_PACKET_HPP

#include "handle.hpp"
#include "geometry.hpp"
#include "discrete_state.hpp"

namespace Opera {

using IdType = unsigned int;
using BodyIdType = std::string;

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
    BodyStatePacket(BodyIdType const& id, DiscreteState const& location, List<List<Point>> const& points, TimestampType const& timestamp);
    //! \brief Construct without a location
    BodyStatePacket(BodyIdType const& id, List<List<Point>> const& points, TimestampType const& timestamp);
    //! \brief The id of the related body
    BodyIdType const& id() const;
    //! \brief The location
    DiscreteState const& location() const;
    //! \brief The samples for each point
    List<List<Point>> const& points() const;
    //! \brief The timestamp associated with the packet
    TimestampType const& timestamp() const;
  private:
    BodyIdType const _id;
    DiscreteState const _location;
    List<List<Point>> const _points;
    TimestampType const _timestamp;
};

//! \brief A representation of an outbound packet for a detected collision
class CollisionNotificationPacket {
  public:
    //! \brief Construct from fields
    CollisionNotificationPacket(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
                                DiscreteState const& discrete_state, TimestampType const& lower_collision_time, TimestampType const& upper_collision_time,
                                PositiveFloatType const& likelihood);

    //! \brief The identifier of the human
    BodyIdType const& human_id() const;
    //! \brief The identifier of the segment for the human
    IdType const& human_segment_id() const;
    //! \brief The identifier of the robot
    BodyIdType const& robot_id() const;
    //! \brief The identifier of the segment for the robot
    IdType const& robot_segment_id() const;
    //! \brief The discrete state of the robot
    DiscreteState const& discrete_state() const;
    //! \brief The lower bound on the collision time
    TimestampType const& lower_collision_time() const;
    //! \brief The upper bound on the collision time
    TimestampType const& upper_collision_time() const;
    //! \brief The probability that this collision will happen
    PositiveFloatType const& likelihood() const;

  private:
    BodyIdType const _human_id;
    IdType const _human_segment_id;
    BodyIdType const _robot_id;
    IdType const _robot_segment_id;
    DiscreteState const _discrete_state;
    TimestampType const _lower_collision_time;
    TimestampType const _upper_collision_time;
    PositiveFloatType const _likelihood;
};

}

#endif //OPERA_PACKET_HPP
