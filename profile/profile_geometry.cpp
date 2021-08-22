/***************************************************************************
 *            profile_geometry.cpp
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
#include "profile.hpp"

using namespace Opera;

struct ProfileGeometry : public Profiler {

    ProfileGeometry() : Profiler(1e5) { }

    void run() {
        profile_center();
        profile_hull();
        profile_circle_radius();
        profile_segment_distance();
        profile_point_distance();
    }

    void profile_center() {
        Ariadne::List<BoundingType> bbs;
        for (SizeType i=0; i<num_tries(); ++i) {
            auto pt1 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            auto pt2 = Ariadne::Point<FloatType>({rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)});
            auto bx1 = BoundingType({IntervalType(pt1.x,pt1.x),IntervalType(pt1.z,pt1.z),IntervalType(pt1.z,pt1.z)});
            bbs.push_back(hull(bx1,pt2));
        }

        profile("Bounding box centre",[&bbs](SizeType i){ (bbs.at(i).centre()); });
    }

    void profile_hull() {
        Ariadne::List<Point> heads, tails;
        for (SizeType i=0; i<num_tries(); ++i) {
            heads.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            tails.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Hull of two points",[&heads,&tails](SizeType i){ hull(heads.at(i),heads.at(i)); });
    }

    void profile_circle_radius() {
        Ariadne::List<BoundingType> bbs;
        for (SizeType i=0; i<num_tries(); ++i) {
            auto pt1 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            auto pt2 = Ariadne::Point<FloatType>({rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)});
            auto bx1 = BoundingType({IntervalType(pt1.x,pt1.x),IntervalType(pt1.z,pt1.z),IntervalType(pt1.z,pt1.z)});
            bbs.push_back(hull(bx1,pt2));
        }

        profile("Bounding box circle error",[&bbs](SizeType i){ circle_radius(bbs.at(i)); });
    }

    void profile_segment_distance() {
        Point s1h(1.0,3.0,-2.0);
        Point s1t(4.0,1.2,0);
        Ariadne::List<Point> heads, tails;
        for (SizeType i=0; i<num_tries(); ++i) {
            heads.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            tails.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Segment distance",[s1h,s1t,heads,tails](SizeType i){ distance(s1h,s1t,heads.at(i),tails.at(i)); });
    }

    void profile_point_distance() {
        Ariadne::List<Point> points, heads, tails;
        for (SizeType i=0; i<num_tries(); ++i) {
            points.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            heads.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            tails.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Point distance using segment distance",[points,heads,tails](SizeType i){ distance(points.at(i),points.at(i),heads.at(i),tails.at(i)); });
        profile("Point distance using dedicated distance",[points,heads,tails](SizeType i){ distance(points.at(i),heads.at(i),tails.at(i)); });
    }
};


int main() {
    ProfileGeometry().run();
}
