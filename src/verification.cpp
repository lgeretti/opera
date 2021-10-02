/***************************************************************************
 *            verification.cpp
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

#include "macros.hpp"
#include "verification.hpp"

namespace Opera {

VerificationTraceBase::VerificationTraceBase(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
                                             TimestampType const& initial_time, DiscreteLocation const& initial_robot_discrete_state)
    : _human_id(human_id), _human_segment_id(human_segment_id), _robot_id(robot_id), _robot_segment_id(robot_segment_id), _initial_time(initial_time),
      _initial_robot_discrete_state(initial_robot_discrete_state) { }

//! \brief Interface accessors
BodyIdType const& VerificationTraceBase::human_id() const {
    return _human_id;
}

IdType const& VerificationTraceBase::human_segment_id() const {
    return _human_segment_id;
}

BodyIdType const& VerificationTraceBase::robot_id() const {
    return _robot_id;
}

IdType const& VerificationTraceBase::robot_segment_id() const {
    return _robot_segment_id;
}

TimestampType const& VerificationTraceBase::initial_time() const {
    return _initial_time;
}

DiscreteLocation const& VerificationTraceBase::initial_robot_discrete_state() const {
    return _initial_robot_discrete_state;
}

RobotDiscreteTrace const& VerificationTraceBase::robot_discrete_trace() const {
    return _robot_discrete_trace;
}

SimpleVerificationTrace::SimpleVerificationTrace(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
            TimestampType const& initial_time, DiscreteLocation const& initial_robot_discrete_state)
            : VerificationTraceBase(human_id,human_segment_id,robot_id,robot_segment_id,initial_time,initial_robot_discrete_state) { }

}