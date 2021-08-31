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
        BodyDeserialiser d(FilePath(Resources::path("json/presentation/human0.json").c_str()));
        ARIADNE_TEST_ASSERT(d.is_human())
        ARIADNE_TEST_PRINT(d)
    }

    void test_bodydeserialiser_make_human() {
        BodyDeserialiser d1(FilePath(Resources::path("json/presentation/human0.json").c_str()));
        ARIADNE_TEST_FAIL(d1.make_robot())
        auto h1 = d1.make_human();
        ARIADNE_TEST_PRINT(h1)
        BodyDeserialiser d2(String("{\n"
                                   "  \"id\": \"h0\",\n"
                                   "  \"isHuman\": true,\n"
                                   "  \"pointIds\": [[14,12],[11,12],[5,7],[6,8],[7,9],[8,10],[1,2],[0,1],[0,2],[1,3],[2,4],[3,5],[4,6],[17,0],[17,5],[17,6],[17,11],[17,12]],\n"
                                   "  \"thicknesses\": [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]\n"
                                   "}"));
        auto h2 = d2.make_human();
        ARIADNE_TEST_EQUALS(h1.id(),h2.id())
    }
    void test_bodydeserialiser_make_robot() {
        BodyDeserialiser d1(FilePath(Resources::path("json/presentation/robot0.json").c_str()));
        ARIADNE_TEST_FAIL(d1.make_human())
        auto r1 = d1.make_robot();
        ARIADNE_TEST_PRINT(r1)
        BodyDeserialiser d2(String("{\n"
                            "  \"id\": \"r0\",\n"
                            "  \"isHuman\": false,\n"
                            "  \"packageFrequency\": 10,\n"
                            "  \"pointIds\": [[0,1],[1,2],[2,3],[3,4],[4,5],[5,6],[6,7]],\n"
                            "  \"thicknesses\": [1,1,1,1,1,1,1]\n"
                            "}"));
        auto r2 = d2.make_robot();
        ARIADNE_TEST_EQUALS(r1.id(),r2.id())
    }
};


int main() {
    TestDeserialisation().test();

    return ARIADNE_TEST_FAILURES;
}
