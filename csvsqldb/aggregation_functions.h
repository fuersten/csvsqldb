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

#pragma once

#include <csvsqldb/inc.h>

#include <csvsqldb/block.h>


namespace csvsqldb
{
  class AggregationFunction;
  using AggregationFunctionPtr = std::shared_ptr<AggregationFunction>;
  using AggregationFunctions = std::vector<AggregationFunctionPtr>;


  class CSVSQLDB_EXPORT AggregationFunction
  {
  public:
    AggregationFunction() = default;

    virtual ~AggregationFunction() = default;

    AggregationFunction(const AggregationFunction&) = delete;
    AggregationFunction& operator=(const AggregationFunction&) = delete;
    AggregationFunction(AggregationFunction&&) = delete;
    AggregationFunction& operator=(AggregationFunction&&) = delete;

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

  private:
    virtual void doInit() = 0;
    virtual void doStep(const Variant& value) = 0;
    virtual const Variant& doFinalize() = 0;
  };


  class CSVSQLDB_EXPORT CountAggregationFunction : public AggregationFunction
  {
  public:
    CountAggregationFunction() = default;

    AggregationFunction* clone(BlockPtr block) const override;

    std::string toString() const override
    {
      return "COUNT";
    }

  private:
    void doInit() override;
    void doStep(const Variant& value) override;
    const Variant& doFinalize() override;

    Variant _count{INT};
  };


  class CSVSQLDB_EXPORT RowCountAggregationFunction : public AggregationFunction
  {
  public:
    RowCountAggregationFunction() = default;

    AggregationFunction* clone(BlockPtr block) const override;

    std::string toString() const override
    {
      return "COUNT_STAR";
    }

  private:
    void doInit() override;
    void doStep(const Variant& value) override;
    const Variant& doFinalize() override;

    Variant _count{0};
  };


  class CSVSQLDB_EXPORT PaththroughAggregationFunction : public AggregationFunction
  {
  public:
    PaththroughAggregationFunction(bool suppress)
    : _suppress(suppress)
    {
    }

    AggregationFunction* clone(BlockPtr block) const override;

    std::string toString() const override
    {
      return "PATH_THROUGH";
    }

    bool suppress() const override
    {
      return _suppress;
    }

  private:
    void doInit() override;
    void doStep(const Variant& value) override;
    const Variant& doFinalize() override;

    Variant _value;
    bool _suppress{false};
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

    AggregationFunction* clone(BlockPtr block) const override;

    std::string toString() const override
    {
      return "SUM";
    }

  private:
    void doInit() override;
    void doStep(const Variant& value) override;
    const Variant& doFinalize() override;

    Variant _sum;
  };


  class CSVSQLDB_EXPORT AvgAggregationFunction : public AggregationFunction
  {
  public:
    AvgAggregationFunction(eType type)
    : _sum(type)
    {
      if (type != INT && type != REAL) {
        CSVSQLDB_THROW(csvsqldb::Exception, "only INT and REAL as summation aggregation allowed");
      }
    }

    AggregationFunction* clone(BlockPtr block) const override;

    std::string toString() const override
    {
      return "AVG";
    }

  private:
    void doInit() override;
    void doStep(const Variant& value) override;
    const Variant& doFinalize() override;

    Variant _count{0};
    Variant _sum;
  };


  class CSVSQLDB_EXPORT MinAggregationFunction : public AggregationFunction
  {
  public:
    MinAggregationFunction(eType type)
    : _value(type)
    {
    }

    AggregationFunction* clone(BlockPtr block) const override;

    std::string toString() const override
    {
      return "MIN";
    }

  private:
    void doInit() override;
    void doStep(const Variant& value) override;
    const Variant& doFinalize() override;

    Variant _value;
  };


  class CSVSQLDB_EXPORT MaxAggregationFunction : public AggregationFunction
  {
  public:
    MaxAggregationFunction(eType type)
    : _value(type)
    {
    }

    AggregationFunction* clone(BlockPtr block) const override;

    std::string toString() const override
    {
      return "MAX";
    }

  private:
    void doInit() override;
    void doStep(const Variant& value) override;
    const Variant& doFinalize() override;

    Variant _value;
  };


  class CSVSQLDB_EXPORT ArbitraryAggregationFunction : public AggregationFunction
  {
  public:
    ArbitraryAggregationFunction(eType type)
    : _value(type)
    {
    }

    AggregationFunction* clone(BlockPtr block) const override;

    std::string toString() const override
    {
      return "ARBITRARY";
    }

  private:
    void doInit() override;
    void doStep(const Variant& value) override;
    const Variant& doFinalize() override;

    Variant _value;
  };
}
