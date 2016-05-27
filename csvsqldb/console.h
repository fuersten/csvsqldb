//
//  console.h
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

#ifndef csvsqldb_console_h
#define csvsqldb_console_h

#include "libcsvsqldb/inc.h"

#include "types.h"

#include <map>
#include <memory>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;


namespace csvsqldb
{
    /**
     * A console class to support interactive shells. The implementation is based on the linenoise library.
     */
    class CSVSQLDB_EXPORT Console
    {
    public:
        typedef std::function<bool(const csvsqldb::StringVector&)> CommandFunction;
        typedef std::function<bool(const std::string&)> DefaultCommandFunction;

        Console(const std::string& prompt, const fs::path& historyPath);

        ~Console();

        void run();
        void stop();

        void addCommand(const std::string& command, CommandFunction function);
        void addDefault(DefaultCommandFunction function);

        void clearHistory();

    private:
        typedef std::map<std::string, CommandFunction> Commands;

        fs::path _historyPath;
        Commands _commands;
        DefaultCommandFunction _defaultCommand;
        std::string _prompt;
        bool _stop;
    };
}

#endif
