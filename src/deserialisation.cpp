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
#include "discrete_state.hpp"

namespace Opera {

using namespace rapidjson;

BodyPresentationPacket Deserialiser<BodyPresentationPacket>::make() const {
    List<Pair<IdType,IdType>> point_ids;
    for (auto& extremes : _document["pointIds"].GetArray())
        point_ids.push_back(std::make_pair(extremes[0].GetUint(),extremes[1].GetUint()));

    List<FloatType> thicknesses;
    for (auto& thickness : _document["thicknesses"].GetArray())
        thicknesses.push_back(thickness.GetDouble());

    if (_document["isHuman"].GetBool())
        return BodyPresentationPacket(_document["id"].GetString(), point_ids, thicknesses);
    else
        return BodyPresentationPacket(_document["id"].GetString(), _document["packetFrequency"].GetUint(), point_ids, thicknesses);
}

BodyStatePacket Deserialiser<BodyStatePacket>::make() const {
    Map<String,String> discrete_state_values;
    if (_document.HasMember("discreteState"))
        for (auto& v : _document["discreteState"].GetObject())
            discrete_state_values.insert(std::make_pair(v.name.GetString(),v.value.GetString()));
    List<List<Point>> points;
    for (auto& point_samples : _document["continuousState"].GetArray()) {
        List<Point> samples;
        for (auto& pt : point_samples.GetArray())
            samples.push_back(Point(pt[0].GetDouble(),pt[1].GetDouble(),pt[2].GetDouble()));
        points.push_back(samples);
    }

    return BodyStatePacket(_document["bodyId"].GetString(),DiscreteState(discrete_state_values),points,_document["timestamp"].GetUint64());
}

CollisionNotificationPacket Deserialiser<CollisionNotificationPacket>::make() const {
    Map<String,String> discrete_state_values;
    for (auto& v : _document["discreteState"].GetObject())
        discrete_state_values.insert(std::make_pair(v.name.GetString(),v.value.GetString()));

    return CollisionNotificationPacket(_document["human"]["bodyId"].GetString(),
                                       _document["human"]["segmentId"].GetUint(),
                                       _document["robot"]["bodyId"].GetString(),
                                       _document["robot"]["segmentId"].GetUint(),
                                       discrete_state_values,
                                       _document["collisionTime"]["lower"].GetUint64(),
                                       _document["collisionTime"]["upper"].GetUint64(),
                                       _document["likelihood"].GetDouble());
}

}
