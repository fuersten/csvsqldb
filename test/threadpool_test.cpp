//
//  csvsqldb test
//
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


#include "libcsvsqldb/base/exception.h"
#include "libcsvsqldb/base/thread_pool.h"

#include "test.h"
#include <condition_variable>

#include <atomic>
#include <mutex>
#include <random>


namespace
{
  std::condition_variable g_condition;
  static std::mutex g_mutex;
}

class CallObject
{
public:
  void testit(std::atomic_int& count)
  {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 10);
    auto sleep_time = std::bind(distribution, generator);

    std::this_thread::sleep_for(std::chrono::microseconds(sleep_time()));

    ++count;
    if (count.load() == 5) {
      g_condition.notify_one();
    }
  }
};


class ThreadPoolTestCase
{
public:
  void setUp()
  {
  }

  void tearDown()
  {
  }

  void threadpoolTest()
  {
    std::atomic<int> count(0);

    csvsqldb::ThreadPool tp(3);
    CallObject o;

    tp.start();

    tp.enqueueTask(std::bind(&CallObject::testit, o, std::ref(count)));
    tp.enqueueTask(std::bind(&CallObject::testit, o, std::ref(count)));
    tp.enqueueTask(std::bind(&CallObject::testit, o, std::ref(count)));
    tp.enqueueTask(std::bind(&CallObject::testit, o, std::ref(count)));
    tp.enqueueTask(std::bind(&CallObject::testit, o, std::ref(count)));

    std::unique_lock<std::mutex> condition_guard(g_mutex);
    g_condition.wait_for(condition_guard, std::chrono::milliseconds(150), [&] { return count.load() == 5; });

    MPF_TEST_ASSERTEQUAL(5, count.load());

    tp.stop();
  }

  void alreadyStartedTest()
  {
    csvsqldb::ThreadPool tp(3);
    tp.start();
    MPF_TEST_EXPECTS(tp.start(), csvsqldb::InvalidOperationException);
  }

  void enqueueWithStopTest()
  {
    csvsqldb::ThreadPool tp(3);
    tp.start();
    tp.stop();
    CallObject o;
    std::atomic<int> count(0);

    MPF_TEST_EXPECTS(tp.enqueueTask(std::bind(&CallObject::testit, o, std::ref(count))), csvsqldb::InvalidOperationException);
  }
};

MPF_REGISTER_TEST_START("ThreadPoolTestSuite", ThreadPoolTestCase);
MPF_REGISTER_TEST(ThreadPoolTestCase::threadpoolTest);
MPF_REGISTER_TEST(ThreadPoolTestCase::alreadyStartedTest);
MPF_REGISTER_TEST(ThreadPoolTestCase::enqueueWithStopTest);
MPF_REGISTER_TEST_END();
