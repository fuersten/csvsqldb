//
//  execution_engine.h
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

#ifndef csvsqldb_execution_engine_h
#define csvsqldb_execution_engine_h

#include "libcsvsqldb/inc.h"

#include "base/time_measurement.h"
#include "buildin_functions.h"
#include "database.h"
#include "execution_plan_creator.h"
#include "operatornode.h"
#include "operatornode_factory.h"
#include "sql_parser.h"
#include "validation_visitor.h"


namespace csvsqldb
{
  struct CSVSQLDB_EXPORT ExecutionContext {
    ExecutionContext(Database& database);

    Database& _database;
    csvsqldb::StringVector _files;
    bool _showHeaderLine;
  };

  struct CSVSQLDB_EXPORT ExecutionStatistics {
    csvsqldb::chrono::ProcessTimePoint _startParsing;
    csvsqldb::chrono::ProcessTimePoint _endParsing;
    csvsqldb::chrono::ProcessTimePoint _startPreprocessing;
    csvsqldb::chrono::ProcessTimePoint _endPreprocessing;
    csvsqldb::chrono::ProcessTimePoint _startExecution;
    csvsqldb::chrono::ProcessTimePoint _endExecution;

    size_t _maxUsedBlocks;
    size_t _totalBlocks;
    size_t _maxUsedCapacity;
  };

  template<typename OperatorNodeFactory>
  class ExecutionEngine
  {
  public:
    ExecutionEngine(ExecutionContext& execContext)
    : _execContext(execContext)
    , _parser(_functions)
    , _blockManager(1000)
    {
      initBuildInFunctions(_functions);
    }

    ExecutionEngine(const ExecutionEngine&) = delete;
    ExecutionEngine& operator=(const ExecutionEngine&) = delete;
    ExecutionEngine(ExecutionEngine&&) = delete;
    ExecutionEngine& operator=(ExecutionEngine&&) = delete;

    int64_t execute(const std::string& sql, ExecutionStatistics& statistics, std::ostream& stream)
    {
      _parser.setInput(sql);
      return execute(statistics, stream);
    }

    int64_t execute(ExecutionStatistics& statistics, std::ostream& stream)
    {
      OperatorContext context(_execContext._database, _functions, _blockManager, _execContext._files);
      context._showHeaderLine = _execContext._showHeaderLine;

      statistics._startParsing = csvsqldb::chrono::ProcessTimeClock::now();
      ASTNodePtr astnode = _parser.parse();
      statistics._endParsing = csvsqldb::chrono::ProcessTimeClock::now();

      if (!astnode) {
        // ready
        return -1;
      }

      statistics._startPreprocessing = csvsqldb::chrono::ProcessTimeClock::now();
      ASTValidationVisitor validationVisitor(context._database);
      astnode->accept(validationVisitor);

      ExecutionPlan execPlan;
      ExecutionPlanVisitor<OperatorNodeFactory> execVisitor(context, execPlan, stream);
      astnode->accept(execVisitor);
      statistics._endPreprocessing = csvsqldb::chrono::ProcessTimeClock::now();

      statistics._startExecution = csvsqldb::chrono::ProcessTimeClock::now();
      int64_t rowCount = execPlan.execute();
      statistics._endExecution = csvsqldb::chrono::ProcessTimeClock::now();

      statistics._maxUsedBlocks = _blockManager.getMaxUsedBlocks();
      statistics._maxUsedCapacity = (_blockManager.getMaxUsedBlocks() * _blockManager.getBlockCapacity()) / (1024 * 1024);
      statistics._totalBlocks = _blockManager.getTotalBlocks();

      return rowCount;
    }

  private:
    ExecutionContext _execContext;
    FunctionRegistry _functions;
    SQLParser _parser;
    BlockManager _blockManager;
  };
}

#endif
