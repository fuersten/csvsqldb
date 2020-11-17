//
//  operatornode.h
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

#ifndef csvsqldb_operation_node_h
#define csvsqldb_operation_node_h

#include "libcsvsqldb/inc.h"

#include "base/csv_parser.h"
#include "base/tribool.h"
#include "base/types.h"
#include "block.h"
#include "block_iterator.h"
#include "file_mapping.h"
#include "stack_machine.h"
#include "visitor.h"
#include <condition_variable>

#include <algorithm>
#include <istream>
#include <mutex>
#include <queue>
#include <thread>


namespace csvsqldb
{
  struct CSVSQLDB_EXPORT OperatorContext {
    OperatorContext(Database& database, const FunctionRegistry& functions, BlockManager& blockManager,
                    const csvsqldb::StringVector& files)
    : _database(database)
    , _functions(functions)
    , _blockManager(blockManager)
    , _files(files)
    , _showHeaderLine(true)
    {
    }

    Database& _database;
    const FunctionRegistry& _functions;
    BlockManager& _blockManager;
    const csvsqldb::StringVector& _files;
    bool _showHeaderLine;
  };


  class OperatorBaseNode;
  typedef std::shared_ptr<OperatorBaseNode> OperatorBaseNodePtr;

  class RowOperatorNode;
  typedef std::shared_ptr<RowOperatorNode> RowOperatorNodePtr;

  class RootOperatorNode;
  typedef std::shared_ptr<RootOperatorNode> RootOperatorNodePtr;


  class CSVSQLDB_EXPORT OperatorBaseNode
  {
  public:
    typedef std::vector<std::pair<size_t, size_t>> VariableMapping;
    typedef std::map<size_t, size_t> OutputInputMapping;

    struct StackMachineType {
      StackMachineType()
      {
      }

      StackMachineType(StackMachine sm, VariableMapping variableMappings)
      : _sm(sm)
      , _variableMappings(variableMappings)
      {
      }

      StackMachine _sm;
      VariableStore _store;
      VariableMapping _variableMappings;
    };

    typedef std::vector<StackMachineType> StackMachines;

    virtual ~OperatorBaseNode()
    {
    }

    virtual bool connect(const RowOperatorNodePtr& input) = 0;

    const OperatorContext& getContext() const
    {
      return _context;
    }

    const SymbolTable& getSymbolTable() const
    {
      return *_symbolTable;
    }

    virtual void getColumnInfos(SymbolInfos& outputSymbols) = 0;

    virtual BlockManager& getBlockManager()
    {
      return _context._blockManager;
    }

    size_t getMapping(const std::string& variable, const StackMachine::VariableMapping& mapping)
    {
      StackMachine::VariableMapping::const_iterator iter = std::find_if(
        mapping.begin(), mapping.end(), [&](const StackMachine::VariableIndex& var) { return var.first == variable; });
      if (iter == mapping.end()) {
        CSVSQLDB_THROW(csvsqldb::Exception, "no mapping found for variable '" << variable << "'");
      } else {
        return iter->second;
      }
    }

    void fillVariableStore(VariableStore& store, const VariableMapping& variableMapping, const Values& row)
    {
      for (const auto& mapping : variableMapping) {
        store.addVariable(mapping.first, valueToVariant(*row[mapping.second]));
      }
    }

    virtual void dump(std::ostream& stream) const = 0;

  protected:
    OperatorBaseNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
    : _context(context)
    , _symbolTable(symbolTable)
    {
    }

    const OperatorContext& _context;
    SymbolTablePtr _symbolTable;
  };

  class CSVSQLDB_EXPORT RowOperatorNode
  : public OperatorBaseNode
  , public RowProvider
  {
  public:
    virtual void setOutputAlias(const std::string& alias)
    {
      _outputAlias = alias;
    }

  protected:
    RowOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
    : OperatorBaseNode(context, symbolTable)
    {
    }

    void remapOutputSymbols(SymbolInfos& outputSymbols)
    {
      if (!_outputAlias.empty()) {
        for (auto& info : outputSymbols) {
          info->_prefix = _outputAlias;
          info->_qualifiedIdentifier = info->_prefix + "." + info->_identifier;
        }
      }
    }

    std::string _outputAlias;
  };

  class CSVSQLDB_EXPORT RootOperatorNode : public OperatorBaseNode
  {
  public:
    virtual int64_t process() = 0;

  protected:
    RootOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
    : OperatorBaseNode(context, symbolTable)
    {
    }
  };


