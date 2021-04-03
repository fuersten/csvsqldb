//
//  signalhandler.cpp
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

#include "base/signalhandler.h"

#include "base/exception.h"
#include "base/logging.h"

#include <map>
#include <mutex>
#include <pthread.h>
#include <signal.h>
#include <thread>
#include <unistd.h>


namespace csvsqldb
{
  static std::mutex signal_mutex;
  static sigset_t signal_mask;

  class SignalHandlerThreadObject
  {
  public:
    SignalHandlerThreadObject(SignalHandler* sighandler)
    : _quit(false)
    , _sighandler(sighandler)
    {
    }

    void run()
    {
      int sig_caught = 0;

      while (!_quit) {
        if (::sigwait(&signal_mask, &sig_caught) != -1) {
          if (!_quit) {
            switch (sig_caught) {
              case SIGTERM:
              case SIGINT:
              case SIGQUIT:
              case SIGHUP:
              case SIGALRM:
              case SIGUSR1:
              case SIGUSR2: {
                CSVSQLDB_CLASSLOG(SignalHandler, 1, "Signal " << sig_caught << " intercepted.");
                for (auto& handler : _sighandler->handler(sig_caught)) {
                  if (handler->onSignal(sig_caught) < 0) {
                    break;
                  }
                }
                break;
              }
              case 0:
                // just ignore this signal
                break;
              default:
                CSVSQLDB_ERRORLOG("Unhandled signal caught " << sig_caught);
                break;
            }
          }
        } else {
          CSVSQLDB_ERRORLOG("SignalHandler: " << errnoText());
          break;
        }
      }
    }

    bool _quit;
    SignalHandler* _sighandler;
  };

  struct SignalHandler::Private {
    Private(SignalHandler* sighandler)
    : _stopSignalHandling(false)
    , _threadObject(sighandler)
    {
    }

    ~Private()
    {
      _threadObject._quit = true;
      ::kill(getpid(), SIGINT);
      _handlerThread.join();
    }

    void start()
    {
      _handlerThread = std::thread(std::bind(&SignalHandlerThreadObject::run, &_threadObject));
    }

    std::map<int, SignalHandler::SignalEventHandlerList> _handlerMap;
    volatile bool _stopSignalHandling;
    SignalHandlerThreadObject _threadObject;
    std::thread _handlerThread;
  };


  SignalHandler::SignalHandler()
  : _p(new Private(this))
  {
    sigemptyset(&signal_mask);
    sigaddset(&signal_mask, SIGTERM);
    sigaddset(&signal_mask, SIGINT);
    sigaddset(&signal_mask, SIGQUIT);
    sigaddset(&signal_mask, SIGHUP);
    sigaddset(&signal_mask, SIGALRM);
    sigaddset(&signal_mask, SIGUSR1);
    sigaddset(&signal_mask, SIGUSR2);

    /* any newly created threads inherit the signal mask */
    if (::pthread_sigmask(SIG_BLOCK, &signal_mask, NULL) != 0) {
      throwSysError("SignalHandler");
    }

    _p->start();
  }

  SignalHandler::~SignalHandler()
  {
    // Needs to be specified due to the unique_ptr impl.
  }

  void SignalHandler::addHandler(int signum, SignalEventHandler* handler)
  {
    std::unique_lock guard(signal_mutex);

    auto iter = _p->_handlerMap.find(signum);
    if (iter == _p->_handlerMap.end()) {
      SignalEventHandlerList list;
      list.push_back(handler);
      _p->_handlerMap.insert(std::make_pair(signum, list));
    } else {
      iter->second.push_back(handler);
    }
  }

  void SignalHandler::removeHandler(int signum, SignalEventHandler* handler)
  {
    std::unique_lock guard(signal_mutex);

    auto iter = _p->_handlerMap.find(signum);
    if (iter != _p->_handlerMap.end()) {
      iter->second.remove(handler);
    }
  }

  SignalHandler::SignalEventHandlerList SignalHandler::handler(int signum) const
  {
    std::unique_lock guard(signal_mutex);

    auto iter = _p->_handlerMap.find(signum);
    if (iter != _p->_handlerMap.end()) {
      return iter->second;
    }

    return SignalHandler::SignalEventHandlerList();
  }
}
