/***************************************************************************
 *            test_kafka.cpp
 *
 *  Copyright  2021  Andrea Gagliardo, Sandro Ferrari, Luca Geretti
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

#include "test.hpp"
#include "kafka.hpp"

using namespace Opera;

class TestKafka{
public: 
    void test(){
        ARIADNE_TEST_CALL(test_comunication_presentazione())
        //ARIADNE_TEST_CALL(test_comunication_stato_ibrido())
        //ARIADNE_TEST_CALL(test_comunication_notifica())
    }

    void test_comunication_presentazione(){

        ConsumerPresentazione * consumer_pres;
        consumer_pres = new ConsumerPresentazione(0,"localhost:9092", "", 0);

        RdKafka::Producer * producer = create_producer("localhost:9092");

        presentazione prs_sent;
        prs_sent.id = "ID_del_body";
        prs_sent.type = true;
        prs_sent.pointlds.clear();
        prs_sent.pointlds.push_back(std::make_pair(10,10));
        prs_sent.pointlds.push_back(std::make_pair(20,20));
        prs_sent.sampleFrequency = 500;
        prs_sent.thicknesses.clear();
        prs_sent.thicknesses.push_back(12.12);
        prs_sent.thicknesses.push_back(2.15);

        ARIADNE_TEST_CALL(send_presentazione(prs_sent, producer));

        std::thread cpt(consumer_prs_thread, consumer_pres);
        presentazione prs;
        prs = consumer_pres->get_struct();

        ARIADNE_TEST_EQUAL(prs.id, prs_sent.id);
        ARIADNE_TEST_EQUAL(prs.type, prs_sent.type);
        //ARIADNE_TEST_EQUAL(prs.pointlds, prs_sent.pointlds);
        ARIADNE_TEST_EQUAL(prs.sampleFrequency, prs_sent.sampleFrequency);
        //ARIADNE_TEST_EQUAL(prs.thicknesses, prs_sent.thicknesses);
        

    }

};

int main() {
    TestKafka().test();
    return ARIADNE_TEST_FAILURES;
}