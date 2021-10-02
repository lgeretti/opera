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
        profile_centre();
        profile_hull();
        profile_circle_radius();
        profile_point_point_distance();
        profile_point_segment_distance();
        profile_segment_segment_distance();
    }

    void profile_centre() {
        Ariadne::List<Box> bbs;
        for (SizeType i=0; i<num_tries(); ++i) {
            auto pt1 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            auto pt2 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            bbs.push_back(hull(pt1,pt2));
        }

        profile("Bounding box centre",[&](SizeType i){ bbs.at(i).centre(); });
    }

    void profile_hull() {
        Ariadne::List<Point> heads, tails;
        for (SizeType i=0; i<num_tries(); ++i) {
            heads.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            tails.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Hull of two points",[&](SizeType i){ hull(heads.at(i),heads.at(i)); });
    }

    void profile_circle_radius() {
        Ariadne::List<Box> bbs;
        for (SizeType i=0; i<num_tries(); ++i) {
            auto pt1 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            auto pt2 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            bbs.push_back(hull(pt1,pt2));
        }

        profile("Bounding box circle radius",[&](SizeType i){ bbs.at(i).circle_radius(); });
    }

    void profile_point_point_distance() {
        Ariadne::List<Point> p1, p2;
        for (SizeType i=0; i<num_tries(); ++i) {
            p1.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            p2.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Point-point distance",[&](SizeType i){ distance(p1.at(i),p2.at(i)); });
    }

    void profile_point_segment_distance() {
        Ariadne::List<Point> points, heads, tails;
        for (SizeType i=0; i<num_tries(); ++i) {
            points.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            heads.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            tails.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Point-segment distance using segment distance",[&](SizeType i){ distance(points.at(i),points.at(i),heads.at(i),tails.at(i)); });
        profile("Point-segment distance using dedicated distance",[&](SizeType i){ distance(points.at(i),heads.at(i),tails.at(i)); });
    }

    void profile_segment_segment_distance() {
        Point s1h(1.0,3.0,-2.0);
        Point s1t(4.0,1.2,0);
        Ariadne::List<Point> heads, tails;
        for (SizeType i=0; i<num_tries(); ++i) {
            heads.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            tails.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Segment-segment distance",[&](SizeType i){ distance(s1h,s1t,heads.at(i),tails.at(i)); });
    }
};


int main() {
    ProfileGeometry().run();
}
