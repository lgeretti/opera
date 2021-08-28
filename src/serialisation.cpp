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
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include "serialisation.hpp"

namespace Opera {

using namespace rapidjson;

BodyDeserialiser::BodyDeserialiser(String const& filename) {

    using namespace rapidjson;

    std::ifstream ifs(filename);
    ARIADNE_ASSERT_MSG(ifs.is_open(), "Could not open '" << filename << "' file for reading.")
    IStreamWrapper isw(ifs);
    _document.ParseStream(isw);

    ARIADNE_ASSERT_MSG(not _document.HasParseError(),"Parse error '" << _document.GetParseError() << "' at offset " << _document.GetErrorOffset())

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    _document.Accept(writer);
    std::cout << buffer.GetString() << std::endl;

    buffer.Clear();
    Writer<StringBuffer> writer2(buffer);
    _document.Accept(writer2);
    std::cout << buffer.GetString() << std::endl;

    std::ofstream ofs { "output.json" };
    ARIADNE_ASSERT_MSG(ofs.is_open(), "Could not open file for writing.")
    OStreamWrapper osw(ofs);
    Writer<OStreamWrapper> writer3(osw);
    _document.Accept(writer3);
}

bool BodyDeserialiser::is_human() const {
    return false;
}

Human BodyDeserialiser::make_human() const {
    return Human("h0", {0, 1}, {FloatType(1.0, Ariadne::dp)});
}

Robot BodyDeserialiser::make_robot() const {
    return Robot("r0", 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
}

}
