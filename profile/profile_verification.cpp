/***************************************************************************
 *            profile_verification.hpp
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

#include "verification.hpp"
#include "profile.hpp"

using namespace Opera;

struct ProfileVerification : public Profiler {

    ProfileVerification() : Profiler(1e5) { }

    void run() {
        profile_apply_to_trace();
        profile_resume_index();
        profile_resume_or_not();
    }

    void profile_apply_to_trace() {
        Robot r("r0", 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h("h0", {0, 1}, {FloatType(1.0, Ariadne::dp)});
        auto hs = h.segment(0).create_sample(Point(0,0,0),Point(2,0,0)).spherical_approximation();

        MinimumDistanceBarrierTrace trace(hs);
        List<BodySegmentSample> rss;
        for (SizeType i=num_tries(); i>0; --i) {
            rss.append(r.segment(0).create_sample(Point(0,5+i,0),Point(2,6+i,0)));
        }

        profile("Apply to trace (decreasing distance)",[&](SizeType i){ trace.try_update_with(rss.at(i)); });

        trace.clear();
        rss.clear();
        for (SizeType i=0; i<num_tries(); ++i) {
            rss.append(r.segment(0).create_sample(Point(4+i,4,0),Point(6+i,4,0)));
        }

        profile("Apply to trace (increasing distance)",[&](SizeType i){ trace.try_update_with(rss.at(i)); });
    }

    void profile_resume_index() {
        const SizeType ns = 1000;
        const SizeType override_num_tries = 100;
        Robot r("r0", 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h("h0", {0, 1}, {FloatType(0.5, Ariadne::dp)});
        auto hs = h.segment(0).create_sample(Point(ns,ns,0),Point(ns+2,ns,0)).spherical_approximation();

        List<MinimumDistanceBarrierTrace> ts;
        List<SphericalApproximationSample> beginning_sas, middle_sas, end_sas;
        for (SizeType i=0; i<override_num_tries; ++i) {
            MinimumDistanceBarrierTrace trace(hs);
            Point last_head(0,0,0);
            for (SizeType j=0; j<ns;++j) {
                Point new_head(last_head.x+rnd().get(0.99,1.01),last_head.y+rnd().get(0.99,1.01),a_zero);
                Point new_tail(new_head.x+2,new_head.y,new_head.z);
                trace.try_update_with(r.segment(0).create_sample(new_head,new_tail));
                last_head = new_head;
            }
            ts.append(trace);

            Point beginning_head_sa(rnd().get(0.9,1.1),rnd().get(0.9,1.1),a_zero);
            Point beginning_tail_sa(beginning_head_sa.x+2,beginning_head_sa.y,beginning_head_sa.z);
            beginning_sas.append(h.segment(0).create_sample(beginning_head_sa,beginning_tail_sa).spherical_approximation());

            Point middle_head_sa(ns/2*rnd().get(0.9,1.1),ns/2*rnd().get(0.9,1.1),a_zero);
            Point middle_tail_sa(middle_head_sa.x+2,middle_head_sa.y,middle_head_sa.z);
            middle_sas.append(h.segment(0).create_sample(middle_head_sa,middle_tail_sa).spherical_approximation());

            Point end_head_sa(ns*rnd().get(0.9,1.1),ns*rnd().get(0.9,1.1),a_zero);
            Point end_tail_sa(end_head_sa.x+2,end_head_sa.y,end_head_sa.z);
            end_sas.append(h.segment(0).create_sample(end_head_sa,end_tail_sa).spherical_approximation());
        }

        profile("Resume index (strictly beginning of trace)",[&](SizeType i){ ts.at(i).resume_index(beginning_sas.at(i)); },override_num_tries);
        profile("Resume index (middle of trace)",[&](SizeType i){ ts.at(i).resume_index(middle_sas.at(i)); },override_num_tries);
        profile("Resume index (near end of trace)",[&](SizeType i){ ts.at(i).resume_index(end_sas.at(i)); },override_num_tries);
    }

    void profile_resume_or_not() {
        const SizeType ns = 800;
        const SizeType override_num_tries = 1;
        Robot r("r0", 10, {0, 1}, {FloatType(1.0, Ariadne::dp)});
        Human h("h0", {0, 1}, {FloatType(0.5, Ariadne::dp)});
        auto hs = h.segment(0).create_sample(Point(ns,ns,0),Point(ns+2,ns,0)).spherical_approximation();

        List<BodySegmentSample> rss;
        for (SizeType i=0; i<ns; ++i) {
            rss.append(r.segment(0).create_sample(Point(i,0,0),Point(i,2,0)));
        }
        List<BodySegmentSample> hss;
        for (SizeType i=ns; i>0; --i) {
            hss.append(h.segment(0).create_sample(Point(ns+i,0,0),Point(ns+i,2,0)));
        }

        profile("Using resuming for segments intersection detection",[&](SizeType i){
            SizeType resume_idx = 0;
            MinimumDistanceBarrierTrace trace(hss.at(0).spherical_approximation());
            for (SizeType i=0; i<ns; ++i) {
                bool update_trace = true;
                for (SizeType j=resume_idx; j<ns; ++j) {
                    if (update_trace and not trace.try_update_with(rss.at(j)))
                        update_trace = false;
                    if (hss.at(i).intersects(rss.at(j))) break;
                }
                if (i<ns-1) {
                    trace.reset(hss.at(i+1),rss);
                    resume_idx = trace.next_index();
                }
            }
        },override_num_tries);

        profile("Not using resuming for segments intersection detection",[&](SizeType i){
            for (SizeType i=0; i<ns; ++i) {
                for (SizeType j=i; j<ns; ++j) {
                    if (hss.at(i).intersects(rss.at(j))) break;
                }
            }
        },override_num_tries);
    }
};

int main() {
    ProfileVerification().run();
}
