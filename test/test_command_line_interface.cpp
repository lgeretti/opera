/***************************************************************************
 *            test_command_line_interface.cpp
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

#include "command_line_interface.hpp"
#include "logging.hpp"
#include "test.hpp"

using namespace Opera;

class TestCommandLineInterface {
  public:

    void test() {
        OPERA_TEST_CALL(test_empty_argument_stream())
        OPERA_TEST_CALL(test_nonempty_argument_stream())
        OPERA_TEST_CALL(test_cli_instantiation())
        OPERA_TEST_CALL(test_from_c_arguments())
        OPERA_TEST_CALL(test_scheduler_parsing())
        OPERA_TEST_CALL(test_theme_parsing())
        OPERA_TEST_CALL(test_verbosity_parsing())
        OPERA_TEST_CALL(test_multiple_argument_parsing())
        OPERA_TEST_CALL(test_unrecognised_argument())
        OPERA_TEST_CALL(test_duplicate_argument())
        OPERA_TEST_CALL(test_print_help())
    }

    void test_empty_argument_stream() {
        OPERA_TEST_FAIL(ArgumentStream({}))
    }

    void test_nonempty_argument_stream() {
        auto stream = ArgumentStream({"a","b"});
        OPERA_TEST_ASSERT(not stream.empty())
        OPERA_TEST_EQUALS(stream.size(),2)
        OPERA_TEST_EQUALS(stream.peek(),"a")
        auto val1 = stream.pop();
        OPERA_TEST_EQUALS(val1,"a")
        auto val2 = stream.pop();
        OPERA_TEST_EQUALS(val2,"b")
        OPERA_TEST_ASSERT(stream.empty())
        OPERA_TEST_FAIL(stream.peek())
        OPERA_TEST_FAIL(stream.pop())
    }

    void test_cli_instantiation() {
        OPERA_TEST_EXECUTE(CommandLineInterface::instance())
    }

    void test_from_c_arguments() {
        const char* argv1[] = {""};
        bool success1 = CommandLineInterface::instance().acquire(1,argv1);
        OPERA_TEST_ASSERT(success1)
        const char* argv2[] = {"",""};
        bool success2 = CommandLineInterface::instance().acquire(2,argv2);
        OPERA_TEST_ASSERT(not success2)
    }

    void test_scheduler_parsing() {
        bool success1 = CommandLineInterface::instance().acquire({"", "-s", "immediate"});
        OPERA_TEST_ASSERT(success1)
        bool success2 = CommandLineInterface::instance().acquire({"", "--scheduler", "immediate"});
        OPERA_TEST_ASSERT(success2)
        bool success3 = CommandLineInterface::instance().acquire({"", "-s", "wrong"});
        OPERA_TEST_ASSERT(not success3)
        bool success4 = CommandLineInterface::instance().acquire({"", "-s", "blocking"});
        OPERA_TEST_ASSERT(success4)
        bool success5 = CommandLineInterface::instance().acquire({"", "-s", "nonblocking"});
        OPERA_TEST_ASSERT(success5)
        bool success6 = CommandLineInterface::instance().acquire({"", "-s"});
        OPERA_TEST_ASSERT(not success6)
        bool success7 = CommandLineInterface::instance().acquire({"", "-c", "max", "-s", "immediate"});
        OPERA_TEST_ASSERT(success7)
    }

    void test_theme_parsing() {
        bool success1 = CommandLineInterface::instance().acquire({"", "-t", "none"});
        OPERA_TEST_ASSERT(success1)
        bool success2 = CommandLineInterface::instance().acquire({"", "--theme", "none"});
        OPERA_TEST_ASSERT(success2)
        bool success3 = CommandLineInterface::instance().acquire({"", "-t", "nn"});
        OPERA_TEST_ASSERT(not success3)
        bool success4 = CommandLineInterface::instance().acquire({"", "-t", "light"});
        OPERA_TEST_ASSERT(success4)
        bool success5 = CommandLineInterface::instance().acquire({"", "-t", "dark"});
        OPERA_TEST_ASSERT(success5)
        bool success6 = CommandLineInterface::instance().acquire({"", "-t"});
        OPERA_TEST_ASSERT(not success6)
    }

    void test_verbosity_parsing() {
        bool success1 = CommandLineInterface::instance().acquire({"", "-v", "5"});
        OPERA_TEST_ASSERT(success1)
        OPERA_TEST_EQUALS(Logger::instance().configuration().verbosity(),5)
        bool success2 = CommandLineInterface::instance().acquire({"", "--verbosity", "0"});
        OPERA_TEST_ASSERT(success2)
        OPERA_TEST_EQUALS(Logger::instance().configuration().verbosity(),0)
        bool success3 = CommandLineInterface::instance().acquire({"", "-v", "-2"});
        OPERA_TEST_ASSERT(not success3)
        bool success4 = CommandLineInterface::instance().acquire({"", "-v", "q"});
        OPERA_TEST_ASSERT(not success4)
        bool success5 = CommandLineInterface::instance().acquire({"", "-v"});
        OPERA_TEST_ASSERT(not success5)
    }

    void test_multiple_argument_parsing() {
        bool success = CommandLineInterface::instance().acquire({"", "-c", "2", "--verbosity", "4"});
        OPERA_TEST_ASSERT(success)
        OPERA_TEST_EQUALS(Logger::instance().configuration().verbosity(),4)
    }

    void test_unrecognised_argument() {
        bool success = CommandLineInterface::instance().acquire({"", "--invalid"});
        OPERA_TEST_ASSERT(not success)
    }

    void test_duplicate_argument() {
        bool success = CommandLineInterface::instance().acquire({"", "--verbosity", "2", "-v", "5"});
        OPERA_TEST_ASSERT(not success)
    }

    void test_print_help() {
        bool success = CommandLineInterface::instance().acquire({"", "-h"});
        OPERA_TEST_ASSERT(not success)
    }

};

int main(int argc, const char* argv[]) {
    TestCommandLineInterface().test();
    return OPERA_TEST_FAILURES;
}

