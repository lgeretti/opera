/***************************************************************************
 *            profile_algebra.cpp
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

#include "algebra3d.hpp"
#include <ariadne/utility/stopwatch.hpp>

using namespace Opera;

class ProfileAlgebra {
private:
    Ariadne::Stopwatch<Ariadne::Microseconds> sw;
    const unsigned int NUM_TRIES = 1000000;
public:
    void profile() {
        profile_center();
    }

    void profile_center() {
        Point p1(1.0,3.0,-2.0);
        Point p2(4.0,1.2,0);
        sw.restart();
        for (unsigned int i=0; i<NUM_TRIES; ++i) {
            center(p1,p2);
        }
        sw.click();
        std::cout << "Center completed in " << ((double)sw.duration().count())/NUM_TRIES*1000 << " ns on average" << std::endl;
    }
};


int main() {
    ProfileAlgebra().profile();
}
