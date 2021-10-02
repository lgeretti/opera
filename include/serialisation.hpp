/***************************************************************************
 *            serialisation.hpp
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

#ifndef OPERA_SERIALISATION_HPP
#define OPERA_SERIALISATION_HPP

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <filesystem>
#include <fstream>
#include "body.hpp"
#include "packet.hpp"
#include "macros.hpp"

namespace Opera {

using FilePath = std::filesystem::path;

//! \brief Interface for serialisation into a file or a string
template<class T> class SerialiserInterface {
  public:
    //! \brief Serialise into file
    virtual void to_file(FilePath const& file) const = 0;
    //! \brief Serialise into String
    virtual std::string to_string() const = 0;
};

//! \brief Base implementation of serialisation, from a rapidjson Document
template<class T> class SerialiserBase : public SerialiserInterface<T> {
  public:
    //! \brief Pass the object by const reference
    SerialiserBase(T const& o) : obj(o) { }

    //! \brief Convert the object into a rapidjson document
    virtual rapidjson::Document to_document() const = 0;

  public:
    void to_file(FilePath const& file) const override {
        auto document = to_document();
        std::ofstream ofs(file);
        OPERA_ASSERT_MSG(ofs.is_open(), "Could not open file '" << file << "' for writing.")
        rapidjson::OStreamWrapper osw(ofs);
        rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
        document.Accept(writer);
    }

    std::string to_string() const override {
        auto document = to_document();
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        return buffer.GetString();
    }

  protected:
    T const& obj;
};

template<class T> class Serialiser;

//! \brief Utility for making a JSON description from a presentation packet
template<> class Serialiser<BodyPresentationPacket> : public SerialiserBase<BodyPresentationPacket> {
  public:
    using SerialiserBase::SerialiserBase;
  protected:
    rapidjson::Document to_document() const override;
};

//! \brief Utility for making a JSON description from a state packet
template<> class Serialiser<BodyStatePacket> : public SerialiserBase<BodyStatePacket> {
  public:
    using SerialiserBase::SerialiserBase;
  protected:
    rapidjson::Document to_document() const override;
};

//! \brief Utility for making a JSON description from a notification packet
template<> class Serialiser<CollisionNotificationPacket> : public SerialiserBase<CollisionNotificationPacket> {
  public:
    using SerialiserBase::SerialiserBase;
  protected:
    rapidjson::Document to_document() const override;
};

}

#endif //OPERA_SERIALISATION_HPP
