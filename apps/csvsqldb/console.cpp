//
//  console.cpp
//  csvdb
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

#include "console.h"

#include <csvsqldb/base/exception.h>
#include <csvsqldb/base/string_helper.h>

#include <iostream>
#include <linenoise.h>
#include <signal.h>


namespace csvsqldb
{
  Console::Console(const std::string& prompt, const fs::path& historyPath, uint16_t historyLength)
  : _historyPath(historyPath)
  , _prompt(prompt)
  , _stop(false)
  {
    linenoiseHistorySetMaxLen(historyLength);
    linenoiseHistoryLoad(_historyPath.string().c_str());
  }

  Console::~Console()
  {
    linenoiseHistorySave(_historyPath.string().c_str());
    linenoiseHistoryFree();
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
        } else if (_defaultCommand && _defaultCommand(line)) {
          linenoiseHistoryAdd(line.c_str());
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
