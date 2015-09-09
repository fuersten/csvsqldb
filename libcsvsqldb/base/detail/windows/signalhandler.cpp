/***************************************************************************
 *                                                                         *
 *   Copyright (c) 2004-2007 by Athelas GmbH. All Rights Reserved.         *
 *                                                                         *
 *   The contents of this file are subject to the Mozilla Public License   *
 *   Version 1.1 (the "License"); you may not use this file except in      *
 *   compliance with the License. You may obtain a copy of the License at  *
 *   http://www.mozilla.org/MPL/                                           *
 *                                                                         *
 *   Software distributed under the License is distributed on an "AS IS"   *
 *   basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See   *
 *   the License for the specificlanguage governing rights and             *
 *   limitations under the License.                                        *
 *                                                                         *
 *   The Original Code is mpf.                                             *
 *                                                                         *
 *   The Initial Developer of the Original Code is Lars-Christian          *
 *   Fuerstenberg, with contributions from Steffen Schuemann. Portions     *
 *   created by Lars-Christian Fuerstenberg are Copyright (c) 2002-2003    *
 *   All Rights Reserved.                                                  *
 *                                                                         *
 ***************************************************************************/

#include "libcsvsqldb/base/signalhandler.h"

#include <windows.h>

#include <algorithm>
#include <map>
#include <mutex>
#include <thread>


namespace csvsqldb
{

	typedef std::map<int, SignalHandler::SignalEventHandlerList> SignalToHandlerMap;

	BOOL WINAPI onWindowsSignal(DWORD dwCtrlType)
	{
		int signum = 0;

		switch (dwCtrlType)
		{
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
		for (SignalHandler::SignalEventHandlerList::iterator iter = list.begin(); callNextHandler && iter != list.end(); ++iter)
		{
			callNextHandler = (*iter)->onSignal(signum) >= 0 ? true : false;
		}

		return TRUE;
	}

	static std::mutex signal_mutex;

	struct SignalHandler::Private
	{
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
		if (iter == _p->_handlerMap.end())
		{
			SignalEventHandlerList list;
			list.push_back(handler);
			_p->_handlerMap.insert(std::make_pair(signum, list));
		}
		else
		{
			iter->second.push_back(handler);
		}
	}

	void SignalHandler::removeHandler(int signum, SignalEventHandler* handler)
	{
		std::unique_lock<std::mutex> guard(signal_mutex);

		SignalToHandlerMap::iterator iter = _p->_handlerMap.find(signum);
		if (iter != _p->_handlerMap.end())
		{
			SignalEventHandlerList& list = iter->second;
			std::remove(list.begin(), list.end(), handler);
		}
	}

	SignalHandler::SignalEventHandlerList SignalHandler::handler(int signum) const
	{
		std::unique_lock<std::mutex> guard(signal_mutex);

		SignalToHandlerMap::iterator iter = _p->_handlerMap.find(signum);
		if (iter != _p->_handlerMap.end())
		{
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
