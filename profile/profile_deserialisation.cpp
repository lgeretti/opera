/***************************************************************************
 *            profile_deserialisation.cpp
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

#include "profile.hpp"

#include "utility.hpp"
#include "packet.hpp"
#include "deserialisation.hpp"

using namespace Opera;

class ProfileDeserialisation : public Profiler {
  public:
    ProfileDeserialisation() : Profiler(1e5) { }

    void run() {
        profile_scenario_samples();
    }

    void profile_scenario_samples() {
        SizeType num_samples = 4259;
        profile("Deserialisation of a human sample JSON file into BodyStatePacket",[&](SizeType i){
            BodyStatePacketDeserialiser(Resources::path("json/scenarios/nocollision/h0/"+std::to_string(i+1)+".json")).make();
        },num_samples);

        List<String> json_texts;
        profile("Deserialisation of a human sample JSON file into String",[&](SizeType i){
            json_texts.append(BodyStatePacketDeserialiser(Resources::path("json/scenarios/nocollision/h0/"+std::to_string(i+1)+".json")).to_string());
        },num_samples);

        profile("Deserialisation of a human sample JSON String into BodyStatePacket",[&](SizeType i){
            BodyStatePacketDeserialiser(json_texts.at(i).c_str()).make();
        },num_samples);
    }

};

int main() {
    ProfileDeserialisation().run();
}
