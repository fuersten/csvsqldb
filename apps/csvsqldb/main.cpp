//
//  main.cpp
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

#include <csvsqldb/base/application.h>
#include <csvsqldb/base/default_configuration.h>
#include <csvsqldb/base/exception.h>
#include <csvsqldb/base/glob.h>
#include <csvsqldb/base/global_configuration.h>
#include <csvsqldb/base/logging.h>
#include <csvsqldb/base/lua_configuration.h>
#include <csvsqldb/base/signalhandler.h>
#include <csvsqldb/base/string_helper.h>
#include <csvsqldb/base/time_measurement.h>
#include <csvsqldb/execution_engine.h>
#include <csvsqldb/version.h>

#include "console.h"

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <fstream>
#include <sstream>
#include <stdio.h>


#define OUT(arg)                                                                                                                 \
  do {                                                                                                                           \
    std::ostringstream XX_ss__;                                                                                                  \
    XX_ss__ << arg;                                                                                                              \
    output(XX_ss__.str());                                                                                                       \
  } while (0);


class CsvDB
{
public:
  CsvDB(csvsqldb::Database& database, bool showHeaderLine, bool verbose, csvsqldb::StringVector files)
  : _database(database)
  , _showHeaderLine(showHeaderLine)
  , _verbose(verbose)
  , _files(std::move(files))
  {
  }

  bool executeSql(const std::string& sql)
  {
    try {
      csvsqldb::ExecutionContext context(_database);
      context._files = _files;
      context._showHeaderLine = _showHeaderLine;

      csvsqldb::ExecutionEngine<csvsqldb::OperatorNodeFactory> engine(context);
      csvsqldb::ExecutionStatistics statistics;

      int64_t rowCount = engine.execute(sql, statistics, std::cout);
      while (rowCount >= 0) {
        OUT("\n[" << rowCount << (rowCount > 1 || rowCount == 0 ? " rows]" : " row]"));

        OUT("\nParsing elapsed time " << statistics._endParsing - statistics._startParsing);
        OUT("Preprocessing elapsed time " << statistics._endPreprocessing - statistics._startPreprocessing);
        OUT("Execution elapsed time " << statistics._endExecution - statistics._startExecution);
        OUT("Total elapsed time " << statistics._endExecution - statistics._startParsing);
        OUT("\nUsed max " << statistics._maxUsedBlocks << " blocks with a total of " << statistics._maxUsedCapacity << " MiB");
        OUT("Total blocks used " << statistics._totalBlocks);

        rowCount = engine.execute(statistics, std::cout);
      }
    } catch (const std::exception& ex) {
      std::cout << "ERROR: " << ex.what() << "\n";
    }
    return true;
  }

  void setVerbosity(bool verbosity)
  {
    _verbose = verbosity;
  }

  bool getVerbosity() const
  {
    return _verbose;
  }

  bool addFile(const std::string& csvFile)
  {
    if (std::find(_files.begin(), _files.end(), csvFile) == _files.end()) {
      _files.push_back(csvFile);
      return true;
    }
    return false;
  }

  const csvsqldb::StringVector& getFiles() const
  {
    return _files;
  }

private:
  void output(const std::string& message)
  {
    if (_verbose) {
      std::cout << message << std::endl;
    }
  }

  csvsqldb::Database& _database;
  bool _showHeaderLine;
  bool _verbose;
  csvsqldb::StringVector _files;
};


class CSVDBGlobalConfiguration : public csvsqldb::GlobalConfiguration
{
public:
  virtual void doConfigure(const csvsqldb::Configuration::Ptr&)
  {
    if (logging.device == "None") {
      logging.device = "Console";
    }
  }
};


