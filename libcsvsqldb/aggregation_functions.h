//
//  aggregation_functions.h
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

#ifndef csvsqldb_aggregation_functions_h
#define csvsqldb_aggregation_functions_h

#include "libcsvsqldb/inc.h"

#include "block.h"


namespace csvsqldb
{
  class AggregationFunction;
  typedef std::shared_ptr<AggregationFunction> AggregationFunctionPtr;
  typedef std::vector<AggregationFunctionPtr> AggregationFunctions;


  class CSVSQLDB_EXPORT AggregationFunction
  {
  public:
    virtual ~AggregationFunction()
    {
    }

    void init()
    {
      doInit();
    }

    void step(const Variant& value)
    {
      doStep(value);
    }

    const Variant& finalize()
    {
      return doFinalize();
    }

    virtual bool suppress() const
    {
      return false;
    }

    virtual std::string toString() const = 0;

    virtual AggregationFunction* clone(BlockPtr block) const = 0;

    static AggregationFunctionPtr create(eAggregateFunction aggrFunc, eType type);

  protected:
    AggregationFunction()
    {
    }

  private:
    virtual void doInit() = 0;
    virtual void doStep(const Variant& value) = 0;
    virtual const Variant& doFinalize() = 0;
  };


  class CSVSQLDB_EXPORT CountAggregationFunction : public AggregationFunction
  {
  public:
    CountAggregationFunction()
    : _count(INT)
    {
    }

    virtual AggregationFunction* clone(BlockPtr block) const;

    virtual std::string toString() const
    {
      return "COUNT";
    }

  private:
    virtual void doInit();
    virtual void doStep(const Variant& value);
    virtual const Variant& doFinalize();

    Variant _count;
  };


  class CSVSQLDB_EXPORT RowCountAggregationFunction : public AggregationFunction
  {
  public:
    RowCountAggregationFunction()
    : _count(0)
    {
    }

    virtual AggregationFunction* clone(BlockPtr block) const;

    virtual std::string toString() const
    {
      return "COUNT_STAR";
    }

  private:
    virtual void doInit();
    virtual void doStep(const Variant& value);
    virtual const Variant& doFinalize();

    Variant _count;
  };


  class CSVSQLDB_EXPORT PaththroughAggregationFunction : public AggregationFunction
  {
  public:
    PaththroughAggregationFunction(bool suppress)
    : _suppress(suppress)
    {
    }

    virtual AggregationFunction* clone(BlockPtr block) const;

    virtual std::string toString() const
    {
      return "PATH_THROUGH";
    }

    virtual bool suppress() const
    {
      return _suppress;
    }

  private:
    virtual void doInit();
    virtual void doStep(const Variant& value);
    virtual const Variant& doFinalize();

    Variant _value;
    bool _suppress;
  };


  class CSVSQLDB_EXPORT SumAggregationFunction : public AggregationFunction
  {
  public:
    SumAggregationFunction(eType type)
    : _sum(type)
    {
      if (type != INT && type != REAL) {
        CSVSQLDB_THROW(csvsqldb::Exception, "only INT and REAL as summation aggregation allowed");
      }
    }

    virtual AggregationFunction* clone(BlockPtr block) const;

    virtual std::string toString() const
    {
      return "SUM";
    }

  private:
    virtual void doInit();
    virtual void doStep(const Variant& value);
    virtual const Variant& doFinalize();

    Variant _sum;
  };


  class CSVSQLDB_EXPORT AvgAggregationFunction : public AggregationFunction
  {
  public:
    AvgAggregationFunction(eType type)
    : _count(0)
    , _sum(type)
    {
      if (type != INT && type != REAL) {
        CSVSQLDB_THROW(csvsqldb::Exception, "only INT and REAL as summation aggregation allowed");
      }
    }

    virtual AggregationFunction* clone(BlockPtr block) const;

    virtual std::string toString() const
    {
      return "AVG";
    }

  private:
    virtual void doInit();
    virtual void doStep(const Variant& value);
    virtual const Variant& doFinalize();

    Variant _count;
    Variant _sum;
  };


  class CSVSQLDB_EXPORT MinAggregationFunction : public AggregationFunction
  {
  public:
    MinAggregationFunction(eType type)
    : _value(type)
    {
    }

    virtual AggregationFunction* clone(BlockPtr block) const;

    virtual std::string toString() const
    {
      return "MIN";
    }

  private:
    virtual void doInit();
    virtual void doStep(const Variant& value);
    virtual const Variant& doFinalize();

    Variant _value;
  };


  class CSVSQLDB_EXPORT MaxAggregationFunction : public AggregationFunction
  {
  public:
    MaxAggregationFunction(eType type)
    : _value(type)
    {
    }

    virtual AggregationFunction* clone(BlockPtr block) const;

    virtual std::string toString() const
    {
      return "MAX";
    }

  private:
    virtual void doInit();
    virtual void doStep(const Variant& value);
    virtual const Variant& doFinalize();

    Variant _value;
  };


  class CSVSQLDB_EXPORT ArbitraryAggregationFunction : public AggregationFunction
  {
  public:
    ArbitraryAggregationFunction(eType type)
    : _value(type)
    {
    }

    virtual AggregationFunction* clone(BlockPtr block) const;

    virtual std::string toString() const
    {
      return "ARBITRARY";
    }

  private:
    virtual void doInit();
    virtual void doStep(const Variant& value);
    virtual const Variant& doFinalize();

    Variant _value;
  };
}

#endif
