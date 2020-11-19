//
//  thread_pool.h
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

#pragma once

#include "libcsvsqldb/inc.h"

#include <condition_variable>

#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


/**
 * The general csvsqldb namespace.
 */
namespace csvsqldb
{
  /**
   * A thread pool with a fixed thread number. Arbitrary <tt>std::function<void()></tt> functions can be enqueued.
   */
  class CSVSQLDB_EXPORT ThreadPool
  {
  public:
    using Callback = std::function<void()>;

    /**
     * Constructs a thread pool with the given number of threads to execute ThreadPool::Callback handler. The threads will be
     * created and started
     * upon calling the start() method.
     * @param numberOfThreads Number of threads to spawn
     */
    explicit ThreadPool(uint16_t numberOfThreads);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    /**
     * Destructs a thread pool. Will call stop, which will wait for thread termination. All things in currently execution will
     * be
     * finished before stopping.
     */
    ~ThreadPool();

    /**
     * Starts the construction of threads and the execution of ThreadPool::Callback handler. Will throw
     * InvalidOperationException if
     * the thread pool was already started. It is possible to call start() again, after stop() was called.
     */
    void start();

    /**
     * Stops the thread pool. Will wait for thread termination. All things currently in execution will be finished before
     * stopping.
     */
    void stop();

    /**
     * Checks if the thread pool was already stopped.
     * @return true if it was stopped already, false if not
     */
    bool isStopped() const
    {
      return _quit.load();
    }

    /**
     * Enqueues a ThreadPool::Callback handler for execution. Will throw InvalidOperationException if the thread pool was
     * already stopped.
     * @param task The ThreadPool::Callback handler to enqueue. This has to be a <tt>std::function<void()></tt> object. This
     * can be created by using the
     * std::bind template.
     *
     * @code{.cpp}
     * ThreadPool tp(3);
     * CallObject o;
     * tp.start();
     * tp.enqueueTask(std::bind(&CallObject::addToSum, o, 4711));
     * @endcode
     */
    void enqueueTask(Callback task);

  private:
    /**
     * The thread entry function.
     */
    void run();

    std::queue<Callback> _taskQueue;
    const uint16_t _numberOfThreads;
    std::vector<std::thread> _serviceThreads;
    std::atomic<bool> _quit;
    std::mutex _taskQueueMutex;
    std::condition_variable _taskQueueCondition;
  };
}