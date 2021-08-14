/***************************************************************************
 *            profile_body.cpp
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
#include <ariadne/utility/stopwatch.hpp>

using namespace Opera;

struct Randomiser {
    static FloatType get(double min, double max) {
        return FloatType((max-min)*rand()/RAND_MAX + min,Ariadne::dp);
    }
};

class ProfileBody {
private:
    Ariadne::Stopwatch<Ariadne::Microseconds> sw;
    const unsigned int NUM_TRIES = 1000000;
    Randomiser rnd;
public:
    void profile() {
        profile_bodysegment_intersection();
        profile_bodysegment_state_update();
        profile_history_acquirement_and_update();
    }

    void profile_bodysegment_intersection() {
        FloatType thickness(1.0,Ariadne::dp);
        Body b(0, BodyType::ROBOT, 10, {0,1}, {thickness});
        auto segment = b.segments().at(0);

        auto s1 = segment.create_sample(Point(0, 0, 0), Point(5, 5, 5));
        auto s2 = segment.create_sample(Point(0, 3, 0), Point(6, 6, 6));
        auto s3 = segment.create_sample(Point(0, 8, 0), Point(0, 10, 0));

        sw.restart();
        for (SizeType i=0; i<NUM_TRIES; ++i) s1.intersects(s3);
        sw.click();
        std::cout << "Simple intersections completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;

        sw.restart();
        for (unsigned int i=0; i<NUM_TRIES; ++i) s1.intersects(s2);
        sw.click();
        std::cout << "Complex intersections completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;
    }

    void profile_bodysegment_state_update() {
        FloatType thickness(1.0,Ariadne::dp);
        Body b(0, BodyType::ROBOT, 10, {0,1}, {thickness});
        auto segment = b.segments().at(0);

        auto s = segment.create_sample(Point(0, 0, 0), Point(5, 5, 5));

        Ariadne::List<Point> heads, tails;
        for (SizeType i=0; i<NUM_TRIES; ++i) {
            heads.push_back(Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0)));
            tails.push_back(Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0)));
        }

        sw.restart();
        for (SizeType i=0; i<NUM_TRIES; ++i) s.update(heads.at(i),tails.at(i));
        sw.click();
        std::cout << "States updates completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;
    }

    void profile_history_acquirement_and_update() {
        FloatType thickness(1.0,Ariadne::dp);
        Body b(0, BodyType::ROBOT, 10, {0,1}, {thickness});
        Ariadne::StringVariable robot("robot");
        auto history = b.make_history();

        Ariadne::List<BodyStatePackage> pkgs;
        for (SizeType i=0; i<NUM_TRIES; ++i) {
            pkgs.push_back(BodyStatePackage(DiscreteLocation(robot|"first"),
                                            {{Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0))},
                                                    {Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0))}},
                                                    10*i));
        }

        sw.restart();
        for (auto pkg : pkgs) history.acquire(pkg);
        sw.click();
        std::cout << "Acquired package for new location completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;

        history.acquire(BodyStatePackage(DiscreteLocation(robot|"second"),
                                         {{Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0))},
                                          {Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0))}},
                                          10000010));

        pkgs.clear();
        for (SizeType i=0; i<NUM_TRIES; ++i) {
            pkgs.push_back(BodyStatePackage(DiscreteLocation(robot|"first"),
                                            {{Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0))},
                                             {Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0))}},
                                             10000020+10*i));
        }

        sw.restart();
        for (auto pkg : pkgs) history.acquire(pkg);
        sw.click();
        std::cout << "Acquired package for existing location completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;
    }
};


int main() {
    ProfileBody().profile();
}