  class CSVSQLDB_EXPORT OutputRowOperatorNode : public RootOperatorNode
  {
  public:
    OutputRowOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, std::ostream& stream);

    virtual int64_t process();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual void dump(std::ostream& stream) const;

  private:
    std::ostream& _stream;
    std::stringstream _outputBuffer;
    bool _firstCall;
    RowOperatorNodePtr _input;
  };


  class CSVSQLDB_EXPORT LimitOperatorNode : public RowOperatorNode
  {
  public:
    LimitOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const ASTExprNodePtr& limit,
                      const ASTExprNodePtr& offset);

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual void dump(std::ostream& stream) const;

  private:
    RowOperatorNodePtr _input;
    SymbolInfos _inputSymbols;
    int64_t _limit;
    int64_t _offset;
  };


  class CSVSQLDB_EXPORT SortOperatorNode : public RowOperatorNode
  {
  public:
    SortOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, OrderExpressions orderExpressions);

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual void dump(std::ostream& stream) const;

  private:
    Types _types;
    Values _row;
    SortingBlockIteratorPtr _iterator;
    RowOperatorNodePtr _input;
    SymbolInfos _inputSymbols;
    OrderExpressions _orderExpressions;
  };


  class CSVSQLDB_EXPORT GroupingOperatorNode : public RowOperatorNode
  {
  public:
    GroupingOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const Expressions& nodes,
                         const Identifiers& groupByIdentifiers);

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual void dump(std::ostream& stream) const;

  private:
    void addPathThrough(const ASTIdentifierPtr& ident, csvsqldb::IndexVector& groupingIndices,
                        csvsqldb::IndexVector& outputColumns, bool suppress);

    Types _types;
    Values _row;
    RowOperatorNodePtr _input;
    SymbolInfos _inputSymbols;
    SymbolInfos _outputSymbols;
    Expressions _nodes;
    Identifiers _groupByIdentifiers;
    GroupingBlockIteratorPtr _iterator;
    AggregationFunctions _aggregateFunctions;
  };


  class CSVSQLDB_EXPORT AggregationOperatorNode
  : public RowOperatorNode
  , public BlockProvider
  {
  public:
    AggregationOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const Expressions& nodes);

    virtual ~AggregationOperatorNode();

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    /// BlockProvider interface
    virtual BlockPtr getNextBlock();

    virtual void dump(std::ostream& stream) const;

  private:
    SymbolInfos _outputSymbols;
    const Expressions& _nodes;
    AggregationFunctions _aggregateFunctions;
    BlockPtr _block;
    StackMachines _sms;
    RowOperatorNodePtr _input;
    BlockIteratorPtr _iterator;
    Types _types;
  };


  class CSVSQLDB_EXPORT ExtendedProjectionOperatorNode
  : public RowOperatorNode
  , public BlockProvider
  {
  public:
    ExtendedProjectionOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const Expressions& nodes);

    virtual ~ExtendedProjectionOperatorNode();

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    /// BlockProvider interface
    virtual BlockPtr getNextBlock();

    virtual void dump(std::ostream& stream) const;

  private:
    BlockPtr prepareNextBuffer();

    SymbolInfos _inputSymbols;
    SymbolInfos _outputSymbols;
    const Expressions& _nodes;
    BlockPtr _block;
    StackMachines _sms;
    OutputInputMapping _outputInputMapping;
    RowOperatorNodePtr _input;
    BlockIteratorPtr _iterator;
    Types _types;
  };


  class CSVSQLDB_EXPORT CrossJoinOperatorNode : public RowOperatorNode
  {
  public:
    CrossJoinOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable);

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual void dump(std::ostream& stream) const;

  protected:
    SymbolInfos _inputLhsSymbols;
    SymbolInfos _inputRhsSymbols;

  private:
    Values _row;
    const Values* _currentLhs;
    CachingBlockIteratorPtr _rhsIterator;
    SymbolInfos _outputSymbols;
    RowOperatorNodePtr _lhsInput;
    RowOperatorNodePtr _rhsInput;
  };


  class CSVSQLDB_EXPORT InnerJoinOperatorNode : public CrossJoinOperatorNode
  {
  public:
    InnerJoinOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const ASTExprNodePtr& exp);

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual void dump(std::ostream& stream) const;

  private:
    StackMachineType _sm;
    ASTExprNodePtr _exp;
  };


  class CSVSQLDB_EXPORT InnerHashJoinOperatorNode : public RowOperatorNode
  {
  public:
    InnerHashJoinOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const ASTExprNodePtr& exp);

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual void dump(std::ostream& stream) const;

  private:
    SymbolInfos _inputLhsSymbols;
    SymbolInfos _inputRhsSymbols;

    Values _row;
    const Values* _currentLhs;
    HashingBlockIteratorPtr _rhsIterator;
    SymbolInfos _outputSymbols;
    RowOperatorNodePtr _lhsInput;
    RowOperatorNodePtr _rhsInput;
    ASTExprNodePtr _exp;
    size_t _hashTableKeyPosition;
  };


  class CSVSQLDB_EXPORT UnionOperatorNode : public RowOperatorNode
  {
  public:
    UnionOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable);

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual void dump(std::ostream& stream) const;

  private:
    SymbolInfos _inputSymbols;
    RowOperatorNodePtr _currentInput;
    RowOperatorNodePtr _firstInput;
    RowOperatorNodePtr _secondInput;
  };


  class CSVSQLDB_EXPORT SelectOperatorNode : public RowOperatorNode
  {
  public:
    SelectOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const ASTExprNodePtr& exp);

    virtual const Values* getNextRow();

    virtual bool connect(const RowOperatorNodePtr& input);

    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual void dump(std::ostream& stream) const;

  private:
    SymbolInfos _inputSymbols;
    VariableStore _store;
    StackMachine _sm;
    StackMachine::VariableMapping _mapping;
    IdentifierSet _expressionVariables;
    VariableMapping _variableMapping;
    RowOperatorNodePtr _input;
  };


  class CSVSQLDB_EXPORT ScanOperatorNode : public RowOperatorNode
  {
  public:
    virtual void getColumnInfos(SymbolInfos& outputSymbols);

    virtual bool connect(const RowOperatorNodePtr& input)
    {
      CSVSQLDB_THROW(csvsqldb::Exception, "connect not allowed");
    }

  protected:
    ScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const SymbolInfo& tableInfo);

    Types _types;
    const TableData& _tableData;
    const SymbolInfo& _tableInfo;
  };


  class CSVSQLDB_EXPORT SystemTableScanOperatorNode
  : public ScanOperatorNode
  , public BlockProvider
  {
  public:
    SystemTableScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const SymbolInfo& tableInfo);

    virtual void dump(std::ostream& stream) const;

    virtual const Values* getNextRow();

    /// BlockProvider interface
    virtual BlockPtr getNextBlock();

  private:
    BlockPtr _currentBlock;
    BlockIteratorPtr _iterator;
  };

  class CSVSQLDB_EXPORT BlockReader : public csvsqldb::csv::CSVParserCallback
  {
  public:
    typedef std::shared_ptr<csvsqldb::csv::CSVParser> CSVParserPtr;

    BlockReader(BlockManager& blockManager);

    ~BlockReader();

    void initialize(CSVParserPtr csvparser);

    bool valid() const
    {
      return _csvparser.get();
    }

    BlockPtr getNextBlock();

    /// CSVParserCallback interface
    virtual void onLong(int64_t num, bool isNull);

    virtual void onDouble(double num, bool isNull);

    virtual void onString(const char* s, size_t len, bool isNull);

    virtual void onDate(const csvsqldb::Date& date, bool isNull);

    virtual void onTime(const csvsqldb::Time& time, bool isNull);

    virtual void onTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull);

    virtual void onBoolean(bool boolean, bool isNull);

  private:
    typedef std::queue<BlockPtr> Blocks;

    void readBlocks();

    CSVParserPtr _csvparser;
    BlockManager& _blockManager;
    Blocks _blocks;
    BlockPtr _block;
    std::thread _readThread;
    std::condition_variable _cv;
    std::mutex _queueMutex;
    bool _continue;
  };


  class CSVSQLDB_EXPORT TableScanOperatorNode
  : public ScanOperatorNode
  , public BlockProvider
  {
  public:
    TableScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const SymbolInfo& tableInfo);

    virtual void dump(std::ostream& stream) const;

    virtual const Values* getNextRow();

    /// BlockProvider interface
    virtual BlockPtr getNextBlock();

  private:
    typedef std::shared_ptr<std::istream> IStreamPtr;
    typedef std::shared_ptr<csvsqldb::csv::CSVParser> CSVParserPtr;

    void initializeBlockReader();

    BlockReader _blockReader;
    BlockIteratorPtr _iterator;

    IStreamPtr _stream;
    CSVParserPtr _csvparser;
    csvsqldb::csv::CSVParserContext _csvContext;
  };
}

#endif
