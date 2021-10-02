/***************************************************************************
 *            profile_state.hpp
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

#include "state.hpp"
#include "packet.hpp"
#include "profile.hpp"

using namespace Opera;

struct ProfileState : public Profiler {

    ProfileState() : Profiler(1e5) { }

    void run() {
        profile_human_instance_acquirement();
        profile_robot_history_acquirement_and_update();
    }

    void profile_human_instance_acquirement() {
        FloatType thickness = 1.0;
        Human h("h0", {{0, 1}}, {thickness});

        Ariadne::List<BodyStatePacket> pkts;
        for (SizeType i=0; i<num_tries(); ++i) {
            pkts.push_back(BodyStatePacket(h.id(),{{Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))},
                                              {Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))}},
                                              10*i));
        }

        profile("Make human state instance from packet fields",[&](SizeType i){ HumanStateInstance hsi(h,pkts.at(i).points(),pkts.at(i).timestamp()); });
    }

    void profile_robot_history_acquirement_and_update() {
        FloatType thickness = 1.0;
        Robot r("r0", 10, {{0, 1}}, {thickness});
        Ariadne::StringVariable robot("robot");
        RobotStateHistory history(&r);

        Ariadne::List<BodyStatePacket> pkts;
        for (SizeType i=0; i<num_tries(); ++i) {
            pkts.push_back(BodyStatePacket(r.id(),DiscreteLocation(robot|"first"),
                                            {{Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))},
                                                    {Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))}},
                                                    10*i));
        }

        profile("Acquire robot packet for new location",[&](SizeType i){ history.acquire(pkts.at(i).location(),pkts.at(i).points(),pkts.at(i).timestamp()); });

        history.acquire(DiscreteLocation(robot|"second"),
                                         {{Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))},
                                          {Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))}},
                                          10000010);

        pkts.clear();
        for (SizeType i=0; i<num_tries(); ++i) {
            pkts.push_back(BodyStatePacket(r.id(),DiscreteLocation(robot|"first"),
                                            {{Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))},
                                             {Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0))}},
                                             10000020+10*i));
        }

        profile("Acquire robot packet for existing location",[&](SizeType i){ history.acquire(pkts.at(i).location(),pkts.at(i).points(),pkts.at(i).timestamp()); });
    }
};

int main() {
    ProfileState().run();
}
