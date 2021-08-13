/***************************************************************************
 *            profile_geometry.cpp
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

#include "geometry.hpp"
#include <ariadne/utility/stopwatch.hpp>

using namespace Opera;
using Ariadne::SizeType;

struct Randomiser {
    static FloatType get(double min, double max) {
        return FloatType((max-min)*rand()/RAND_MAX + min,Ariadne::dp);
    }
};

class ProfileGeometry {
private:
    Ariadne::Stopwatch<Ariadne::Microseconds> sw;
    const unsigned int NUM_TRIES = 1000000;
    Randomiser rnd;
public:
    void profile() {
        profile_center();
        profile_distance();
    }

    void profile_center() {
        Ariadne::List<Point> heads, tails;
        for (SizeType i=0; i<NUM_TRIES; ++i) {
            heads.push_back(Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0)));
            tails.push_back(Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0)));
        }
        sw.restart();
        for (SizeType i=0; i<NUM_TRIES; ++i) {
            center(heads.at(i),tails.at(i));
        }
        sw.click();
        std::cout << "Center completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;
    }

    void profile_distance() {
        Point s1h(1.0,3.0,-2.0);
        Point s1t(4.0,1.2,0);
        Ariadne::List<Point> heads, tails;
        for (SizeType i=0; i<NUM_TRIES; ++i) {
            heads.push_back(Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0)));
            tails.push_back(Point(rnd.get(-5.0,5.0),rnd.get(-5.0,5.0),rnd.get(-5.0,5.0)));
        }

        sw.restart();
        for (SizeType i=0; i<NUM_TRIES; ++i) {
            distance(s1h,s1t,heads.at(i),tails.at(i));
        }
        sw.click();
        std::cout << "Distance completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;
    }
};


int main() {
    ProfileGeometry().profile();
}
