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
inline FloatType distance(BodySegmentOccupancy const& s1, BodySegmentOccupancy const& s2) {

    const FloatType SMALL_VALUE(0.000001,Ariadne::dp);

    auto u = s1.tail_position() - s1.head_position();
    auto v = s2.tail_position() - s2.head_position();
    auto w = s1.head_position() - s2.head_position();

    FloatType a = dot(u, u);
    FloatType b = dot(u, v);
    FloatType c = dot(v, v);
    FloatType d = dot(u, w);
    FloatType e = dot(v, w);
    FloatType D = a * c - b * b;
    FloatType sc = D, sN = D, sD = D;
    FloatType tc = D, tN = D, tD = D;
    if (decide(D < SMALL_VALUE)) {
        sN = 0;
        sD = 1;
        tN = e;
        tD = c;
    } else {
        sN = (b * e - c * d);
        tN = (a * e - b * d);
        if (decide(sN < 0)) {
            sN = 0;
            tN = e;
            tD = c;
        } else if (decide(sN > sD)) {
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }
    if (decide(tN < 0)) {
        tN = 0;
        if (decide(-d < 0)) {
            sN = 0;
        } else if (decide(-d > a)) {
            sN = sD;
        } else {
            sN = -d;
            sD = a;
        }
    } else if (decide(tN > tD)) {
        tN = tD;
        if (decide((-d + b) < 0)) {
            sN = 0;
        } else if (decide((-d + b) > a)) {
            sN = sD;
        } else {
            sN = (-d + b);
            sD = a;
        }
    }

    if (decide(abs(sN) < SMALL_VALUE)) sc = 0;
    else sc = sN / sD;

    if (decide(abs(tN) < SMALL_VALUE)) tc = 0;
    else tc = tN / tD;

    auto dP = w + (sc * u) - (tc * v);

    return sqrt(dot(dP, dP));
}

}

#endif //OPERA_BODY_HPP
