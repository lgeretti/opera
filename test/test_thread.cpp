/***************************************************************************
 *            test_thread.cpp
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

#include "thread.hpp"
#include "declarations.hpp"
#include "utility.hpp"
#include "test.hpp"

using namespace Opera;

using namespace std::chrono_literals;

class TestThread {
  public:

    void test_create() const {
        Thread thread1([]{}, "thr");
        OPERA_TEST_EXECUTE(thread1.id())
        OPERA_TEST_EQUALS(thread1.name(),"thr")
        Thread thread2([]{});
        OPERA_TEST_EQUALS(to_string(thread2.id()),thread2.name())
    }

    void test_destroy_before_completion() const {
        Thread thread([] { std::this_thread::sleep_for(100ms); });
    }

    void test_task() const {
        int a = 0;
        Thread thread([&a] { a++; });
        std::this_thread::sleep_for(10ms);
        OPERA_TEST_EQUALS(a,1)
    }

    void test_atomic_multiple_threads() const {
        SizeType n_threads = 10*std::thread::hardware_concurrency();
        OPERA_TEST_PRINT(n_threads)
        List<std::shared_ptr<Thread>> threads;

        std::atomic<SizeType> a = 0;
        for (SizeType i=0; i<n_threads; ++i) {
            threads.push_back(std::make_shared<Thread>([&a] { a++; }));
        }

        std::this_thread::sleep_for(100ms);
        OPERA_TEST_EQUALS(a,n_threads)
        threads.clear();
    }

    void test() {
        OPERA_TEST_CALL(test_create())
        OPERA_TEST_CALL(test_destroy_before_completion())
        OPERA_TEST_CALL(test_task())
        OPERA_TEST_CALL(test_atomic_multiple_threads())
    }

};

int main() {
    TestThread().test();
    return OPERA_TEST_FAILURES;
}
