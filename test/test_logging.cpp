/***************************************************************************
 *            test_logging.cpp
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

#include <sys/ioctl.h>
#include <unistd.h>
#include "thread.hpp"
#include "logging.hpp"
#include "progress_indicator.hpp"
#include "test.hpp"

using namespace Opera;

void sample_function() {
    OPERA_LOG_SCOPE_CREATE
    OPERA_LOG_PRINTLN("val=inf, x0=2.0^3*1.32424242432423[2,3], y>[0.1:0.2] (z={0:1}), 1, x0, x11, true@1.")
}

void sample_printhold_simple_loop(std::string txt, unsigned int u) {
    OPERA_LOG_SCOPE_CREATE
    for (unsigned int i=0; i<3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(u));
        OPERA_LOG_SCOPE_PRINTHOLD(txt<<"@"<<i);
    }
}

void sample_printhold_nested_loop(std::string txt, unsigned int u) {
    OPERA_LOG_SCOPE_CREATE
    for (unsigned int i=0; i<3; ++i) {
        sample_printhold_simple_loop("internal",u);
        std::this_thread::sleep_for(std::chrono::milliseconds(u));
        OPERA_LOG_SCOPE_PRINTHOLD(txt<<"@"<<i)
    }
}

void print_something1() {
    OPERA_LOG_PRINTLN("This is a call from thread id " << std::this_thread::get_id() << " named '" << Logger::instance().current_thread_name() << "'")
}

void print_something2() {
    OPERA_LOG_SCOPE_CREATE
    OPERA_LOG_PRINTLN("This is a call from thread id " << std::this_thread::get_id() << " named '" << Logger::instance().current_thread_name() << "'")
}

class TestLogging {
  public:

    TestLogging() {
        Logger::instance().configuration().set_prints_level_on_change_only(false);
    }

    void test() {
        OPERA_TEST_CALL(test_very_pretty_function())
        OPERA_TEST_CALL(test_print_configuration())
        OPERA_TEST_CALL(test_shown_single_print())
        OPERA_TEST_CALL(test_hidden_single_print())
        OPERA_TEST_CALL(test_muted_print())
        OPERA_TEST_CALL(test_use_blocking_scheduler())
        OPERA_TEST_CALL(test_use_nonblocking_scheduler())
        OPERA_TEST_CALL(test_shown_call_function_with_entrance_and_exit())
        OPERA_TEST_CALL(test_hide_call_function_with_entrance_and_exit())
        OPERA_TEST_CALL(test_indents_based_on_level())
        OPERA_TEST_CALL(test_hold_line())
        OPERA_TEST_CALL(test_hold_long_line())
        OPERA_TEST_CALL(test_hold_multiple())
        OPERA_TEST_CALL(test_light_theme())
        OPERA_TEST_CALL(test_dark_theme())
        OPERA_TEST_CALL(test_theme_custom_keyword())
        OPERA_TEST_CALL(test_theme_bgcolor_bold_underline())
        OPERA_TEST_CALL(test_handles_multiline_output())
        OPERA_TEST_CALL(test_discards_newlines_and_indentation())
        OPERA_TEST_CALL(test_redirect())
        OPERA_TEST_CALL(test_multiple_threads_with_blocking_scheduler())
        OPERA_TEST_CALL(test_multiple_threads_with_nonblocking_scheduler())
        OPERA_TEST_CALL(test_register_self_thread())
        OPERA_TEST_CALL(test_thread_name_printing_policy())
        OPERA_TEST_CALL(test_set_prints_level_on_change_only())
    }

    void test_very_pretty_function() {
        std::string fn = "Opera::ReturnClass Opera::Class::method(Opera::ArgumentClass const&) const";
        auto prettified = very_pretty_function(fn);
        OPERA_TEST_EQUAL(prettified,"ReturnClass Class::method(ArgumentClass const&) const")
    }

    void test_print_configuration() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(1);
        OPERA_LOG_PRINTLN(Logger::instance().configuration())
    }

    void test_shown_single_print() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(1);
        OPERA_LOG_PRINTLN("This is a call on level 1")
    }

    void test_hidden_single_print() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(0);
        OPERA_LOG_PRINTLN("This is a hidden call on level 1")
    }

    void test_muted_print() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(1);
        OPERA_LOG_PRINTLN("This is the first line shown.")
        OPERA_LOG_RUN_MUTED(print_something1())
        OPERA_LOG_PRINTLN("This is the second and last line shown.")
    }

    void test_use_blocking_scheduler() {
        Logger::instance().use_blocking_scheduler();
        Logger::instance().configuration().set_verbosity(1);
        OPERA_LOG_PRINTLN("This is a call")
        OPERA_LOG_PRINTLN("This is another call")
    }

    void test_use_nonblocking_scheduler() {
        Logger::instance().use_nonblocking_scheduler();
        Logger::instance().configuration().set_verbosity(1);
        OPERA_LOG_PRINTLN("This is a call")
        OPERA_LOG_PRINTLN("This is another call")
    }

    void test_shown_call_function_with_entrance_and_exit() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(2);
        Logger::instance().configuration().set_prints_scope_entrance(true);
        Logger::instance().configuration().set_prints_scope_exit(true);
        OPERA_LOG_PRINTLN("This is a call on level 1");
        OPERA_LOG_RUN_AT(0,sample_function());
        OPERA_LOG_PRINTLN("This is again a call on level 1");
    }

    void test_hide_call_function_with_entrance_and_exit() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(2);
        Logger::instance().configuration().set_prints_scope_entrance(false);
        Logger::instance().configuration().set_prints_scope_exit(false);
        OPERA_LOG_PRINTLN("This is a call on level 1");
        OPERA_LOG_RUN_AT(1,sample_function());
        OPERA_LOG_PRINTLN("This is again a call on level 1");
    }

    void test_indents_based_on_level() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(2);
        Logger::instance().configuration().set_indents_based_on_level(true);
        OPERA_LOG_PRINTLN("Call at level 1");
        OPERA_LOG_PRINTLN_AT(1,"Call at level 2");
        Logger::instance().configuration().set_indents_based_on_level(false);
        OPERA_LOG_PRINTLN("Call at level 1");
        OPERA_LOG_PRINTLN_AT(1,"Call at level 2");
    }

    void test_handles_multiline_output() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(2);
        Logger::instance().configuration().set_handles_multiline_output(true);
        OPERA_LOG_PRINTLN("This is a very long string for this test that will most definitely be longer than just one line, at least if the number of columns in the terminal is not excessively large, but this should suffice I believe. Just to be sure, let's add some more characters to the line and the result should get into a second line.");
        Logger::instance().configuration().set_handles_multiline_output(false);
        OPERA_LOG_PRINTLN("This is a very long string for this test that will most definitely be longer than just one line, at least if the number of columns in the terminal is not excessively large, but this should suffice I believe. Just to be sure, let's add some more characters to the line and the result should get into a second line.");
    }

    void test_discards_newlines_and_indentation() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(2);
        Logger::instance().configuration().set_discards_newlines_and_indentation(true);
        OPERA_LOG_PRINTLN("This text should just be in a single line \n       with no extra whitespaces.");
        Logger::instance().configuration().set_discards_newlines_and_indentation(false);
        OPERA_LOG_PRINTLN("This text should be in two lines\n          where the second one starts several whitespaces in.");
    }

    void _hold_short_line() {
        OPERA_LOG_SCOPE_CREATE;
        ProgressIndicator indicator(10.0);
        for (unsigned int i=0; i<10; ++i) {
            indicator.update_current(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            OPERA_LOG_SCOPE_PRINTHOLD("[" << indicator.symbol() << "] " << indicator.percentage() << "%");
        }
    }

    void test_hold_line() {
        Logger::instance().use_immediate_scheduler();
        _hold_short_line();
        Logger::instance().use_blocking_scheduler();
        _hold_short_line();
        Logger::instance().use_nonblocking_scheduler();
        _hold_short_line();
    }

    void test_hold_long_line() {
        Logger::instance().use_immediate_scheduler();
        OPERA_LOG_SCOPE_CREATE;

        const unsigned int DEFAULT_COLUMNS = 80;
        struct winsize ws;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        SizeType num_cols = (ws.ws_col > 0 ? ws.ws_col : DEFAULT_COLUMNS);

        std::string exactly_str(num_cols-4,'x'); // exactly the length required to fill the columns (given a prefix of 4 chars)
        std::string larger_str(num_cols,'x'); // larger enough

        for (unsigned int i=0; i<10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            OPERA_LOG_SCOPE_PRINTHOLD(exactly_str);
        }

        for (unsigned int i=0; i<10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            OPERA_LOG_SCOPE_PRINTHOLD(larger_str);
        }
    }

    void test_hold_multiple() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(4);
        OPERA_LOG_SCOPE_CREATE;
        SizeType u=30;
        for (unsigned int i=0; i<3; ++i) {
            sample_printhold_nested_loop("intermediate",u);
            std::this_thread::sleep_for(std::chrono::milliseconds(u));
            OPERA_LOG_SCOPE_PRINTHOLD("external@"<<i);
        }
    }

    void test_light_theme() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(2);
        Logger::instance().configuration().set_theme(TT_THEME_LIGHT);
        std::clog << TT_THEME_LIGHT << std::endl;
        OPERA_LOG_PRINTLN("This is a call on level 1");
        OPERA_LOG_RUN_AT(0,sample_function());
        OPERA_LOG_PRINTLN("This is again a call on level 1");
        Logger::instance().configuration().set_theme(TT_THEME_NONE);
    }

    void test_dark_theme() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(2);
        Logger::instance().configuration().set_theme(TT_THEME_DARK);
        std::clog << TT_THEME_DARK << std::endl;
        OPERA_LOG_PRINTLN("This is a call on level 1");
        OPERA_LOG_RUN_AT(0,sample_function());
        OPERA_LOG_PRINTLN("This is again a call on level 1");
    }

    void test_theme_custom_keyword() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(1);
        Logger::instance().configuration().add_custom_keyword("first");
        Logger::instance().configuration().add_custom_keyword("second",TT_STYLE_CREAM);
        OPERA_LOG_PRINTLN("This is a first custom keyword and a custom styled second one.");
    }

    void test_theme_bgcolor_bold_underline() {
        Logger::instance().use_immediate_scheduler();
        std::list<TerminalTextStyle> styles;
        styles.push_back(TerminalTextStyle(0,0,true,false));
        styles.push_back(TerminalTextStyle(0,0,false,true));
        styles.push_back(TerminalTextStyle(0,0,true,true));
        styles.push_back(TerminalTextStyle(0,88,false,false));
        styles.push_back(TerminalTextStyle(0,88,true,false));
        styles.push_back(TerminalTextStyle(0,88,false,true));
        styles.push_back(TerminalTextStyle(0,88,true,true));
        std::ostringstream ss;
        for (auto style: styles) {
            ss << style() << "x" << TerminalTextStyle::RESET << " ";
        }
        std::clog << ss.str() << std::endl;
    }

    void test_redirect() {
        Logger::instance().use_immediate_scheduler();
        Logger::instance().configuration().set_verbosity(1);
        OPERA_LOG_PRINTLN("This is call 1");
        Logger::instance().redirect_to_file("log.txt");
        OPERA_LOG_PRINTLN("This is call 2");
        OPERA_LOG_PRINTLN("This is call 3");
        Logger::instance().redirect_to_console();
        OPERA_LOG_PRINTLN("This is call 4");
        OPERA_LOG_PRINTLN("This is call 5");

        std::string line;
        std::ifstream file("log.txt");
        unsigned int count = 0;
        if(file.is_open()) {
            while(!file.eof()) {
                getline(file,line);
                count++;
            }
            file.close();
        }
        OPERA_TEST_EQUALS(count,3);
    }

    void test_multiple_threads_with_blocking_scheduler() {
        Logger::instance().use_blocking_scheduler();
        Logger::instance().configuration().set_verbosity(3);
        Logger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::BEFORE);
        OPERA_LOG_PRINTLN("Printing on the " << Logger::instance().current_thread_name() << " thread without other threads");
        OPERA_TEST_EQUALS(Logger::instance().cached_last_printed_thread_name().compare("main"),0);
        Thread thread1([] { print_something1(); },"thr1");
        Thread thread2([] { print_something2(); },"thr2");
        OPERA_LOG_PRINTLN("Printing again on the main thread, but with other threads");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        OPERA_TEST_PRINT(Logger::instance().cached_last_printed_thread_name());
        OPERA_TEST_ASSERT(Logger::instance().cached_last_printed_thread_name().compare("thr1") == 0 or
                                    Logger::instance().cached_last_printed_thread_name().compare("thr2") == 0);
    }

    void test_multiple_threads_with_nonblocking_scheduler() {
        Logger::instance().use_nonblocking_scheduler();
        Logger::instance().configuration().set_theme(TT_THEME_DARK);
        Logger::instance().configuration().set_verbosity(3);
        Logger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::BEFORE);

        OPERA_LOG_PRINTLN("Printing on the " << Logger::instance().current_thread_name() << " thread without other threads");
        Thread thread1([] { print_something1(); });
        Thread thread2([] { print_something1(); });
        Thread thread3([] { print_something1(); });
        Thread thread4([] { print_something1(); });
        Thread thread5([] { print_something1(); });
        Thread thread6([] { print_something1(); });
        OPERA_LOG_PRINTLN("Printing again on the main thread, but with other threads");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    void test_register_self_thread() {
        Logger::instance().use_blocking_scheduler();
        Logger::instance().configuration().set_verbosity(3);
        Logger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::BEFORE);
        OPERA_LOG_PRINTLN("Printing on the " << Logger::instance().current_thread_name() << " thread without other threads");
        OPERA_TEST_EQUALS(Logger::instance().cached_last_printed_thread_name().compare("main"),0);
        std::thread::id thread_id;
        std::thread thread1([&thread_id] { thread_id = std::this_thread::get_id(); Logger::instance().register_self_thread("thr1",1); print_something1(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        thread1.join();
        Logger::instance().unregister_thread(thread_id);
    }

    void test_thread_name_printing_policy() {
        OPERA_PRINT_TEST_COMMENT("Policies: " << ThreadNamePrintingPolicy::BEFORE << " " << ThreadNamePrintingPolicy::AFTER << " " << ThreadNamePrintingPolicy::NEVER)
        Logger::instance().use_blocking_scheduler();
        Logger::instance().configuration().set_verbosity(3);
        Logger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::BEFORE);
        Thread thread1([] { print_something1(); },"thr1");
        OPERA_LOG_PRINTLN("Printing thread name before");
        OPERA_LOG_PRINTLN("Printing thread name before again");
        Logger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::AFTER);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        Thread thread2([] { print_something1(); },"thr2");
        OPERA_LOG_PRINTLN("Printing thread name after");
        OPERA_LOG_PRINTLN("Printing thread name after again");
        Logger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::NEVER);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        OPERA_LOG_PRINTLN("Printing thread name never");
    }

    void test_set_prints_level_on_change_only() {
        Logger::instance().use_blocking_scheduler();
        Logger::instance().configuration().set_verbosity(3);
        Logger::instance().configuration().set_prints_level_on_change_only(true);
        Logger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::BEFORE);
        Thread thread1([] { print_something1(); },"thr1");
        OPERA_LOG_PRINTLN("Printing thread name before");
        OPERA_LOG_PRINTLN("Printing thread name before again");
        Logger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::AFTER);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        Thread thread2([] { print_something1(); },"thr2");
        OPERA_LOG_PRINTLN("Printing thread name after");
        OPERA_LOG_PRINTLN("Printing thread name after again");
        Logger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::NEVER);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        OPERA_LOG_PRINTLN("Printing thread name never");
    }
};

int main() {

    TestLogging().test();

    return OPERA_TEST_FAILURES;
}

