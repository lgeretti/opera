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
#include "deserialisation.hpp"

using namespace Opera;

class ProfileDeserialisation : public Profiler {
  public:
    ProfileDeserialisation() : Profiler(1e5) { }

    void run() {
        profile_bodysamplepacket();
        profile_bodypresentationpacket();
        profile_collisionnotificationpacket();
    }

    void profile_bodysamplepacket() {
        SizeType num_samples = 4259;
        profile("Deserialisation of a human sample JSON file into BodyStatePacket",[&](SizeType i){
            Deserialiser<BodyStatePacket>(Resources::path("json/scenarios/nocollision/h0/"+std::to_string(i+1)+".json")).make();
        },num_samples);

        List<std::string> json_texts;
        profile("Deserialisation of a human sample JSON file into String",[&](SizeType i){
            json_texts.push_back(Deserialiser<BodyStatePacket>(Resources::path("json/scenarios/nocollision/h0/"+std::to_string(i+1)+".json")).to_string());
        },num_samples);

        profile("Deserialisation of a human sample JSON String into BodyStatePacket",[&](SizeType i){
            Deserialiser<BodyStatePacket>(json_texts.at(i).c_str()).make();
        },num_samples);
    }

    void profile_bodypresentationpacket() {
        SizeType num_samples = 1000;
        profile("Deserialisation of a robot presentation JSON file into BodyPresentationPacket",[&](auto){
            Deserialiser<BodyPresentationPacket>(Resources::path("json/examples/presentation/robot0.json")).make();
            },num_samples);

        List<std::string> json_texts;
        profile("Deserialisation of a robot presentation JSON file into String",[&](auto){
            json_texts.push_back(Deserialiser<BodyPresentationPacket>(Resources::path("json/examples/presentation/robot0.json")).to_string());
            },num_samples);

        profile("Deserialisation of a robot presentation JSON String into BodyPresentationPacket",[&](SizeType i){
            Deserialiser<BodyPresentationPacket>(json_texts.at(i).c_str()).make();
            },num_samples);
    }

    void profile_collisionnotificationpacket() {
        SizeType num_samples = 1000;
        profile("Deserialisation of a collision notification JSON file into CollisionNotificationPacket",[&](auto){
            Deserialiser<CollisionNotificationPacket>(Resources::path("json/examples/notification/notification0.json")).make();
            },num_samples);

        List<std::string> json_texts;
        profile("Deserialisation of a collision notification JSON file into String",[&](auto){
            json_texts.push_back(Deserialiser<CollisionNotificationPacket>(Resources::path("json/examples/notification/notification0.json")).to_string());
            },num_samples);

        profile("Deserialisation of a collision notification JSON String into CollisionNotificationPacket",[&](SizeType i){
            Deserialiser<CollisionNotificationPacket>(json_texts.at(i).c_str()).make();
            },num_samples);
    }

};

int main() {
    ProfileDeserialisation().run();
}
