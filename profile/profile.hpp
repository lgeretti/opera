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
#include <ariadne/utility/stopwatch.hpp>
#include <ariadne/numeric/float_approximation.hpp>

typedef Ariadne::FloatDPApproximation FloatType;
using Ariadne::SizeType;

struct Randomiser {
    static FloatType get(double min, double max) {
        return FloatType((max-min)*rand()/RAND_MAX + min,Ariadne::dp);
    }
};

class Profiler {
  public:
    Profiler(SizeType num_tries) : _num_tries(num_tries) { }

    SizeType num_tries() const { return _num_tries; }

    Randomiser const& rnd() const { return _rnd; }

    void profile(std::string msg, std::function<void(SizeType)> function) {
        _sw.restart();
        for (SizeType i=0; i<_num_tries; ++i) function(i);
        _sw.click();
        std::cout << msg << " completed in " << ((double)_sw.duration().count())/_num_tries*1000 << " ns on average" << std::endl;
    }
  private:
    Ariadne::Stopwatch<Ariadne::Microseconds> _sw;
    Randomiser _rnd;
    const SizeType _num_tries;
};
