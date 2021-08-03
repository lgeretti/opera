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

    //! \brief Whether it intersects an \a other segment
    //! \details Returns true also in the case of tangency
    bool intersects(BodySegment const& other) const;

  private:
    Point _begin;
    Point _end;
    FloatType _thickness;
    BoundingType _bb;
};

//! \brief Calculate the minimum distance between two segments
inline FloatType distance(BodySegment const& s1, BodySegment const& s2) {

    const FloatType SMALL_VALUE(0.000001,Ariadne::dp);

    Vector uS({ s1.begin().x, s1.begin().y, s1.begin().z });
    Vector uE({ s1.end().x, s1.end().y, s1.end().z });
    Vector vS({ s2.begin().x, s2.begin().y, s2.begin().z });
    Vector vE({ s2.end().x, s2.end().y, s2.end().z });
    auto u = uE - uS;
    auto v = vE - vS;
    auto w = uS - vS;

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

    Vector dP = w + (sc * u) - (tc * v);

    return sqrt(dot(dP, dP));
}

}

#endif //OPERA_BODY_HPP
