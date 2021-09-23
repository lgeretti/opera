/***************************************************************************
 *            serialisation.cpp
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

#include "serialisation.hpp"

namespace Opera {

using namespace rapidjson;

Document BodyPresentationPacketSerialiser::to_document() const {

    Document document;
    document.SetObject();
    Document::AllocatorType& allocator = document.GetAllocator();

    Value id;
    id.SetString(obj.id().c_str(),obj.id().length());
    document.AddMember("id",id,allocator);
    document.AddMember("isHuman",Value().SetBool(obj.is_human()),allocator);
    if (not obj.is_human()) document.AddMember("packetFrequency",Value().SetUint(obj.packet_frequency()),allocator);

    Value thicknesses;
    Value point_ids;
    thicknesses.SetArray();
    point_ids.SetArray();
    for (SizeType i=0; i<obj.point_ids().size(); ++i) {
        thicknesses.PushBack(Value().SetDouble(obj.thicknesses()[i].get_d()),allocator);
        Value points;
        points.SetArray();
        points.PushBack(Value().SetUint(obj.point_ids()[i].first),allocator);
        points.PushBack(Value().SetUint(obj.point_ids()[i].second),allocator);
        point_ids.PushBack(points,allocator);
    }
    document.AddMember("pointIds",point_ids,allocator);
    document.AddMember("thicknesses",thicknesses,allocator);

    return document;
}

Document BodyStatePacketSerialiser::to_document() const {

    Document document;
    document.SetObject();
    Document::AllocatorType& allocator = document.GetAllocator();

    Value id;
    id.SetString(obj.id().c_str(),obj.id().length());
    document.AddMember("bodyId",id,allocator);

    if (not obj.location().values().empty()) {
        Value discreteState;
        discreteState.SetObject();
        for (auto v : obj.location().values()) {
            discreteState.AddMember(Value().SetString(v.first.c_str(),v.first.length(),allocator),Value().SetString(v.second.c_str(),v.second.length(), allocator),allocator);
        }
        document.AddMember("discreteState",discreteState,allocator);
    }

    Value continuous_state;
    continuous_state.SetArray();
    for (auto samples : obj.points()) {
        Value samples_array;
        samples_array.SetArray();
        for (auto point : samples) {
            Value coordinates;
            coordinates.SetArray();
            coordinates.PushBack(Value().SetDouble(point.x.get_d()),allocator)
            .PushBack(Value().SetDouble(point.y.get_d()),allocator)
            .PushBack(Value().SetDouble(point.z.get_d()),allocator);
            samples_array.PushBack(coordinates,allocator);
        }
        continuous_state.PushBack(samples_array,allocator);
    }
    document.AddMember("continuousState",continuous_state,allocator);
    document.AddMember("timestamp",Value().SetUint64(obj.timestamp()),allocator);

    return document;
}

Document CollisionNotificationPacketSerialiser::to_document() const {

    Document document;
    document.SetObject();
    Document::AllocatorType& allocator = document.GetAllocator();

    Value human;
    human.SetObject();
    human.AddMember("bodyId",Value().SetString(obj.human_id().c_str(),obj.human_id().length(),allocator),allocator);
    human.AddMember("segmentId",Value().SetUint(obj.human_segment_id()),allocator);
    document.AddMember("human",human,allocator);

    Value robot;
    robot.SetObject();
    robot.AddMember("bodyId",Value().SetString(obj.robot_id().c_str(),obj.robot_id().length(),allocator),allocator);
    robot.AddMember("segmentId",Value().SetUint(obj.robot_segment_id()),allocator);
    document.AddMember("robot",robot,allocator);

    if (not obj.discrete_state().values().empty()) {
        Value discreteState;
        discreteState.SetObject();
        for (auto v : obj.discrete_state().values()) {
            discreteState.AddMember(Value().SetString(v.first.c_str(),v.first.length(),allocator),Value().SetString(v.second.c_str(),v.second.length(), allocator),allocator);
        }
        document.AddMember("discreteState",discreteState,allocator);
    }

    Value collision_time;
    collision_time.SetObject();
    collision_time.AddMember("lower",Value().SetUint64(obj.lower_collision_time()),allocator);
    collision_time.AddMember("upper",Value().SetUint64(obj.upper_collision_time()),allocator);
    document.AddMember("collisionTime",collision_time,allocator);

    document.AddMember("likelihood",Value().SetDouble(obj.likelihood().get_d()),allocator);

    return document;
}

}
