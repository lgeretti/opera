/***************************************************************************
 *            test_scenarios.cpp
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
#include "packet.hpp"
#include "deserialisation.hpp"
#include "barrier.hpp"

using namespace Opera;

class TestScenarios {
  public:
    void test() {
        ARIADNE_TEST_CALL(test_acquire_human_samples())
    }

    void test_acquire_human_samples() {
        SizeType num_samples = 4259;
        Human h0 = BodyDeserialiser(Resources::path("json/scenarios/presentation_human.json")).make_human();

        List<BodyStatePacket> human_packets;
        for (SizeType idx = 1; idx <= num_samples; ++idx)
            human_packets.append(BodyStatePacketDeserialiser(Resources::path("json/scenarios/nocollision/h0/"+std::to_string(idx)+".json")).make());
        ARIADNE_TEST_EQUALS(human_packets.size(),num_samples)
    }

};

int main() {
    TestScenarios().test();
    return ARIADNE_TEST_FAILURES;
}
