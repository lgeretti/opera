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
#include "body.hpp"

namespace Opera {

//! \brief Utility for making a human or robot from a JSON description file
class BodyDeserialiser {
  public:
    //! \brief Construct by parsing a JSON \a filename
    BodyDeserialiser(String const& filename);

    //! \brief If it is a human or alternatively a robot
    bool is_human() const;

    //! \brief Make a human out of the parsed content
    Human make_human() const;
    //! \brief Make a robot out of the parsed content
    Robot make_robot() const;

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, BodyDeserialiser const& d);

  private:

    //! \brief Get the point ids from the document
    List<SizeType> _get_point_ids() const;
    //! \brief Get the thicknesses from the document
    List<FloatType> _get_thicknesses() const;

  private:
    rapidjson::Document _document;
};

}

#endif //OPERA_DESERIALISATION_HPP
