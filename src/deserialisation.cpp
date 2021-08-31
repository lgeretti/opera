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

bool BodyDeserialiser::is_human() const {
    return _document["isHuman"].GetBool();
}

List<SizeType> BodyDeserialiser::_get_point_ids() const {
    List<SizeType> result;
    auto segments_array = _document["pointIds"].GetArray();
    for (SizeType i=0; i<segments_array.Size(); ++i) {
        auto point_ids_array = segments_array[i].GetArray();
        result.append(point_ids_array[0].GetUint());
        result.append(point_ids_array[1].GetUint());
    }
    return result;
}

List<FloatType> BodyDeserialiser::_get_thicknesses() const {
    List<FloatType> result;
    auto thicknesses_array = _document["thicknesses"].GetArray();
    for (SizeType i=0; i<thicknesses_array.Size(); ++i) {
        result.append(FloatType(thicknesses_array[i].GetDouble(),dp));
    }
    return result;
}

Human BodyDeserialiser::make_human() const {
    ARIADNE_ASSERT(is_human())
    return Human(_document["id"].GetString(), _get_point_ids(), _get_thicknesses());
}

Robot BodyDeserialiser::make_robot() const {
    ARIADNE_ASSERT(not is_human())
    return Robot(_document["id"].GetString(), _document["packageFrequency"].GetUint(), _get_point_ids(), _get_thicknesses());
}

}
