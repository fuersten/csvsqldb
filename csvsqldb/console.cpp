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

#include "linenoise-ng/linenoise.h"

#include <iostream>
#include <signal.h>


namespace csvsqldb
{
  Console::Console(const std::string& prompt, const fs::path& historyPath)
  : _historyPath(historyPath)
  , _prompt(prompt)
  , _stop(false)
  {
    linenoiseHistoryLoad(_historyPath.c_str());
  }

  Console::~Console()
  {
    linenoiseHistorySave(_historyPath.c_str());
  }

  void Console::run()
  {
    std::string line;
    while (!_stop) {
      char* result = linenoise(_prompt.c_str());
      if (!result) {
        // no more to do
        break;
      }
      line = std::string(result);
      free(result);

      csvsqldb::StringVector params;
      csvsqldb::split(line, ' ', params, false);

      if (params.size()) {
        Commands::iterator it = _commands.find(csvsqldb::tolower_copy(*(params.begin())));
        if (it != _commands.end()) {
          // remove first entry as it is the command name
          params.erase(params.begin());
          if (it->second(params)) {
            linenoiseHistoryAdd(line.c_str());
          }
        } else if (_defaultCommand) {
          if (_defaultCommand(line)) {
            linenoiseHistoryAdd(line.c_str());
          }
        }
      }
    }
  }

  void Console::stop()
  {
    _stop = true;
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
    linenoiseHistoryFree();
  }
}
