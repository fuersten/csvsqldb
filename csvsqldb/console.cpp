//
//  console.cpp
//  csvsqldb
//
// Copyright (C) 2015  Lars-Christian Fürstenberg
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "console.h"

#include "libcsvsqldb/base/exception.h"
#include "libcsvsqldb/base/string_helper.h"

#include <iostream>

#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>


namespace csvsqldb
{

    struct Console::Private {
        Private(const std::string& prompt)
        : _prompt(prompt)
        , _continue(true)
        , _ready(false)
        {
            _me = this;

            if(::pipe(_pipeFD) < 0) {
                CSVSQLDB_THROW(Exception, "could not initialize pipe");
            }
        }

        ~Private()
        {
            rl_callback_handler_remove();
        }

        void onSignal()
        {
            // write to pipe in order to trigger the select
            const char buf[] = "stop";
            ::write(_pipeFD[1], buf, 1);
            _continue = false;
        }

        static void process_line(char* lineRead)
        {
            // TODO LCF: find a better solution
            _me->do_process_line(lineRead);
        }

        void do_process_line(char* lineRead)
        {
            if(lineRead) {
                _line = lineRead;
                free(lineRead);
            } else {
                _line.clear();
                std::cout << std::endl;
            }
            rl_callback_handler_remove();
            _ready = true;
        }

        void addToHistory(const std::string& line)
        {
            if(!line.empty()) {
                int index = history_search_pos(line.c_str(), 0, 0);
                if(index >= 0) {
                    HIST_ENTRY* entry = remove_history(index);
#if RL_READLINE_VERSION >= 0x0500
                    free_history_entry(entry);
#else
                    free(entry);
#endif
                }
                add_history(line.c_str());
            }
        }

        std::string getLine()
        {
            rl_callback_handler_install(_prompt.c_str(), &Console::Private::process_line);
            while(_continue && !_ready) {
                fd_set fds;
                int maxFD = std::max(::fileno(rl_instream), _pipeFD[0]);

                FD_ZERO(&fds);
                FD_SET(0, &fds);
                FD_SET(_pipeFD[0], &fds);

                select(maxFD + 1, &fds, NULL, NULL, NULL);

                for(int n = 0; n <= maxFD; ++n) {
                    if(FD_ISSET(n, &fds) && n == _pipeFD[0]) {
                        // pipe was written => quit
                        _line.clear();
                        rl_callback_handler_remove();
                        break;
                    } else if(FD_ISSET(::fileno(rl_instream), &fds) && _continue) {
                        rl_callback_read_char();
                        break;
                    }
                }
            }
            // remove closing semicolons
            while(_line.back() == ';') {
                _line.pop_back();
            }

            _ready = false;
            return _line;
        }

        std::string _prompt;
        std::string _line;
        bool _continue;
        bool _ready;
        int _pipeFD[2];
        static Private* _me;
    };

    Console::Private* Console::Private::_me = nullptr;


    Console::Console(const std::string& prompt, const fs::path& historyPath, SignalHandler& signalHandler)
    : _guard(SIGINT, &signalHandler, this)
    , _historyPath(historyPath)
    , _p(new Private(prompt))
    {
        rl_initialize();
        using_history();
        stifle_history(40);
        read_history(_historyPath.c_str());
    }

    Console::~Console()
    {
        write_history(_historyPath.c_str());
    }

    int Console::onSignal(int signum)
    {
        _p->onSignal();
        return 0;
    }

    void Console::run()
    {
        std::string line;
        while(_p->_continue) {
            line = _p->getLine();
            if(line.empty()) {
                std::cout << std::endl;
                continue;
            }

            csvsqldb::StringVector params;
            csvsqldb::split(line, ' ', params, false);

            Commands::iterator it = _commands.find(csvsqldb::tolower_copy(*(params.begin())));
            if(it != _commands.end()) {
                // remove first entry as it is the command name
                params.erase(params.begin());
                if(it->second(params)) {
                    _p->addToHistory(line);
                }
            } else if(_defaultCommand) {
                if(_defaultCommand(line)) {
                    _p->addToHistory(line);
                }
            }
        }
    }

    void Console::stop()
    {
        _p->onSignal();
    }

    void Console::addCommand(const std::string& command, CommandFunction function)
    {
        _commands.insert(std::make_pair(csvsqldb::tolower_copy(command), function));
    }

    void Console::addDefault(DefaultCommandFunction function)
    {
        _defaultCommand = function;
    }

    void Console::clearHistory()
    {
        clear_history();
    }
}
