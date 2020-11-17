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


#include "libcsvsqldb/base/exception.h"
#include "libcsvsqldb/base/thread_pool.h"

#include <condition_variable>

#include <catch2/catch.hpp>

#include <atomic>
#include <mutex>
#include <random>


namespace
{
  std::condition_variable g_condition;
  static std::mutex g_mutex;

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
}


TEST_CASE("Threadpool Test", "[utils]")
{
  SECTION("threadpool")
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

    CHECK(5 == count.load());

    tp.stop();
  }

  SECTION("already started")
  {
    csvsqldb::ThreadPool tp(3);
    tp.start();
    CHECK_THROWS_AS(tp.start(), csvsqldb::InvalidOperationException);
  }

  SECTION("enqueue with stop")
  {
    csvsqldb::ThreadPool tp(3);
    tp.start();
    tp.stop();
    CallObject o;
    std::atomic<int> count(0);

    CHECK_THROWS_AS(tp.enqueueTask(std::bind(&CallObject::testit, o, std::ref(count))), csvsqldb::InvalidOperationException);
  }
}
