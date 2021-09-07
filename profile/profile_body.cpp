/***************************************************************************
 *            profile_body.hpp
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

#include "body.hpp"
#include "profile.hpp"

using namespace Opera;

struct ProfileBody : public Profiler {

    ProfileBody() : Profiler(1e5) { }

    void run() {
        profile_bodysegment_intersection();
        profile_bodysegment_sample_update();
    }

    void profile_bodysegment_intersection() {
        FloatType thickness(1.0,Ariadne::dp);
        Robot r("r0", 10, {{0, 1}}, {thickness});
        auto segment = r.segment(0);

        auto s1 = segment.create_sample();
        s1.update({Point(0, 0, 0)},{Point(5, 5, 5)});
        auto s2 = segment.create_sample();
        s2.update({Point(0, 3, 0)},{Point(6, 6, 6)});
        auto s3 = segment.create_sample();
        s3.update({Point(0, 8, 0)},{Point(0, 10, 0)});

        profile("Box intersection",[&](SizeType i){ s1.intersects(s3); });
        profile("Pill intersection",[&](SizeType i){ s1.intersects(s2); });
    }

    void profile_bodysegment_sample_update() {
        FloatType thickness(1.0,Ariadne::dp);
        Robot r("r0", 10, {{0, 1}}, {thickness});
        auto segment = r.segment(0);

        auto s = segment.create_sample();
        s.update({Point(0, 0, 0)},{Point(5, 5, 5)});

        Ariadne::List<Point> heads, tails;
        for (SizeType i=0; i<num_tries(); ++i) {
            heads.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            tails.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Sample update",[&](SizeType i){ s.update(heads.at(i), tails.at(i)); });
    }
};

int main() {
    ProfileBody().run();
}
