/***************************************************************************
 *            test_deserialisation.cpp
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
#include "deserialisation.hpp"

using namespace Opera;

class TestDeserialisation {
public:
    void test() {
        ARIADNE_TEST_CALL(test_bodydeserialiser_create())
        ARIADNE_TEST_CALL(test_bodydeserialiser_make_human())
        ARIADNE_TEST_CALL(test_bodydeserialiser_make_robot())
    }

    void test_bodydeserialiser_create() {
        BodyDeserialiser d(Resources::path("json/presentation/human0.json"));
        ARIADNE_TEST_ASSERT(d.is_human())
        ARIADNE_TEST_PRINT(d)
    }

    void test_bodydeserialiser_make_human() {
        BodyDeserialiser d(Resources::path("json/presentation/human0.json"));
        ARIADNE_TEST_FAIL(d.make_robot())
        Human h = d.make_human();
        ARIADNE_TEST_PRINT(h)
    }
    void test_bodydeserialiser_make_robot() {
        BodyDeserialiser d(Resources::path("json/presentation/robot0.json"));
        ARIADNE_TEST_FAIL(d.make_human())
        Robot r = d.make_robot();
        ARIADNE_TEST_PRINT(r)
    }
};


int main() {
    TestDeserialisation().test();

    return ARIADNE_TEST_FAILURES;
}
