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

#include "algebra3d.hpp"

namespace Opera {

class BodySegment {
  public:
    //! \brief Construct from head_position/tail_position identifiers and thickness
    BodySegment(unsigned int head_id, unsigned int tail_id, FloatType const& thickness);

    //! \brief Identifier for the head_position point
    unsigned int head_id() const;

    //! \brief Identifier for the tail_position point
    unsigned int tail_id() const;

    //! \brief Return the thickness of the body segment around the geometrical segment
    FloatType const& thickness() const;

  private:
    unsigned int const _head_id;
    unsigned int const _tail_id;
    FloatType const _thickness;
};

class BodySegmentOccupancy {
  public:

    //! \brief Construct from two points and a thickness
    BodySegmentOccupancy(BodySegment* segment, Point const& begin, Point const& end);

    //! \brief Return the position of the head_position point of the segment
    Point const& head_position() const;
    //! \brief Return the position of the tail_position point of the segment
    Point const& tail_position() const;

    //! \brief Return the bounding box overapproximation
    BoundingType const& bounding_box() const;

    //! \brief Whether it intersects an \a other segment
    //! \details Returns true also in the case of tangency
    bool intersects(BodySegmentOccupancy const& other) const;

  private:
    Point const _begin;
    Point const _end;
    BodySegment* const _segment;
    BoundingType _bb;
};

//! \brief Calculate the minimum distance between two segments
FloatType distance(BodySegmentOccupancy const& s1, BodySegmentOccupancy const& s2);

}

#endif //OPERA_BODY_HPP
