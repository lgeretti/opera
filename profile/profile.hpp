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

#include <functional>
#include <iostream>
#include <ariadne/utility/stopwatch.hpp>
#include "declarations.hpp"

using namespace Opera;

struct Randomiser {
    static FloatType get(double min, double max) {
        return (max-min)*rand()/RAND_MAX + min;
    }
};

inline bool _init_randomiser() {
    srand(time(nullptr));
    return true;
}
static const bool init_randomiser = _init_randomiser();

class Profiler {
  public:
    Profiler(SizeType num_tries) : _num_tries(num_tries) { }

    SizeType num_tries() const { return _num_tries; }

    Randomiser const& rnd() const { return _rnd; }

    void profile(std::string msg, std::function<void(SizeType)> function, SizeType num_tries) {
        _sw.restart();
        for (SizeType i=0; i<num_tries; ++i) function(i);
        _sw.click();
        std::cout << msg << " completed in " << ((double)_sw.duration().count())/num_tries*1000 << " ns on average" << std::endl;
    }

    void profile(std::string msg, std::function<void(SizeType)> function) {
        profile(msg,function,_num_tries);
    }

  private:
    Ariadne::Stopwatch<Ariadne::Microseconds> _sw;
    Randomiser _rnd;
    const SizeType _num_tries;
};
