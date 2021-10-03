/***************************************************************************
 *            profile_barrier.hpp
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

#include "barrier.hpp"
#include "profile.hpp"

using namespace Opera;

struct ProfileBarrier : public Profiler {

    ProfileBarrier() : Profiler(100000) { }

    void run() {
        profile_apply_to_trace();
        profile_resume_index();
        profile_resume_or_not();
    }

    void profile_apply_to_trace() {
        Robot r("r0", 10, {{0, 1}}, {1.0});
        Human h("h0", {{0, 1}}, {1.0});
        auto hs = h.segment(0).create_sample({Point(0,0,0)},{Point(2,0,0)});

        DiscreteState first({r.id(),"first"});
        MinimumDistanceBarrierTrace trace1(hs,0);
        List<BodySegmentSample> rss;
        for (SizeType i=num_tries(); i>0; --i) {
            rss.push_back(r.segment(0).create_sample({Point(0,5+i,0)},{Point(2,6+i,0)}));
        }

        profile("Update trace with sample (decreasing distance)",[&](SizeType i){ trace1.try_update_with(first,rss.at(i)); });

        MinimumDistanceBarrierTrace trace2(hs,0);
        rss.clear();
        for (SizeType i=0; i<num_tries(); ++i) {
            rss.push_back(r.segment(0).create_sample({Point(4+i,4,0)},{Point(6+i,4,0)}));
        }

        profile("Update trace with sample (increasing distance)",[&](SizeType i){ trace2.try_update_with(first,rss.at(i)); });
    }

    void profile_resume_index() {
        const SizeType ns = 1000;
        const SizeType override_num_tries = 100;
        Robot r("r0", 10, {{0, 1}}, {1.0});
        Human h("h0", {{0, 1}}, {0.5});
        auto hs = h.segment(0).create_sample({Point(FloatType(ns),FloatType(ns),0)},{Point(FloatType(ns+2),FloatType(ns),0)});

        DiscreteState first({r.id(),"first"});
        List<MinimumDistanceBarrierTrace> ts;
        List<SphericalApproximationSample> beginning_sas, middle_sas, end_sas;
        for (SizeType i=0; i<override_num_tries; ++i) {
            MinimumDistanceBarrierTrace trace(hs,0);
            Point last_head(0,0,0);
            for (SizeType j=0; j<ns;++j) {
                Point new_head(last_head.x+rnd().get(0.99,1.01),last_head.y+rnd().get(0.99,1.01),0.0);
                Point new_tail(new_head.x+2,new_head.y,new_head.z);
                trace.try_update_with(first,r.segment(0).create_sample({new_head},{new_tail}));
                last_head = new_head;
            }
            ts.push_back(trace);

            Point beginning_head_sa(rnd().get(0.9,1.1),rnd().get(0.9,1.1),0.0);
            Point beginning_tail_sa(beginning_head_sa.x+2,beginning_head_sa.y,beginning_head_sa.z);
            beginning_sas.push_back(h.segment(0).create_sample({beginning_head_sa},{beginning_tail_sa}).spherical_approximation());

            Point middle_head_sa(FloatType(ns)/2*rnd().get(0.9,1.1),FloatType(ns)/2*rnd().get(0.9,1.1),0.0);
            Point middle_tail_sa(middle_head_sa.x+2,middle_head_sa.y,middle_head_sa.z);
            middle_sas.push_back(h.segment(0).create_sample({middle_head_sa},{middle_tail_sa}).spherical_approximation());

            Point end_head_sa(FloatType(ns)*rnd().get(0.9,1.1),FloatType(ns)*rnd().get(0.9,1.1),0.0);
            Point end_tail_sa(end_head_sa.x+2,end_head_sa.y,end_head_sa.z);
            end_sas.push_back(h.segment(0).create_sample({end_head_sa},{end_tail_sa}).spherical_approximation());
        }

        profile("Find resume element (strictly beginning of trace)",[&](SizeType i){ ts.at(i).resume_element(beginning_sas.at(i)); },override_num_tries);
        profile("Find resume element (middle of trace)",[&](SizeType i){ ts.at(i).resume_element(middle_sas.at(i)); },override_num_tries);
        profile("Find resume element (near end of trace)",[&](SizeType i){ ts.at(i).resume_element(end_sas.at(i)); },override_num_tries);
    }

    void profile_resume_or_not() {
        const SizeType ns = 1000;
        const SizeType override_num_tries = 1;
        Robot r("r0", 10, {{0, 1}}, {1.0});
        Human h("h0", {{0, 1}}, {0.5});
        auto hs = h.segment(0).create_sample({Point(ns,ns,0)},{Point(ns+2,ns,0)});

        DiscreteState first({r.id(),"first"});
        DiscreteState second({r.id(),"second"});
        MinimumDistanceBarrierTrace trace(hs,0);
        RobotStateHistory history(&r);
        for (SizeType i=0; i<ns; ++i) {
            history.acquire(first,{{Point(FloatType(i),0,0)},{Point(FloatType(i),2,0)}},static_cast<TimestampType>(i*100000000));
        }
        history.acquire(second,{{Point(FloatType(ns),0,0)},{Point(FloatType(ns),2,0)}},static_cast<TimestampType>(ns*100000000));

        List<BodySegmentSample> hss;
        for (SizeType i=ns; i>0; --i) {
            hss.push_back(h.segment(0).create_sample({Point(FloatType(ns+i),0,0)},{Point(FloatType(ns+i),2,0)}));
        }

        auto const& samples = history.samples(first).at(trace.robot_segment_id());

        profile("Using resuming for segments intersection detection",[&](auto){
            SizeType resume_idx = 0;
            MinimumDistanceBarrierTrace tr(hss.at(0), 0);
            for (SizeType i=0; i<ns; ++i) {
                bool update_trace = true;
                for (SizeType j=resume_idx; j<ns; ++j) {
                    if (update_trace and not tr.try_update_with(first, samples.at(j)))
                        update_trace = false;
                    if (hss.at(i).intersects(samples.at(j))) break;
                }
                if (i<ns-1) {
                    tr.reset(hss.at(i + 1), history);
                    resume_idx = tr.next_index();
                }
            }
        },override_num_tries);

        profile("Not using resuming for segments intersection detection",[&](auto){
            for (SizeType i=0; i<ns; ++i) {
                for (SizeType j=i; j<ns; ++j) {
                    if (hss.at(i).intersects(samples.at(j))) break;
                }
            }
        },override_num_tries);
    }
};

int main() {
    ProfileBarrier().run();
}
