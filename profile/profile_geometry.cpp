/***************************************************************************
 *            profile_geometry.cpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of Opera, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "geometry.hpp"
#include "profile.hpp"

using namespace Opera;

struct ProfileGeometry : public Profiler {

    ProfileGeometry() : Profiler(100000) { }

    void run() {
        profile_centre();
        profile_hull();
        profile_circle_radius();
        profile_point_point_distance();
        profile_point_segment_distance();
        profile_segment_segment_distance();
    }

    void profile_centre() {
        List<Box> bbs;
        for (SizeType i=0; i<num_tries(); ++i) {
            auto pt1 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            auto pt2 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            bbs.push_back(hull(pt1,pt2));
        }

        profile("Bounding box centre",[&](SizeType i){ bbs.at(i).centre(); });
    }

    void profile_hull() {
        List<Point> heads, tails;
        for (SizeType i=0; i<num_tries(); ++i) {
            heads.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            tails.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Hull of two points",[&](SizeType i){ hull(heads.at(i),heads.at(i)); });
    }

    void profile_circle_radius() {
        List<Box> bbs;
        for (SizeType i=0; i<num_tries(); ++i) {
            auto pt1 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            auto pt2 = Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0));
            bbs.push_back(hull(pt1,pt2));
        }

        profile("Bounding box circle radius",[&](SizeType i){ bbs.at(i).circle_radius(); });
    }

    void profile_point_point_distance() {
        List<Point> p1, p2;
        for (SizeType i=0; i<num_tries(); ++i) {
            p1.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
            p2.push_back(Point(rnd().get(-5.0,5.0),rnd().get(-5.0,5.0),rnd().get(-5.0,5.0)));
        }

        profile("Point-point distance",[&](SizeType i){ distance(p1.at(i),p2.at(i)); });
    }

    void profile_point_segment_distance() {
        List<Point> points, heads, tails;
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
        List<Point> heads, tails;
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
