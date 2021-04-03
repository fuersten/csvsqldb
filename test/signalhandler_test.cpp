//
//  csvsqldb test
//
//  BSD 3-Clause License
//  Copyright (c) 2015-2020 Lars-Christian Fürstenberg
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


#include <csvsqldb/base/signalhandler.h>

#include <catch2/catch.hpp>

#include <condition_variable>
#include <mutex>
#include <signal.h>
#include <thread>
#include <unistd.h>


namespace
{
  static std::condition_variable _test_condition;

  class TestHandler : public csvsqldb::SignalEventHandler
  {
  public:
    TestHandler()
    : _signalSet(false)
    {
    }

    int onSignal(int signum) override
    {
      if (signum == SIGINT) {
        _signalSet = true;
      }
      _test_condition.notify_one();
      return 0;
    }

    bool _signalSet;
  };

  class CountHandler : public csvsqldb::SignalEventHandler
  {
  public:
    CountHandler(int ret)
    : _ret{ret}
    {}

    int onSignal(int signum) override
    {
      _counter++;
      _test_condition.notify_one();
      return _ret;
    }

    int _counter{0};
    int _ret;
  };
}


TEST_CASE("Signalhandler Test", "[utils]")
{
  csvsqldb::SignalHandler sighandler;
  std::mutex test_mutex;

  SECTION("add and get handler")
  {
    CountHandler handler{0};
    TestHandler testHandler;

    sighandler.addHandler(SIGINT, &handler);
    sighandler.addHandler(SIGTERM, &testHandler);
    CHECK(sighandler.handler(SIGINT).size() == 1);
    CHECK(sighandler.handler(SIGTERM).size() == 1);
    sighandler.addHandler(SIGTERM, &handler);
    CHECK(sighandler.handler(SIGTERM).size() == 2);
    CHECK(sighandler.handler(SIGALRM).empty());

    sighandler.removeHandler(SIGINT, &handler);
    CHECK(sighandler.handler(SIGINT).empty());
    
    CHECK(sighandler.handler(SIGQUIT).empty());
  }
  SECTION("simple signal handler")
  {
    TestHandler tester;
    csvsqldb::SetUpSignalEventHandler guard(SIGINT, &sighandler, &tester);

    ::kill(getpid(), SIGINT);

    std::unique_lock condition_guard(test_mutex);
    _test_condition.wait_for(condition_guard, std::chrono::milliseconds(150), [&] { return tester._signalSet == true; });

    CHECK(tester._signalSet);
  }
  SECTION("add multiple handler and execute")
  {
    CountHandler handler1{0};
    CountHandler handler2{0};

    sighandler.addHandler(SIGINT, &handler1);
    sighandler.addHandler(SIGINT, &handler2);

    ::kill(getpid(), SIGINT);

    std::unique_lock condition_guard(test_mutex);
    _test_condition.wait_for(condition_guard, std::chrono::milliseconds(150), [&] { return handler1._counter > 0 && handler2._counter > 0; });

    CHECK((handler1._counter > 0 && handler2._counter > 0));
  }
  SECTION("add multiple handler and execute one")
  {
    CountHandler handler1{-1};
    CountHandler handler2{0};

    sighandler.addHandler(SIGINT, &handler1);
    sighandler.addHandler(SIGINT, &handler2);

    ::kill(getpid(), SIGINT);

    std::unique_lock condition_guard(test_mutex);
    _test_condition.wait_for(condition_guard, std::chrono::milliseconds(150), [&] { return handler1._counter > 0; });

    CHECK((handler1._counter > 0 && handler2._counter == 0));
  }
}
