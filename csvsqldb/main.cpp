//
//  main.cpp
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

#include "libcsvsqldb/base/application.h"
#include "libcsvsqldb/base/default_configuration.h"
#include "libcsvsqldb/base/exception.h"
#include "libcsvsqldb/base/glob.h"
#include "libcsvsqldb/base/global_configuration.h"
#include "libcsvsqldb/base/logging.h"
#include "libcsvsqldb/base/lua_configuration.h"
#include "libcsvsqldb/base/signalhandler.h"
#include "libcsvsqldb/base/string_helper.h"
#include "libcsvsqldb/base/time_measurement.h"

#include "libcsvsqldb/execution_engine.h"
#include "libcsvsqldb/version.h"

#include <boost/program_options.hpp>

#include <fstream>
#include <sstream>

#include <stdio.h>


namespace po = boost::program_options;


#define OUT(arg)                                                                                                                 \
    do {                                                                                                                         \
        std::ostringstream XX_ss__;                                                                                              \
        XX_ss__ << arg;                                                                                                          \
        output(XX_ss__.str());                                                                                                   \
    } while(0);


class CsvDB
{
public:
    CsvDB(csvsqldb::Database& database, bool showHeaderLine, bool verbose, csvsqldb::StringVector files)
    : _database(database)
    , _showHeaderLine(showHeaderLine)
    , _verbose(verbose)
    , _files(files)
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
            while(rowCount >= 0) {
                OUT("\n[" << rowCount << (rowCount > 1 || rowCount == 0 ? " rows]" : " row]"));

                OUT("\nParsing elapsed time " << statistics._endParsing - statistics._startParsing);
                OUT("Preprocessing elapsed time " << statistics._endPreprocessing - statistics._startPreprocessing);
                OUT("Execution elapsed time " << statistics._endExecution - statistics._startExecution);
                OUT("Total elapsed time " << statistics._endExecution - statistics._startParsing);
                OUT("\nUsed max " << statistics._maxUsedBlocks << " blocks with a total of " << statistics._maxUsedCapacity
                                  << " MiB");
                OUT("Total blocks used " << statistics._totalBlocks);

                rowCount = engine.execute(statistics, std::cout);
            }
        } catch(const std::exception& ex) {
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
        if(std::find(_files.begin(), _files.end(), csvFile) == _files.end()) {
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
        if(_verbose) {
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
        if(logging.device == "None") {
            logging.device = "Console";
        }
    }
};


class CSVDBApp : public csvsqldb::Application, public csvsqldb::SignalEventHandler
{
public:
    CSVDBApp(int argc, char** argv, csvsqldb::SignalHandler& signalHandler)
    : csvsqldb::Application(argc, argv)
    , _databasePath("./.csvdb")
    , _showHeaderLine(true)
    , _verbose(false)
    , _interactive(false)
    , _signalHandler(signalHandler)
    {
        csvsqldb::GlobalConfiguration::create<CSVDBGlobalConfiguration>();
        try {
            csvsqldb::config<CSVDBGlobalConfiguration>()->configure(
            std::make_shared<csvsqldb::LuaConfiguration>("./csvdb_config.lua"));
        } catch(csvsqldb::FilesystemException&) {
            // no configuration specified, go on with some sane defaults
            csvsqldb::config<CSVDBGlobalConfiguration>()->configure(std::make_shared<csvsqldb::DefaultConfiguration>());
        }
        csvsqldb::Logging::init();
    }

    virtual int onSignal(int signum)
    {
        if(signum == SIGINT || signum == SIGTERM) {
            // TODO LCF: here we should stop the engine and terminate
        }
        return 0;
    }

private:
    typedef std::unique_ptr<csvsqldb::Console> ConsolePtr;

    void printVersion()
    {
        std::cout << "csvsqldb tool version " << CSVSQLDB_VERSION_STRING << std::endl;
        std::cout << CSVSQLDB_COPYRIGHT_STRING << std::endl;
    }

    virtual bool setUp(int argc, char** argv)
    {
        std::string showHeader("on");

        // clang-format off
        po::options_description desc("Options");
        desc.add_options()("help", "shows this help")
        ("version", "shows the version of the program")
        ("interactive,i", "opens an interactive sql shell")
        ("verbose,v", "output verbose statistics")
        ("show-header-line", po::value<std::string>(&showHeader), "if set to 'on' outputs a header line")
        ("datbase-path,p", po::value<std::string>(&_databasePath), "path to the database")
        ("command-file,c", po::value<std::string>(&_commandFile), "command file with sql commands to process")
        ("sql,s", po::value<std::string>(&_sql), "sql commands to call")
        ("mapping,m", po::value<csvsqldb::StringVector>()->composing(), "mapping from csv file to table")
        ("files,f", po::value<std::vector<std::string>>(&_files), "csv files to process, can use expansion patterns like ~ or *");
        // clang-format on

        po::positional_options_description p;
        p.add("files", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if(vm.count("help")) {
            printVersion();
            std::cout << desc << std::endl;
            return false;
        }
        if(vm.count("version")) {
            printVersion();
            return false;
        }
        if(vm.count("sql") && vm.count("command-file")) {
            CSVSQLDB_THROW(csvsqldb::BadoptionException, "not allowed to specify 'sql' and 'command-file' option");
        }
        if(vm.count("interactive")) {
            _interactive = true;
        }
        fs::path path(_databasePath);
        if(vm.count("datbase-path")) {
            path = vm["datbase-path"].as<std::string>();
            if(path.filename() != ".csvdb") {
                path /= ".csvdb";
            }
        }
        _databasePath = fs::absolute(path).normalize().string();
        if(vm.count("verbose")) {
            _verbose = true;
        }
        if(vm.count("show-header-line")) {
            _showHeaderLine = csvsqldb::toupper_copy(vm["show-header-line"].as<std::string>()) == "ON";
        }
        if(vm.count("mapping")) {
            csvsqldb::StringVector mapping;
            for(const auto& part : vm["mapping"].as<std::vector<std::string>>()) {
                csvsqldb::split(part, ';', mapping);
            }
            csvsqldb::FileMapping::Mappings map;
            for(const auto& s : mapping) {
                map.push_back({ s, ',', false });
            }
            _mapping.initialize(map);
        }

        csvsqldb::initTypeSystem();

        return true;
    }

    void output(const std::string& message)
    {
        if(_verbose) {
            std::cout << message << std::endl;
        }
    }

    virtual int doRun()
    {
        OUT("Running csvsqldb tool version " << CSVSQLDB_VERSION_STRING);

        csvsqldb::Database database(_databasePath, _mapping);
        database.setUp();

        if(!_commandFile.empty()) {
            std::ifstream stream(_commandFile);
            if(!stream) {
                CSVSQLDB_THROW(csvsqldb::BadoptionException, "command file '" << _commandFile << "' could not be opened");
            }
            std::stringstream sql;
            sql << stream.rdbuf();
            _sql = sql.str();
            stream.close();
        } else if(_files.size()) {
            OUT("Processing files " << csvsqldb::join(_files, ","));
            OUT("Using table mapping " << csvsqldb::join(_mapping.asStringVector(), ","));
        }

        OUT("");

        CsvDB csvDB(database, _showHeaderLine, _verbose, _files);

        if(!_sql.empty()) {
            csvDB.executeSql(_sql);
            _sql.clear();
        }

        if(_interactive) {
            csvsqldb::Console console("sql> ", "./.csvdb/.history", _signalHandler);
            console.addCommand("quit",
                               [&console](const csvsqldb::StringVector&) -> bool {
                                   console.stop();
                                   return false;
                               });
            console.addCommand("exit",
                               [&console](const csvsqldb::StringVector&) -> bool {
                                   console.stop();
                                   return false;
                               });
            console.addCommand("help",
                               [](const csvsqldb::StringVector&) -> bool {
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
            console.addCommand("database",
                               [this](const csvsqldb::StringVector&) -> bool {
                                   std::cout << "database path: " << _databasePath << std::endl;
                                   return false;
                               });
            console.addCommand("version",
                               [this](const csvsqldb::StringVector&) -> bool {
                                   printVersion();
                                   return false;
                               });
            console.addCommand("verbose",
                               [&csvDB](const csvsqldb::StringVector& params) -> bool {
                                   if(params.size()) {
                                       if(csvsqldb::tolower_copy(params[0]) == "on") {
                                           csvDB.setVerbosity(true);
                                       } else {
                                           csvDB.setVerbosity(false);
                                       }
                                   } else {
                                       std::cout << "verbose " << (csvDB.getVerbosity() ? "on" : "off") << std::endl;
                                   }
                                   return false;
                               });
            console.addCommand("clear",
                               [&console](const csvsqldb::StringVector&) -> bool {
                                   console.clearHistory();
                                   return false;
                               });
            console.addCommand("add",
                               [&csvDB](const csvsqldb::StringVector& params) -> bool {
                                   if(params.size()) {
                                       if(csvsqldb::tolower_copy(params[0]) == "file") {
                                           if(params.size() == 2) {
                                               csvsqldb::StringVector files;
                                               csvsqldb::expand(params[1], files);
                                               size_t count = 0;
                                               for(const auto& file : files) {
                                                   if(csvDB.addFile(file)) {
                                                       ++count;
                                                   }
                                               }
                                               std::cout << "added " << count << " new csv files for processing\n";
                                           } else {
                                               std::cout << "ERROR: csv file parameter missing\n";
                                           }
                                       }
                                   }
                                   return true;
                               });
            console.addCommand("show",
                               [&database, &csvDB](const csvsqldb::StringVector& params) -> bool {
                                   if(params.size()) {
                                       if(csvsqldb::tolower_copy(params[0]) == "tables") {
                                           csvsqldb::Database::Tables tables;
                                           database.getTables(tables);
                                           for(const auto& table : tables) {
                                               std::cout << table.name() << "\n";
                                           }
                                       } else if(csvsqldb::tolower_copy(params[0]) == "mappings") {
                                           csvsqldb::StringVector mappings;
                                           database.getMappings(mappings);
                                           for(const auto& mapping : mappings) {
                                               std::cout << mapping << "\n";
                                           }
                                       } else if(csvsqldb::tolower_copy(params[0]) == "columns") {
                                           if(params.size() == 2) {
                                               try {
                                                   const csvsqldb::TableData& table = database.getTable(params[1]);
                                                   for(size_t n = 0; n < table.columnCount(); ++n) {
                                                       std::cout << table.getColumn(n)._name << " : "
                                                                 << typeToString(table.getColumn(n)._type) << "\n";
                                                   }
                                               } catch(const std::exception& ex) {
                                                   std::cout << "ERROR: " << ex.what() << "\n";
                                               }
                                           } else {
                                               std::cout << "ERROR: tablename parameter missing\n";
                                           }
                                       } else if(csvsqldb::tolower_copy(params[0]) == "functions") {
                                           csvsqldb::FunctionRegistry functionRegistry;
                                           csvsqldb::initBuildInFunctions(functionRegistry);
                                           csvsqldb::FunctionRegistry::FunctionVector functions;
                                           functionRegistry.getFunctions(functions);
                                           for(const auto& func : functions) {
                                               std::cout << func->getName() << "(";
                                               bool first = true;
                                               for(const auto& param : func->getParameterTypes()) {
                                                   if(first) {
                                                       first = false;
                                                   } else {
                                                       std::cout << ",";
                                                   }
                                                   std::cout << typeToString(param);
                                               }
                                               std::cout << ") -> " << typeToString(func->getReturnType()) << "\n";
                                           }
                                       } else if(csvsqldb::tolower_copy(params[0]) == "files") {
                                           std::cout << "csv files for processing:\n";
                                           for(const auto& file : csvDB.getFiles()) {
                                               std::cout << file << "\n";
                                           }
                                       }
                                       std::cout << "\n";
                                   }
                                   return true;
                               });
            console.addDefault(std::bind(&CsvDB::executeSql, &csvDB, std::placeholders::_1));
            console.run();
        }

        return 0;
    }

    std::string _databasePath;
    std::string _commandFile;
    std::string _sql;
    csvsqldb::FileMapping _mapping;
    bool _showHeaderLine;
    bool _verbose;
    bool _interactive;
    csvsqldb::StringVector _files;
    csvsqldb::SignalHandler& _signalHandler;
};


int main(int argc, char** argv)
{
    csvsqldb::SignalHandler sighandler;
    CSVDBApp csvdb(argc, argv, sighandler);
    csvsqldb::SetUpSignalEventHandler guard(SIGINT, &sighandler, &csvdb);

    int ret = csvdb.run();

    return ret;
}
