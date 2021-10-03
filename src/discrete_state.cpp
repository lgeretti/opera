/***************************************************************************
 *            discrete_state.cpp
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

#include "discrete_state.hpp"
#include "macros.hpp"

namespace Opera {

DiscreteState::DiscreteState(const Map<String,String>& sm) : _mapping(sm) { }

DiscreteState::DiscreteState(Pair<String,String> const& pair) : DiscreteState({pair}) { }

DiscreteState::DiscreteState(std::initializer_list<Pair<String,String>> const& vals) {
    for (auto v : vals) {
        _mapping.insert(std::make_pair(v.first,v.second));
    }
}

bool DiscreteState::is_empty() const {
    return _mapping.empty();
}

Map<String,String> const& DiscreteState::values() const {
    return _mapping;
}

bool operator==(const DiscreteState& q1, const DiscreteState& q2) {
    bool identical=true;
    const Map<String,String>& q1sm=q1._mapping;
    const Map<String,String>& q2sm=q2._mapping;
    Map<String,String>::const_iterator q1iter=q1sm.begin();
    Map<String,String>::const_iterator q2iter=q2sm.begin();

    while(q1iter!=q1sm.end() && q2iter!=q2sm.end()) {
        if(q1iter->first==q2iter->first) {
            if(q1iter->second != q2iter->second) { return false; }
            ++q1iter; ++q2iter;
        } else if(q1iter->first<q2iter->first) {
            identical=false; ++q1iter;
        } else {
            identical=false; ++q2iter;
        }
    }
    if(q1iter!=q1sm.end() || q2iter!=q2sm.end()) { identical=false; }
    OPERA_ASSERT_MSG(identical,"Locations "<<q1<<" and "<<q2<<" have different key values, hence they are not comparable.");
    return true;
}

bool operator!=(const DiscreteState& q1, const DiscreteState& q2) {
    return !(q1==q2);
}

bool operator<(const DiscreteState& q1, const DiscreteState& q2) {
    const Map<String,String>& q1sm=q1._mapping;
    const Map<String,String>& q2sm=q2._mapping;
    Map<String,String>::const_iterator q1iter=q1sm.begin();
    Map<String,String>::const_iterator q2iter=q2sm.begin();

    while(q1iter!=q1sm.end() && q2iter!=q2sm.end()) {
        if(q1iter->first!=q2iter->first) {
            return (q1iter->first<q2iter->first);
        } else if(q1iter->second != q2iter->second) {
            return q1iter->second < q2iter->second;
        } else {
            ++q1iter; ++q2iter;
        }
    }
    return q2iter!=q2sm.end();
}

std::ostream& operator<<(std::ostream& os, DiscreteState const& s) {
    if (s._mapping.empty()) return os << "{}";
    Map<String,String>::const_iterator it=s._mapping.begin();
    os << "{" << it->first << "|" << it->second;
    while (it != s._mapping.end()) {
        os << "," << it->first << "|" << it->second;
        ++it;
    }
    return os << "}";
}


} //namespace Opera
