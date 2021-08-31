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
#include <filesystem>
#include "body.hpp"

namespace Opera {

using FilePath = std::filesystem::path;

//! \brief Utility for making a JSON description file from a human or robot
class BodySerialiser {
  public:
    //! \brief Construct providing the path to save into
    BodySerialiser(FilePath const& file);

    //! \brief Serialise the body
    void serialise(Body const& body) const;

  private:

    //! \brief Save the \a document
    void _save(rapidjson::Document const& document) const;

  private:
    FilePath const _filepath;
};

}

#endif //OPERA_SERIALISATION_HPP
