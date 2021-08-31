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

#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include "serialisation.hpp"

namespace Opera {

using namespace rapidjson;

BodySerialiser::BodySerialiser(String const& filename) : _filename(filename) { }

void BodySerialiser::serialise(Body const& body) const {

    auto robot_ptr = dynamic_cast<Robot const*>(&body);
    bool is_human = (robot_ptr == nullptr);
    Document document;
    document.SetObject();
    Document::AllocatorType& allocator = document.GetAllocator();

    Value id;
    id.SetString(body.id().c_str(),body.id().length());
    document.AddMember("id",id,allocator);
    document.AddMember("isHuman",Value().SetBool(is_human),allocator);

    Value thicknesses;
    Value point_ids;
    thicknesses.SetArray();
    point_ids.SetArray();
    for (SizeType i=0; i<body.num_segments(); ++i) {
        auto const& s = body.segment(i);
        thicknesses.PushBack(Value().SetDouble(s.thickness().get_d()),allocator);
        Value points;
        points.SetArray();
        points.PushBack(Value().SetUint(s.head_id()),allocator);
        points.PushBack(Value().SetUint(s.tail_id()),allocator);
        point_ids.PushBack(points,allocator);
    }
    document.AddMember("pointIds",point_ids,allocator);
    document.AddMember("thicknesses",thicknesses,allocator);

    _save(document);
}

void BodySerialiser::_save(Document const& document) const {
    std::ofstream ofs(_filename);
    ARIADNE_ASSERT_MSG(ofs.is_open(), "Could not open file '" << _filename << "' for writing.")
    OStreamWrapper osw(ofs);
    Writer<OStreamWrapper> writer(osw);
    document.Accept(writer);
}

}
