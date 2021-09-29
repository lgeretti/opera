/***************************************************************************
 *            profile_serialisation.cpp
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
#include "serialisation.hpp"

using namespace Opera;

class ProfileSerialisation : public Profiler {
  public:
    ProfileSerialisation() : Profiler(1e5) { }

    void run() {
        profile_bodypresentationpacket();
        profile_bodystatepacket();
        profile_collisionnotificationpacket();
    }

    void profile_bodypresentationpacket() {
        SizeType num_samples = 1000;
        List<BodyPresentationPacket> packets;
        for (SizeType i=0; i<num_tries(); ++i)
            packets.append(BodyPresentationPacketDeserialiser(Resources::path("json/examples/presentation/human0.json")).make());

        profile("Serialisation of a BodyPresentationPacket into a human presentation JSON String",[&](SizeType i){
            Serialiser<BodyPresentationPacket>(packets.at(i)).to_string();
            },num_samples);
    }

    void profile_bodystatepacket() {
        SizeType num_samples = 4259;

        List<BodyStatePacket> packets;
        for (SizeType i=0; i<num_samples; ++i)
            packets.append(BodyStatePacketDeserialiser(Resources::path("json/scenarios/nocollision/h0/"+std::to_string(i+1)+".json")).make());

        profile("Serialisation of a BodyStatePacket into a human sample JSON String",[&](SizeType i){
            Serialiser<BodyStatePacket>(packets.at(i)).to_string();
        },num_samples);
    }

    void profile_collisionnotificationpacket() {
        SizeType num_samples = 1000;
        List<CollisionNotificationPacket> packets;
        for (SizeType i=0; i<num_tries(); ++i)
            packets.append(CollisionNotificationPacketDeserialiser(Resources::path("json/examples/notification/notification0.json")).make());

        profile("Serialisation of a CollisionNotificationPacket into a human presentation JSON String",[&](SizeType i){
            Serialiser<CollisionNotificationPacket>(packets.at(i)).to_string();
            },num_samples);
    }
};

int main() {
    ProfileSerialisation().run();
}
