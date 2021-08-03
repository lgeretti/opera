/***************************************************************************
 *            body.hpp
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

#ifndef OPERA_BODY_HPP
#define OPERA_BODY_HPP

#include "point.hpp"
#include <ariadne.hpp>

namespace Opera {

class BodySegment {
  public:

    //! \brief Construct from two points and a thickness
    BodySegment(Point const& begin, Point const& end, FloatType const& thickness);

    //! \brief Return the begin point of the segment
    Point const& begin() const;
    //! \brief Return the end point of the segment
    Point const& end() const;
    //! \brief Return the thickness of the body segment around the geometrical segment
    FloatType const& thickness() const;

    //! \brief Return the bounding box overapproximation
    BoundingType const& bounding_box() const;

  private:
    Point _begin;
    Point _end;
    FloatType _thickness;
    BoundingType _bb;
};

}

#endif //OPERA_BODY_HPP
