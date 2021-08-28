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

#include "serialisation.hpp"

using namespace Opera;

class TestSerialisation {
public:
    void test() {
        ARIADNE_TEST_CALL(test_bodydeserialiser_create())
    }

    void test_bodydeserialiser_create() {
        BodyDeserialiser s("../resources/json/presentation/human0.json");
    }

};


int main() {
    TestSerialisation().test();

    return ARIADNE_TEST_FAILURES;
}
