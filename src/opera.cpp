/***************************************************************************
 *            operad.cpp
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

#include <iostream>
#include <thread>
#include <chrono>
#include <mosquitto.h>

int main() {

    struct mosquitto* mosq = mosquitto_new(nullptr, true, nullptr);

    int rc = mosquitto_connect(mosq, "test.mosquitto.org", 1883, 60);
    if (rc != MOSQ_ERR_SUCCESS){
        mosquitto_destroy(mosq);
        std::cout << "Error connecting: " << mosquitto_strerror(rc) << std::endl;
        return 1;
    }

    mosquitto_loop_start(mosq);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::string payload = "hello sir!";

    rc = mosquitto_publish(mosq, nullptr, "opera", static_cast<int>(payload.size()), payload.c_str(), 2, false);
    if (rc != MOSQ_ERR_SUCCESS){
        std::cout << "Error publishing: " << mosquitto_strerror(rc) << std::endl;
    }

    mosquitto_destroy(mosq);

    return 0;
}
