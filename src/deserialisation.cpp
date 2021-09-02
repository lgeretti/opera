/***************************************************************************
 *            deserialisation.cpp
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

#include "deserialisation.hpp"

namespace Opera {

using namespace rapidjson;
using Ariadne::Map;

bool BodyDeserialiser::is_human() const {
    return _document["isHuman"].GetBool();
}

List<SizeType> BodyDeserialiser::_get_point_ids() const {
    List<SizeType> result;
    for (auto& extremes : _document["pointIds"].GetArray()) {
        result.append(extremes[0].GetUint());
        result.append(extremes[1].GetUint());
    }
    return result;
}

List<FloatType> BodyDeserialiser::_get_thicknesses() const {
    List<FloatType> result;
    for (auto& thickness : _document["thicknesses"].GetArray())
        result.append(FloatType(thickness.GetDouble(),dp));
    return result;
}

Human BodyDeserialiser::make_human() const {
    ARIADNE_ASSERT(is_human())
    return Human(_document["id"].GetString(), _get_point_ids(), _get_thicknesses());
}

Robot BodyDeserialiser::make_robot() const {
    ARIADNE_ASSERT(not is_human())
    return Robot(_document["id"].GetString(), _document["packetFrequency"].GetUint(), _get_point_ids(), _get_thicknesses());
}

BodyStatePacket BodyStatePacketDeserialiser::make() const {
    Map<StringVariable,String> discrete_state_values;
    for (auto& v : _document["discreteState"].GetObject())
        discrete_state_values.insert(std::make_pair(StringVariable(v.name.GetString()),v.value.GetString()));
    List<List<Point>> points;
    for (auto& point_samples : _document["continuousState"].GetArray()) {
        List<Point> samples;
        for (auto& pt : point_samples.GetArray())
            samples.append(Point(pt[0].GetDouble(),pt[1].GetDouble(),pt[2].GetDouble()));
        points.append(samples);
    }

    return BodyStatePacket(_document["bodyId"].GetString(),DiscreteLocation(discrete_state_values),points,_document["timestamp"].GetUint64());
}

CollisionNotificationPacket CollisionNotificationPacketDeserialiser::make() const {
    Map<StringVariable,String> discrete_state_values;
    for (auto& v : _document["discreteState"].GetObject())
        discrete_state_values.insert(std::make_pair(StringVariable(v.name.GetString()),v.value.GetString()));

    return CollisionNotificationPacket(_document["human"]["bodyId"].GetString(),
                                       _document["human"]["segmentId"].GetUint(),
                                       _document["robot"]["bodyId"].GetString(),
                                       _document["robot"]["segmentId"].GetUint(),
                                       DiscreteLocation(discrete_state_values),
                                       _document["collisionTime"]["lower"].GetUint64(),
                                       _document["collisionTime"]["upper"].GetUint64());
}

}
