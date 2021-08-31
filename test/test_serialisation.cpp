/***************************************************************************
 *            test_serialisation.cpp
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

#include "test.hpp"
#include "utility.hpp"
#include "serialisation.hpp"

using namespace Opera;

class TestSerialisation {
public:
    void test() {
        ARIADNE_TEST_CALL(test_bodyserialiser_from_human())
        ARIADNE_TEST_CALL(test_bodyserialiser_from_robot())
    }

    void test_bodyserialiser_from_human() {
        Human h("human1", {0, 1, 3, 2}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)});
        BodySerialiser(FilePath(Resources::path("json/presentation/"+h.id()+".tmp.json").c_str())).serialise(h);
    }

    void test_bodyserialiser_from_robot() {
        Robot r("robot1", 30, {0, 1, 3, 2, 4, 2}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)});
        BodySerialiser(FilePath(Resources::path("json/presentation/"+r.id()+".tmp.json").c_str())).serialise(r);
    }
};


int main() {
    TestSerialisation().test();

    return ARIADNE_TEST_FAILURES;
}
