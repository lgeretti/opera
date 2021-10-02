/***************************************************************************
 *            geometry.cpp
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

#include "geometry.hpp"

namespace Opera {

Point centre(Point const& p1, Point const& p2) {
    return (p1+p2)/2;
}

Box hull(Point const& p1, Point const& p2) {
    return Box(std::min(p1.x, p2.x), std::max(p1.x, p2.x), std::min(p1.y, p2.y), std::max(p1.y, p2.y), std::min(p1.z, p2.z), std::max(p1.z, p2.z));
}

FloatType distance(Point const& s1h, Point const& s1t, Point const& s2h, Point const& s2t) {

    const FloatType SMALL_VALUE = 1e-6;

    auto u = s1t - s1h;
    auto v = s2t - s2h;
    auto w = s1h - s2h;

    FloatType a = dot(u, u);
    FloatType b = dot(u, v);
    FloatType c = dot(v, v);
    FloatType d = dot(u, w);
    FloatType e = dot(v, w);
    FloatType D = a * c - b * b;
    FloatType sc = D, sN = D, sD = D;
    FloatType tc = D, tN = D, tD = D;
    if (D < SMALL_VALUE) {
        sN = 0;
        sD = 1;
        tN = e;
        tD = c;
    } else {
        sN = (b * e - c * d);
        tN = (a * e - b * d);
        if (sN < 0) {
            sN = 0;
            tN = e;
            tD = c;
        } else if (sN > sD) {
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }
    if (tN < 0) {
        tN = 0;
        if (-d < 0) {
            sN = 0;
        } else if (-d > a) {
            sN = sD;
        } else {
            sN = -d;
            sD = a;
        }
    } else if (tN > tD) {
        tN = tD;
        if ((-d + b) < 0) {
            sN = 0;
        } else if ((-d + b) > a) {
            sN = sD;
        } else {
            sN = (-d + b);
            sD = a;
        }
    }

    if (abs(sN) < SMALL_VALUE) sc = 0;
    else sc = sN / sD;

    if (abs(tN) < SMALL_VALUE) tc = 0;
    else tc = tN / tD;

    auto dP = w + (sc * u) - (tc * v);

    return sqrt(dot(dP, dP));
}

FloatType distance(Point const& p1, Point const& s2h, Point const& s2t) {

    const FloatType SMALL_VALUE = 1e-6;

    auto v = s2t - s2h;
    auto w = p1 - s2h;

    FloatType c = dot(v, v);
    FloatType e = dot(v, w);
    FloatType tc = 0.0, tN = e, tD = c;

    if (tN < 0)
        tN = 0;
    else if (tN > tD)
        tN = tD;

    if (abs(tN) >= SMALL_VALUE)
        tc = tN / tD;

    auto dP = w - (tc * v);

    return sqrt(dot(dP, dP));
}

FloatType distance(Point const& p1, Point const& p2) {
    return sqrt(dot(p1-p2,p1-p2));
}

Box::Box(FloatType const& xl, FloatType const& xu, FloatType const& yl, FloatType const& yu, FloatType const& zl, FloatType const& zu)
    : _xl(xl), _xu(xu), _yl(yl), _yu(yu), _zl(zl), _zu(zu) { }

Box Box::make_empty() {
    return Box(infinity,-infinity,infinity,-infinity,infinity,-infinity);
}

bool Box::is_empty() const {
    return (_xl > _xu or _yl > _yu or _zl > _zu);
}

FloatType const& Box::xl() const { return _xl; }

FloatType const& Box::xu() const { return _xu; }

FloatType const& Box::yl() const { return _yl; }

FloatType const& Box::yu() const { return _yu; }

FloatType const& Box::zl() const { return _zl; }

FloatType const& Box::zu() const { return _zu; }

Point Box::centre() const {
    ARIADNE_PRECONDITION(not is_empty())
    return Point((_xl+_xu)/2,(_yl+_yu)/2,(_zl+_zu)/2);
}

FloatType Box::circle_radius() const {
    return sqrt(pow(_xu-_xl,2)+pow(_yu-_yl,2)+pow(_zu-_zl,2))/2;
}

bool Box::disjoint(Box const& other) const {
    return _xu < other._xl or _xl > other._xu or _yu < other._yl or _yl > other._yu or _zu < other._zl or _zl > other._zu;
}

Box widen(Box const& bb, FloatType const& v) {
    return Box(bb.xl()-v,bb.xu()+v,bb.yl()-v,bb.yu()+v,bb.zl()-v,bb.zu()+v);
}

}