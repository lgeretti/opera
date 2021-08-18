/***************************************************************************
 *            geometry.hpp
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

#ifndef OPERA_GEOMETRY_HPP
#define OPERA_GEOMETRY_HPP

#include "declarations.hpp"

namespace Opera {

struct Vector {
    Vector(double x_, double y_, double z_) : x(FloatType(x_,Ariadne::dp)), y(FloatType(y_,Ariadne::dp)), z(FloatType(z_,Ariadne::dp)) { }
    Vector(FloatType x_, FloatType y_, FloatType z_) : x(x_), y(y_), z(z_) { }
    FloatType x;
    FloatType y;
    FloatType z;

    friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
        return os << "(" << v.x << "," << v.y << "," << v.z << ")"; }
};

using Point = Vector;

inline FloatType dot(Vector const& v1, Vector const& v2) {
    return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

inline Vector operator-(Vector const& v1, Vector const& v2) {
    return Vector({v1.x-v2.x, v1.y-v2.y, v1.z-v2.z });
}

inline Vector operator+(Vector const& v1, Vector const& v2) {
    return Vector({v1.x+v2.x, v1.y+v2.y, v1.z+v2.z });
}

inline Vector operator*(FloatType const& s, Vector const& v) {
    return Vector({s*v.x, s*v.y, s*v.z});
}

inline Vector operator/(Vector const& v, unsigned int c) {
    return Vector({v.x/c, v.y/c, v.z/c});
}

inline Vector operator*(Vector const& v, FloatType const& s) {
    return s*v;
}

inline Vector operator*(Vector const& v1, Vector const& v2) {
    return Vector({v1.y*v2.z-v1.z*v2.y, v1.z*v2.x-v1.x*v2.z, v1.x*v2.y-v1.y*v2.x});
}

//! \brief The center of the segment joining two points
Point center(Point const& p1, Point const& p2);

//! \brief The minimum distance between a segment s1 (with head/tail points s1h and s1t) and segment s2
//! (with head/tail points s2h and s2t)
FloatType distance(Point const& s1h, Point const& s1t, Point const& s2h, Point const& s2t);

//! \brief The minimum distance between a point p1 and segment s2
//! (with head/tail points s2h and s2t)
FloatType distance(Point const& p1, Point const& s2h, Point const& s2t);

}

#endif //OPERA_GEOMETRY_HPP
