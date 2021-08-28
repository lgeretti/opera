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
#include <fstream>

namespace Opera {

BodyDeserialiser::BodyDeserialiser(String const& filename) {
    std::ifstream ifs;
    ifs.open(filename);
    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &_root, &errs)) {
        std::cout << errs << std::endl;
    } else
        std::cout << _root << std::endl;
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
