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
        profile_human_instance_acquirement();
        profile_robot_history_acquirement_and_update();
    }

    void profile_bodysegment_intersection() {
        FloatType thickness(1.0,Ariadne::dp);
        Robot r(0, 10, {0, 1}, {thickness});
        auto segment = r.segment(0);

        auto s1 = segment.create_sample(Point(0, 0, 0), Point(5, 5, 5));
        auto s2 = segment.create_sample(Point(0, 3, 0), Point(6, 6, 6));
        auto s3 = segment.create_sample(Point(0, 8, 0), Point(0, 10, 0));

        profile("Box intersection",[s1,s3](SizeType i){ s1.intersects(s3); });
        profile("Pill intersection",[s1,s2](SizeType i){ s1.intersects(s2); });
    }

    void profile_bodysegment_sample_update() {
        FloatType thickness(1.0,Ariadne::dp);
        Robot r(0, 10, {0, 1}, {thickness});
        auto segment = r.segment(0);

        auto s = segment.create_sample(Point(0, 0, 0), Point(5, 5, 5));

        Ariadne::List<Point> heads, tails;
        for (SizeType i=0; i<num_tries(); ++i) {
            heads.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            tails.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Sample update",[&s,heads,tails](SizeType i){ s.update(heads.at(i), tails.at(i)); });
    }

    void profile_human_instance_acquirement() {
        FloatType thickness(1.0,Ariadne::dp);
        Human h(0, 10, {0, 1}, {thickness});

        Ariadne::List<HumanStatePackage> pkgs;
        for (SizeType i=0; i<num_tries(); ++i) {
            pkgs.push_back(HumanStatePackage({{Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))},
                                              {Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))}},
                                              10*i));
        }

        profile("Make human state instance from package",[&h,pkgs](SizeType i){ h.make_instance(pkgs.at(i)); });
    }

    void profile_robot_history_acquirement_and_update() {
        FloatType thickness(1.0,Ariadne::dp);
        Robot r(0, 10, {0, 1}, {thickness});
        Ariadne::StringVariable robot("robot");
        auto history = r.make_history();

        Ariadne::List<RobotStatePackage> pkgs;
        for (SizeType i=0; i<num_tries(); ++i) {
            pkgs.push_back(RobotStatePackage(DiscreteLocation(robot|"first"),
                                            {{Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))},
                                                    {Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))}},
                                                    10*i));
        }

        profile("Acquire robot package for new location",[&history,pkgs](SizeType i){ history.acquire(pkgs.at(i)); });

        history.acquire(RobotStatePackage(DiscreteLocation(robot|"second"),
                                         {{Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))},
                                          {Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))}},
                                          10000010));

        pkgs.clear();
        for (SizeType i=0; i<num_tries(); ++i) {
            pkgs.push_back(RobotStatePackage(DiscreteLocation(robot|"first"),
                                            {{Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))},
                                             {Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))}},
                                             10000020+10*i));
        }

        profile("Acquire robot package for existing location",[&history,pkgs](SizeType i){ history.acquire(pkgs.at(i)); });
    }
};

int main() {
    ProfileBody().run();
}
