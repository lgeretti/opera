/***************************************************************************
 *            macros.hpp
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

#ifndef OPERA_MACROS_HPP
#define OPERA_MACROS_HPP

#include <sstream>
#include <stdexcept>

using StringStream = std::stringstream;

#define OPERA_USING_CONSTRUCTORS(Class,Base) \
    template<class T,typename std::enable_if<std::is_convertible<T,Base>::value,int>::type=0> \
    Class(const T& t) : Base(t) { } \
    template<class T,typename std::enable_if<std::is_constructible<T,Base>::value and not std::is_convertible<T,Base>::value,int>::type=0> \
    explicit Class(const T& t) : Base(t) { } \
    template<class ...Args> Class(Args&&... args) : Base(std::forward<Args>(args)...) { } \

#define OPERA_THROW(except,func,msg)          \
    { \
        StringStream ss; \
        ss << #except " in " << func << ": " << msg;    \
        throw except(ss.str()); \
    } \

#define OPERA_THROW_RTE(error) \
    { \
        OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<__FUNCTION__,error); \
    } \

#define OPERA_ASSERT(expression) \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<__FUNCTION__,"Assertion `" << #expression << "' failed."); \
        } \
    } \


#ifndef NDEBUG
#define OPERA_DEBUG_ASSERT_MSG(expression,error) \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<OPERA_PRETTY_FUNCTION,"Assertion `" << #expression << "' failed.\n"<<"  "<<error); \
        } \
    } \

#else
#define OPERA_DEBUG_ASSERT_MSG(expression,error) \
    { }
#endif


#ifndef NDEBUG
#define OPERA_DEBUG_ASSERT(expression) \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<__FUNCTION__,"Assertion `" << #expression << "' failed."); \
        } \
    } \

#else
#define OPERA_DEBUG_ASSERT(expression) \
    { }
#endif


#define OPERA_PRECONDITION_MSG(expression,error)             \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<OPERA_PRETTY_FUNCTION,"Precondition `" << #expression << "' failed.\n"<<"  "<<error); \
        } \
    } \

#define OPERA_PRECONDITION(expression)             \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<OPERA_PRETTY_FUNCTION,"Precondition `" << #expression << "' failed."); \
        } \
    } \

#ifndef NDEBUG
#define OPERA_DEBUG_PRECONDITION(expression) \
    { \
        bool result = static_cast<bool>(expression); \
        if(!result) { \
            OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<__FUNCTION__,"Precondition `" << #expression << "' failed."); \
        } \
    } \

#else
#define OPERA_DEBUG_PRECONDITION(expression) \
    { }
#endif

#define OPERA_FAIL_MSG(error)             \
    { \
        OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<OPERA_PRETTY_FUNCTION,"ErrorTag "<<error); \
    } \

#define OPERA_ASSERT_MSG(expression,error)             \
    { \
        bool assertion_result = static_cast<bool>(expression); \
        if(!assertion_result) { \
            OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<OPERA_PRETTY_FUNCTION,"Assertion `" << #expression << "' failed.\n"<<"  "<<error); \
        } \
    } \

#define OPERA_ASSERT_EQUAL(expression1,expression2)    \
    { \
        bool assertion_result = static_cast<bool>((expression1) == (expression2));       \
        if(!assertion_result) { \
            OPERA_THROW(std::runtime_error,__FILE__<<":"<<__LINE__<<": "<<OPERA_PRETTY_FUNCTION,"Assertion `" << #expression1 << "==" << #expression2 << "' failed.\n"<<"  "<<expression1<<" != "<<expression2); \
        } \
    } \

#define OPERA_NOT_IMPLEMENTED                 \
    throw std::runtime_error(StringType("Not implemented: ")+OPERA_PRETTY_FUNCTION);

#define OPERA_DEPRECATED(fn,msg)          \
    static bool first_time=true; \
    if(first_time) { \
        first_time=false; \
        std::cerr << "DEPRECATED: Function " << #fn << " is deprecated. " << #msg << std::endl; \
    } \

#define OPERA_NOTIFY(msg)          \
    {                                                                \
        std::cerr << "NOTIFICATION: " << msg << "" << std::endl;                \
    }

#define OPERA_WARN(msg)          \
    {                                                                \
        std::cerr << "WARNING: " << msg << "" << std::endl;                \
    }

#define OPERA_WARN_ONCE(msg)          \
    static bool first_time=true; \
    if(first_time) { \
        first_time=false; \
        std::cerr << "WARNING: " << msg << "" << std::endl; \
    } \

#define OPERA_ERROR(msg)          \
    {                                                                \
        std::cerr << "ERROR: " << msg << "" << std::endl;                \
    }
                                                                  \
#if defined(linux) || defined(__linux) || defined(__linux__)
#define OPERA_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define OPERA_PRETTY_FUNCTION __FUNCTION__
#elif defined(darwin) || defined(__darwin) || defined(__darwin__) || defined(__APPLE__)
#define OPERA_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
#define OPERA_PRETTY_FUNCTION ""
#endif

#endif // OPERA_MACROS_HPP
