//
//  signalhandler.cpp
//  csvsqldb
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

#include "libcsvsqldb/base/signalhandler.h"

#include <algorithm>
#include <map>
#include <mutex>
#include <thread>
#include <windows.h>


namespace csvsqldb
{
  typedef std::map<int, SignalHandler::SignalEventHandlerList> SignalToHandlerMap;

  BOOL WINAPI onWindowsSignal(DWORD dwCtrlType)
  {
    int signum = 0;

    switch (dwCtrlType) {
      case CTRL_C_EVENT:
        signum = SIGINT;
        break;
      case CTRL_BREAK_EVENT:
        signum = SIGINT;
        break;
      case CTRL_CLOSE_EVENT:
        signum = SIGTERM;
        break;
      case CTRL_LOGOFF_EVENT:
        signum = SIGTERM;
        break;
      case CTRL_SHUTDOWN_EVENT:
        signum = SIGTERM;
        break;
    }

    SignalHandler::SignalEventHandlerList list;
    bool callNextHandler = true;
    for (SignalHandler::SignalEventHandlerList::iterator iter = list.begin(); callNextHandler && iter != list.end(); ++iter) {
      callNextHandler = (*iter)->onSignal(signum) >= 0 ? true : false;
    }

    return TRUE;
  }

  static std::mutex signal_mutex;

  struct SignalHandler::Private {
    SignalToHandlerMap _handlerMap;
    volatile bool _stopSignalHandling;
  };


  SignalHandler::SignalHandler()
  : _p(new Private())
  {
    _p->_stopSignalHandling = false;
    SetConsoleCtrlHandler(onWindowsSignal, TRUE);
  }

  SignalHandler::~SignalHandler()
  {
  }

  void SignalHandler::addHandler(int signum, SignalEventHandler* handler)
  {
    std::unique_lock<std::mutex> guard(signal_mutex);

    SignalToHandlerMap::iterator iter = _p->_handlerMap.find(signum);
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
    std::unique_lock<std::mutex> guard(signal_mutex);

    SignalToHandlerMap::iterator iter = _p->_handlerMap.find(signum);
    if (iter != _p->_handlerMap.end()) {
      SignalEventHandlerList& list = iter->second;
      std::remove(list.begin(), list.end(), handler);
    }
  }

  SignalHandler::SignalEventHandlerList SignalHandler::handler(int signum) const
  {
    std::unique_lock<std::mutex> guard(signal_mutex);

    SignalToHandlerMap::iterator iter = _p->_handlerMap.find(signum);
    if (iter != _p->_handlerMap.end()) {
      return iter->second;
    }

    return SignalHandler::SignalEventHandlerList();
  }

  bool SignalHandler::isStopSignalHandling() const
  {
    std::unique_lock<std::mutex> guard(signal_mutex);

    return _p->_stopSignalHandling;
  }

  void SignalHandler::stopSignalHandling()
  {
    std::unique_lock<std::mutex> guard(signal_mutex);

    _p->_stopSignalHandling = true;
  }
}
