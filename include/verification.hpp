/***************************************************************************
 *            verification.hpp
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

#ifndef OPERA_VERIFICATION_HPP
#define OPERA_VERIFICATION_HPP

#include "handle.hpp"
#include "barrier.hpp"
#include "discrete_state.hpp"

namespace Opera {

using IdType = unsigned int;
using BodyIdType = std::string;
using TimestampType = long unsigned int; // Expressed in nanoseconds

//! \brief Interface for a trace of verification data
class VerificationTraceInterface {
  public:
    //! \brief The human identifier
    virtual BodyIdType const& human_id() const = 0;
    //! \brief The human segment identifier
    virtual IdType const& human_segment_id() const = 0;
    //! \brief The robot identifier
    virtual BodyIdType const& robot_id() const = 0;
    //! \brief The robot segment identifier
    virtual IdType const& robot_segment_id() const = 0;
    //! \brief The time from which verification would start
    virtual TimestampType const& initial_time() const = 0;
    //! \brief The location from which verification would start
    virtual DiscreteState const& initial_robot_discrete_state() const = 0;
    //! \brief The current discrete trace of the robot
    virtual RobotDiscreteTrace const& robot_discrete_trace() const = 0;
};

//! \brief Base class for implementations
class VerificationTraceBase : public VerificationTraceInterface {
  protected:
    //! \brief Constructor from body and segment coordinates
    VerificationTraceBase(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
                          TimestampType const& initial_time, DiscreteState const& initial_robot_discrete_state);
  public:
    //! \brief Interface accessors
    BodyIdType const& human_id() const;
    IdType const& human_segment_id() const;
    BodyIdType const& robot_id() const;
    IdType const& robot_segment_id() const;
    TimestampType const& initial_time() const;
    DiscreteState const& initial_robot_discrete_state() const;
    RobotDiscreteTrace const& robot_discrete_trace() const;
  protected:
    BodyIdType const _human_id;
    IdType const _human_segment_id;
    BodyIdType const _robot_id;
    IdType const _robot_segment_id;
    TimestampType const _initial_time;
    DiscreteState const _initial_robot_discrete_state;
    RobotDiscreteTrace _robot_discrete_trace;
};

//! \brief A simple trace implementation where we restart from scratch each time
class SimpleVerificationTrace : public VerificationTraceBase {
  public:
    SimpleVerificationTrace(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
                            TimestampType const& initial_time, DiscreteState const& initial_robot_discrete_state);
};

//! \brief Handle class for a verification trace
class VerificationTrace : public Handle<VerificationTraceInterface> {
  public:
    using Handle<VerificationTraceInterface>::Handle;

    BodyIdType const& human_id() const { return _ptr->human_id(); }
    IdType const& human_segment_id() const { return _ptr->human_segment_id(); }
    BodyIdType const& robot_id() const { return _ptr->robot_id(); }
    IdType const& robot_segment_id() const { return _ptr->robot_segment_id(); }
    TimestampType const& initial_time() const { return _ptr->initial_time(); }
    DiscreteState const& initial_robot_discrete_state() const { return _ptr->initial_robot_discrete_state(); }
    RobotDiscreteTrace const& robot_discrete_trace() const { return _ptr->robot_discrete_trace(); }
};

}

#endif //OPERA_VERIFICATION_HPP
