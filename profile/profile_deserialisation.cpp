/***************************************************************************
 *            profile_deserialisation.cpp
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

#include "profile.hpp"

#include "utility.hpp"
#include "deserialisation.hpp"

using namespace Opera;

class ProfileDeserialisation : public Profiler {
  public:
    ProfileDeserialisation() : Profiler(100000) { }

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
