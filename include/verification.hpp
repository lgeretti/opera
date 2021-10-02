/***************************************************************************
 *            verification.hpp
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

#ifndef OPERA_VERIFICATION_HPP
#define OPERA_VERIFICATION_HPP

#include <ariadne/utility/container.hpp>
#include <ariadne/utility/handle.hpp>
#include <ariadne/utility/string.hpp>
#include "barrier.hpp"

namespace Opera {

using IdType = unsigned int;
using BodyIdType = std::string;
using TimestampType = long unsigned int; // Expressed in nanoseconds
using Ariadne::List;
using Ariadne::DiscreteLocation;
using Ariadne::Handle;

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
    virtual DiscreteLocation const& initial_robot_discrete_state() const = 0;
    //! \brief The current discrete trace of the robot
    virtual RobotDiscreteTrace const& robot_discrete_trace() const = 0;
};

//! \brief Base class for implementations
class VerificationTraceBase : public VerificationTraceInterface {
  protected:
    //! \brief Constructor from body and segment coordinates
    VerificationTraceBase(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
                          TimestampType const& initial_time, DiscreteLocation const& initial_robot_discrete_state);
  public:
    //! \brief Interface accessors
    BodyIdType const& human_id() const;
    IdType const& human_segment_id() const;
    BodyIdType const& robot_id() const;
    IdType const& robot_segment_id() const;
    TimestampType const& initial_time() const;
    DiscreteLocation const& initial_robot_discrete_state() const;
    RobotDiscreteTrace const& robot_discrete_trace() const;
  protected:
    BodyIdType const _human_id;
    IdType const _human_segment_id;
    BodyIdType const _robot_id;
    IdType const _robot_segment_id;
    TimestampType const _initial_time;
    DiscreteLocation const _initial_robot_discrete_state;
    RobotDiscreteTrace _robot_discrete_trace;
};

//! \brief A simple trace implementation where we restart from scratch each time
class SimpleVerificationTrace : public VerificationTraceBase {
  public:
    SimpleVerificationTrace(BodyIdType const& human_id, IdType const& human_segment_id, BodyIdType const& robot_id, IdType const& robot_segment_id,
                            TimestampType const& initial_time, DiscreteLocation const& initial_robot_discrete_state);
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
    DiscreteLocation const& initial_robot_discrete_state() const { return _ptr->initial_robot_discrete_state(); }
    RobotDiscreteTrace const& robot_discrete_trace() const { return _ptr->robot_discrete_trace(); }
};

}

#endif //OPERA_VERIFICATION_HPP
