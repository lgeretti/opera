/***************************************************************************
 *            profile_verification.hpp
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

#include "verification.hpp"
#include "profile.hpp"

using namespace Opera;

struct ProfileVerification : public Profiler {

    ProfileVerification() : Profiler(1e5) { }

    void run() {
        profile_apply_to_trace();
    }

    void profile_apply_to_trace() {
        Robot r(0, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h(1, 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto hs = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0)).spherical_approximation();

        MinimumDistanceBarrierTrace trace;
        List<BodySegmentSample> rss;
        for (SizeType i=num_tries(); i>0; --i) {
            rss.append(r.segment(0).create_sample(Point(0,5+i,0),Point(2,6+i,0)));
        }

        profile("Apply to trace (decreasing distance)",[&](SizeType i){ trace.apply(hs,rss.at(i)); });

        trace.clear();
        rss.clear();
        for (SizeType i=0; i<num_tries(); ++i) {
            rss.append(r.segment(0).create_sample(Point(4+i,4,0),Point(6+i,4,0)));
        }

        profile("Apply to trace (increasing distance)",[&](SizeType i){ trace.apply(hs,rss.at(i)); });
    }
};

int main() {
    ProfileVerification().run();
}
