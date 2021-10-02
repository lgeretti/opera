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

inline bool operator==(Vector const& v1, Vector const& v2) {
    return v1.x == v2.x and v1.y == v2.y and v1.z == v2.z;
}

//! \brief A box in the coordinate 3D space
class Box {
  public:
    Box(FloatType const& xl, FloatType const& xu, FloatType const& yl, FloatType const& yu, FloatType const& zl, FloatType const& zu);

    //! \brief Make an empty box
    static Box make_empty();

    //! \brief Whether the box is empty
    bool is_empty() const;

    FloatType const& xl() const;
    FloatType const& xu() const;
    FloatType const& yl() const;
    FloatType const& yu() const;
    FloatType const& zl() const;
    FloatType const& zu() const;

    //! \brief The centre point
    Point centre() const;
    //! \brief The radius of the circle inscribing the box
    FloatType circle_radius() const;

    //! \brief Check whether the boxes have any common point
    bool disjoint(Box const& other) const;

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, Box const& b) {
        return os << "{[" << b.xl() << ":" << b.xu() <<"],[" << b.yl() << ":" << b.yu() << "],[" << b.zl() << ":" << b.zu() << "]}"; }

  private:
    FloatType _xl;
    FloatType _xu;
    FloatType _yl;
    FloatType _yu;
    FloatType _zl;
    FloatType _zu;
};

//! \brief The centre of the segment joining two points
Point centre(Point const& p1, Point const& p2);

//! \brief The minimum bounding box enclosing the two points \a p1 and \a p2
Box hull(Point const& p1, Point const& p2);

//! \brief The minimum distance between a segment s1 (with head/tail points s1h and s1t) and segment s2
//! (with head/tail points s2h and s2t)
FloatType distance(Point const& s1h, Point const& s1t, Point const& s2h, Point const& s2t);

//! \brief The minimum distance between a point p1 and segment s2
//! (with head/tail points s2h and s2t)
FloatType distance(Point const& p1, Point const& s2h, Point const& s2t);

//! \brief The distance between two points
FloatType distance(Point const& p1, Point const& p2);

//! \brief Return the widening of \a bb of \a v in all directions
Box widen(Box const& bb, FloatType const& v);

}

#endif //OPERA_GEOMETRY_HPP
