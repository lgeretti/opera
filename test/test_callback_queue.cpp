/***************************************************************************
 *            test_callback_queue.cpp
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

#include "test.hpp"

#include "packet.hpp"
#include "callback_queue.hpp"

using namespace Opera;

class TestCallbackQueue {
public:
    void test() {
        ARIADNE_TEST_CALL(test_create())
        ARIADNE_TEST_CALL(test_add_remove())
        ARIADNE_TEST_CALL(test_callback())
    }

    void test_create() {
        CallbackQueue<BodyStatePacket> queue;
        ARIADNE_TEST_ASSERT(queue.empty())
        ARIADNE_TEST_EQUALS(queue.size(),0)
        ARIADNE_TEST_FAIL(queue.front())
        ARIADNE_TEST_FAIL(queue.back())
    }

    void test_add_remove() {
        BodyStatePacket p("h0",{{Point(0,0,0)},{Point(0,2,0)}},3e8);
        CallbackQueue<BodyStatePacket> queue;
        queue.add(p);
        ARIADNE_TEST_EQUALS(queue.size(),1)
        ARIADNE_TEST_EXECUTE(queue.front())
        ARIADNE_TEST_EXECUTE(queue.back())
        queue.remove();
        ARIADNE_TEST_ASSERT(queue.empty())
    }

    void test_callback() {
        BodyStatePacket p("h0",{{Point(0,0,0)},{Point(0,2,0)}},3e8);
        CallbackQueue<BodyStatePacket> queue;
        bool called_back = false;
        queue.set_add_callback([&](auto p){ called_back = true; ARIADNE_TEST_EQUALS(p.id(),"h0") });
        queue.add(p);
        ARIADNE_TEST_ASSERT(called_back)
        called_back = false;
        queue.set_remove_callback([&](auto p){ called_back = true; ARIADNE_TEST_EQUALS(p.id(),"h0") });
        queue.remove();
        ARIADNE_TEST_ASSERT(called_back)
    }
};

int main() {
    TestCallbackQueue().test();
    return ARIADNE_TEST_FAILURES;
}
