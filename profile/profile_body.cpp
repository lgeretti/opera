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

class ProfileBody {
private:
    Ariadne::Stopwatch<Ariadne::Microseconds> sw;
    const unsigned int NUM_TRIES = 1000000;
public:
    void profile() {
        profile_bodysegment_intersection();
    }

    void profile_bodysegment_intersection() {
        FloatType thickness(1.0,Ariadne::dp);
        BodySegment* segment = new BodySegment(0,1,thickness);

        BodySegmentState s1(segment, Point(0, 0, 0), Point(5, 5, 5), 32490234);
        BodySegmentState s2(segment, Point(0, 3, 0), Point(5, 5, 5), 32490234);
        BodySegmentState s7(segment, Point(0, 8, 0), Point(0, 10, 0), 32490234);

        sw.restart();
        for (unsigned int i=0; i<NUM_TRIES; ++i) {
            s1.intersects(s7);
        }
        sw.click();
        std::cout << "Simple intersections completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;

        sw.restart();
        for (unsigned int i=0; i<NUM_TRIES; ++i) {
            s1.intersects(s2);
        }
        sw.click();
        std::cout << "Complex intersections completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;

        delete segment;
    }
};


int main() {
    ProfileBody().profile();
}
