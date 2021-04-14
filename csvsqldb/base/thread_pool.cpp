//
//  thread_pool.cpp
//  csvsqldb
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

#include "thread_pool.h"

#include "exception.h"

#include <algorithm>

namespace csvsqldb
{
  ThreadPool::ThreadPool(uint16_t numberOfThreads)
  : _numberOfThreads(numberOfThreads)
  , _quit(true)
  {
  }

  ThreadPool::~ThreadPool()
  {
    stop();
  }

  void ThreadPool::start()
  {
    if (!_quit.exchange(false)) {
      throw InvalidOperationException("The thread pool was started.");
    }

    for (int n = 0; n < _numberOfThreads; ++n) {
      _serviceThreads.emplace_back([this]() {
        run();
      });
    }
  }

  void ThreadPool::stop()
  {
    if (!_quit.exchange(true)) {
      _taskQueueCondition.notify_all();
      std::for_each(_serviceThreads.begin(), _serviceThreads.end(), [](std::thread& t) {
        if (t.joinable()) {
          t.join();
        }
      });
    }
  }

  void ThreadPool::enqueueTask(Callback task)
  {
    if (_quit.load()) {
      throw InvalidOperationException("The thread pool was stopped.");
    }
    std::unique_lock guard(_taskQueueMutex);
    _taskQueue.push(task);
    _taskQueueCondition.notify_one();
  }

  void ThreadPool::run()
  {
    while (!_quit.load()) {
      Callback task;

      {
        std::unique_lock guard(_taskQueueMutex);
        _taskQueueCondition.wait_for(guard, std::chrono::milliseconds(150), [&] { return _quit.load() || !_taskQueue.empty(); });
        if (_taskQueue.empty() || _quit.load()) {
          continue;
        }
        task = std::move(_taskQueue.front());
        _taskQueue.pop();
      }

      if (task) {
        task();
      }
    }
  }
}
