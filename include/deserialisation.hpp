/***************************************************************************
 *            deserialisation.hpp
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

#ifndef OPERA_DESERIALISATION_HPP
#define OPERA_DESERIALISATION_HPP

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/istreamwrapper.h>
#include <filesystem>
#include <fstream>
#include "body.hpp"
#include "packet.hpp"
#include "macros.hpp"

namespace Opera {

using FilePath = std::filesystem::path;

//! \brief Base for a class deserialising a JSON file or string
template<class T> class DeserialiserBase {
  public:
    DeserialiserBase(FilePath const& file) {
        std::ifstream ifs(file);
        OPERA_ASSERT_MSG(ifs.is_open(), "Could not open '" << file << "' file for reading.")
        rapidjson::IStreamWrapper isw(ifs);
        _document.ParseStream(isw);
        OPERA_ASSERT_MSG(not _document.HasParseError(),"Parse error '" << _document.GetParseError() << "' at offset " << _document.GetErrorOffset())
    }

    DeserialiserBase(const char* text) {
        _document.Parse(text);
        OPERA_ASSERT_MSG(not _document.HasParseError(),"Parse error '" << _document.GetParseError() << "' at offset " << _document.GetErrorOffset())
    }

    //! \brief Convert to string
    String to_string() const {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        _document.Accept(writer);
        return buffer.GetString();
    }

    //! \brief Make the object
    virtual T make() const = 0;

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, DeserialiserBase const& d) {
        return os << d.to_string();
    }
  protected:
    rapidjson::Document _document;
};

template<class T> class Deserialiser;

//! \brief Converter to a BodyPresentationPacket from a JSON description file
template<> class Deserialiser<BodyPresentationPacket> : public DeserialiserBase<BodyPresentationPacket> {
  public:
    using DeserialiserBase::DeserialiserBase;
    BodyPresentationPacket make() const override;
};

//! \brief Converter to a BodyStatePacket from a JSON description file
template<> class Deserialiser<BodyStatePacket> : public DeserialiserBase<BodyStatePacket> {
  public:
    using DeserialiserBase::DeserialiserBase;
    BodyStatePacket make() const override;
};

//! \brief Convert to a CollisionNotificationPacket from a JSON description file
template<> class Deserialiser<CollisionNotificationPacket> : public DeserialiserBase<CollisionNotificationPacket> {
  public:
    using DeserialiserBase::DeserialiserBase;
    CollisionNotificationPacket make() const override;
};

}

#endif //OPERA_DESERIALISATION_HPP
