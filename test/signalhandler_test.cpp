//
//  csvsqldb test
//
//  Created by Lars-Christian Fürstenberg on 28.07.13.
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//  conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of
//  conditions and the following disclaimer in the documentation and/or other materials provided
//  with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to
//  endorse or promote products derived from this software without specific prior written
//  permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//


#include "test.h"

#include "libcsvsqldb/base/signalhandler.h"

#include <unistd.h>
#include <signal.h>
#include <condition_variable>
#include <mutex>
#include <thread>


static std::mutex _test_mutex;
static std::condition_variable _test_condition;

class HandlerTester : public csvsqldb::SignalEventHandler
{
public:
    HandlerTester()
    : _signalSet(false)
    {
    }

    virtual int onSignal(int signum)
    {
        if(signum == SIGINT) {
            _signalSet = true;
        }
        _test_condition.notify_one();
        return 0;
    }

    bool _signalSet;
};

class SignalHandlerTestCase
{
public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void testSignalHandler()
    {
        csvsqldb::SignalHandler sighandler;
        HandlerTester tester;
        csvsqldb::SetUpSignalEventHandler guard(SIGINT, &sighandler, &tester);

        ::kill(getpid(), SIGINT);

        std::unique_lock<std::mutex> condition_guard(_test_mutex);
        _test_condition.wait_for(condition_guard, std::chrono::milliseconds(150), [&] { return tester._signalSet == true; });

        MPF_TEST_ASSERTEQUAL(true, tester._signalSet);
    }
};

MPF_REGISTER_TEST_START("ApplicationTestSuite", SignalHandlerTestCase);
MPF_REGISTER_TEST(SignalHandlerTestCase::testSignalHandler);
MPF_REGISTER_TEST_END();
