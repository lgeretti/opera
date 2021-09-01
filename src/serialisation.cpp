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

Document BodySerialiser::to_document() const {

    auto robot_ptr = dynamic_cast<Robot const*>(&obj);
    bool is_human = (robot_ptr == nullptr);
    Document document;
    document.SetObject();
    Document::AllocatorType& allocator = document.GetAllocator();

    Value id;
    id.SetString(obj.id().c_str(),obj.id().length());
    document.AddMember("id",id,allocator);
    document.AddMember("isHuman",Value().SetBool(is_human),allocator);

    Value thicknesses;
    Value point_ids;
    thicknesses.SetArray();
    point_ids.SetArray();
    for (SizeType i=0; i<obj.num_segments(); ++i) {
        auto const& s = obj.segment(i);
        thicknesses.PushBack(Value().SetDouble(s.thickness().get_d()),allocator);
        Value points;
        points.SetArray();
        points.PushBack(Value().SetUint(s.head_id()),allocator);
        points.PushBack(Value().SetUint(s.tail_id()),allocator);
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

}
