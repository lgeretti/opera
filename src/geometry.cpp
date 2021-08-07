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

Point center(Point const& p1, Point const& p2) {
    return (p1+p2)/2;
}

FloatType distance(Point const& s1h, Point const& s1t, Point const& s2h, Point const& s2t) {

    const FloatType SMALL_VALUE(0.000001,Ariadne::dp);

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