/***************************************************************************
 *            verification.cpp
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

#include "macros.hpp"
#include "verification.hpp"

namespace Opera {

VerificationTraceBase::VerificationTraceBase(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
                                             TimestampType const& initial_time, DiscreteState const& initial_robot_discrete_state)
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

DiscreteState const& VerificationTraceBase::initial_robot_discrete_state() const {
    return _initial_robot_discrete_state;
}

RobotDiscreteTrace const& VerificationTraceBase::robot_discrete_trace() const {
    return _robot_discrete_trace;
}

SimpleVerificationTrace::SimpleVerificationTrace(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
            TimestampType const& initial_time, DiscreteState const& initial_robot_discrete_state)
            : VerificationTraceBase(human_id,human_segment_id,robot_id,robot_segment_id,initial_time,initial_robot_discrete_state) { }

}