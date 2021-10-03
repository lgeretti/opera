/***************************************************************************
 *            profile_body.cpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of Opera, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <functional>
#include <iostream>
#include "stopwatch.hpp"
#include "declarations.hpp"

using namespace Opera;

struct Randomiser {
    static FloatType get(double min, double max) {
        return (max-min)*rand()/RAND_MAX + min;
    }
};

inline bool _init_randomiser() {
    srand(static_cast<unsigned int>(time(nullptr)));
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
    Stopwatch<Microseconds> _sw;
    Randomiser _rnd;
    const SizeType _num_tries;
};