class CSVDBApp
: public csvsqldb::Application
, public csvsqldb::SignalEventHandler
{
public:
  CSVDBApp(int argc, char** argv)
  : csvsqldb::Application(argc, argv)
  , _databasePath("./.csvdb")
  , _showHeaderLine(true)
  , _verbose(false)
  , _interactive(false)
  {
    csvsqldb::GlobalConfiguration::create<CSVDBGlobalConfiguration>();
    try {
      csvsqldb::config<CSVDBGlobalConfiguration>()->configure(std::make_shared<csvsqldb::LuaConfiguration>("./csvdb_config.lua"));
    } catch (csvsqldb::FilesystemException&) {
      // no configuration specified, go on with some sane defaults
      csvsqldb::config<CSVDBGlobalConfiguration>()->configure(std::make_shared<csvsqldb::DefaultConfiguration>());
    }
    csvsqldb::Logging::init();
  }

  int onSignal(int signum) override
  {
    if (signum == SIGINT || signum == SIGTERM) {
      // TODO LCF: here we should stop the engine and terminate
    }
    return 0;
  }

private:
  static std::string version()
  {
    std::stringstream ss;
    ss << "csvsqldb tool version " << CSVSQLDB_VERSION_STRING << std::endl;
    ss << CSVSQLDB_COPYRIGHT_STRING << std::endl;
    return ss.str();
  }

  bool setUp(int argc, char** argv) override
  {
    std::string showHeader("on");
    std::string mapping;
    CLI::App app{version()};

    app.add_flag("-v,--version", "Display program version information and exit");
    app.add_flag("-i,--interactive", _interactive, "Opens an interactive sql shell");
    app.add_flag("-V,--verbose", _verbose, "Output verbose statistics");
    app.add_option("--show-header-line", showHeader, "If set to 'on' outputs a header line");
    app.add_option("-p,--datbase-path", _databasePath, "Path to the database");
    auto command_file_option = app.add_option("-c,--command-file", _commandFile, "Command file with sql commands to process");
    app.add_option("-s,--sql", _sql, "Sql commands to call")->excludes(command_file_option);
    app.add_option("-m,--mapping", mapping, "Mapping from csv file to table");
    app.add_option("files,-f,--files", _files, "Csv files to process, can use expansion patterns like ~ or *");

    try {
      app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
      app.exit(e);
      return false;
    }

    if (app.count("--version")) {
      std::cout << version() << std::endl;
      return false;
    }

    if (app.count("--show-header-line")) {
      _showHeaderLine = csvsqldb::toupper_copy(showHeader) == "ON";
    }

    csvsqldb::fs::path path(_databasePath);
    if (path.filename() != ".csvdb") {
      path /= ".csvdb";
    }

    std::error_code ec;
    _databasePath = csvsqldb::fs::canonical(csvsqldb::fs::absolute(path, ec), ec).string();
    if (ec) {
      _databasePath = ".csvdb";
    }

    if (app.count("--mapping")) {
      csvsqldb::StringVector mappings;
      csvsqldb::split(mapping, ';', mappings);
      csvsqldb::FileMapping::Mappings map;
      for (const auto& s : mappings) {
        map.push_back({s, ',', false});
      }
      _mapping.initialize(map);
    }

    csvsqldb::initTypeSystem();

    return true;
  }

  void output(const std::string& message)
  {
    if (_verbose) {
      std::cout << message << std::endl;
    }
  }

  int doRun() override
  {
    OUT("Running csvsqldb tool version " << CSVSQLDB_VERSION_STRING);

    csvsqldb::Database database(_databasePath, _mapping);

    if (!_commandFile.empty()) {
      std::ifstream stream(_commandFile);
      if (!stream) {
        CSVSQLDB_THROW(csvsqldb::BadoptionException, "command file '" << _commandFile << "' could not be opened");
      }
      std::stringstream sql;
      sql << stream.rdbuf();
      _sql = sql.str();
      stream.close();
    } else if (!_files.empty()) {
      OUT("Processing files " << csvsqldb::join(_files, ","));
      OUT("Using table mapping " << csvsqldb::join(_mapping.asStringVector(), ","));
    }

    OUT("");

    CsvDB csvDB(database, _showHeaderLine, _verbose, _files);

    if (!_sql.empty()) {
      csvDB.executeSql(_sql);
      _sql.clear();
    }

    if (_interactive) {
      csvsqldb::Console console("sql> ", "./.csvdb/.history");
      console.addCommand("quit", [&console](const csvsqldb::StringVector&) -> bool {
        console.stop();
        return false;
      });
      console.addCommand("exit", [&console](const csvsqldb::StringVector&) -> bool {
        console.stop();
        return false;
      });
      console.addCommand("help", [](const csvsqldb::StringVector&) -> bool {
        std::cout << "help - this help\n";
        std::cout << "quit|exit - quit shell\n";
        std::cout << "version - show version\n";
        std::cout << "verbose ([on|off]) - show verbosity or switch it on/off\n";
        std::cout << "database - show the database path\n";
        std::cout << "clear history - clear all history entries\n";
        std::cout << "show [tables|mappings|columns <tablename>|functions|files] - show db objects\n";
        std::cout << "add file <path to csv file> - add the specified csv file for processing\n";
        std::cout << "<sql command> - execute the sql\n\n";
        return false;
      });
      console.addCommand("database", [this](const csvsqldb::StringVector&) -> bool {
        std::cout << "database path: " << _databasePath << std::endl;
        return false;
      });
      console.addCommand("version", [](const csvsqldb::StringVector&) -> bool {
        std::cout << version();
        return false;
      });
      console.addCommand("verbose", [&csvDB](const csvsqldb::StringVector& params) -> bool {
        if (!params.empty()) {
          if (csvsqldb::tolower_copy(params[0]) == "on") {
            csvDB.setVerbosity(true);
          } else {
            csvDB.setVerbosity(false);
          }
        } else {
          std::cout << "verbose " << (csvDB.getVerbosity() ? "on" : "off") << std::endl;
        }
        return false;
      });
      console.addCommand("clear", [&console](const csvsqldb::StringVector&) -> bool {
        console.clearHistory();
        return false;
      });
      console.addCommand("add", [&csvDB](const csvsqldb::StringVector& params) -> bool {
        if (!params.empty()) {
          if (csvsqldb::tolower_copy(params[0]) == "file") {
            if (params.size() == 2) {
              csvsqldb::StringVector files;
              csvsqldb::expand(params[1], files);
              size_t count = 0;
              for (const auto& file : files) {
                if (csvDB.addFile(file)) {
                  ++count;
                }
              }
              std::cout << "added " << count << " new csv files for processing\n";
            } else {
              std::cout << "ERROR: csv file parameter missing\n";
            }
          } else {
            std::cout << "ERROR: unknown parameter for add, expected file\n";
          }
        }
        return true;
      });
      console.addCommand("show", [&database, &csvDB](const csvsqldb::StringVector& params) -> bool {
        if (!params.empty()) {
          if (csvsqldb::tolower_copy(params[0]) == "tables") {
            for (const auto& table : database.getTables()) {
              std::cout << table.name() << "\n";
            }
          } else if (csvsqldb::tolower_copy(params[0]) == "mappings") {
            for (const auto& mapping : database.getMappings()) {
              std::cout << mapping << "\n";
            }
          } else if (csvsqldb::tolower_copy(params[0]) == "columns") {
            if (params.size() == 2) {
              try {
                const csvsqldb::TableData& table = database.getTable(params[1]);
                for (size_t n = 0; n < table.columnCount(); ++n) {
                  std::cout << table.getColumn(n)._name << " : " << typeToString(table.getColumn(n)._type) << "\n";
                }
              } catch (const std::exception& ex) {
                std::cout << "ERROR: " << ex.what() << "\n";
              }
            } else {
              std::cout << "ERROR: tablename parameter missing\n";
            }
          } else if (csvsqldb::tolower_copy(params[0]) == "functions") {
            csvsqldb::FunctionRegistry functionRegistry;
            csvsqldb::initBuildInFunctions(functionRegistry);
            for (const auto& func : functionRegistry.getFunctions()) {
              std::cout << func->getName() << "(";
              bool first = true;
              for (const auto& param : func->getParameterTypes()) {
                if (first) {
                  first = false;
                } else {
                  std::cout << ",";
                }
                std::cout << typeToString(param);
              }
              std::cout << ") -> " << typeToString(func->getReturnType()) << "\n";
            }
          } else if (csvsqldb::tolower_copy(params[0]) == "files") {
            if (!csvDB.getFiles().empty()) {
              std::cout << "csv files for processing:\n";
              for (const auto& file : csvDB.getFiles()) {
                std::cout << file << "\n";
              }
            } else {
              std::cout << "no csv files for processing";
            }
          } else {
            std::cout << "ERROR: unknown parameter for show, expected\n\ttables or mappings or "
                         "columns or functions or files";
          }
          std::cout << "\n";
        } else {
          std::cout << "ERROR: expected parameter for show\n\ttables or mappings or "
                       "columns or functions or files\n";
        }
        return true;
      });
      console.addDefault(std::bind(&CsvDB::executeSql, &csvDB, std::placeholders::_1));
      console.run();
    }

    return 0;
  }

  void tearDown() override
  {
  }

  std::string _databasePath;
  std::string _commandFile;
  std::string _sql;
  csvsqldb::FileMapping _mapping;
  bool _showHeaderLine;
  bool _verbose;
  bool _interactive;
  csvsqldb::StringVector _files;
};


int main(int argc, char** argv)
{
  csvsqldb::SignalHandler sighandler;
  CSVDBApp csvdb(argc, argv);
  csvsqldb::SetUpSignalEventHandler guard(SIGINT, &sighandler, &csvdb);

  return csvdb.run();
}
