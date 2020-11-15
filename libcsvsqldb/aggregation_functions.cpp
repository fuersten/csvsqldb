//
//  aggregation_functions.cpp
//  csvsqldb
//
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
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

#include "aggregation_functions.h"


namespace csvsqldb
{
  AggregationFunctionPtr AggregationFunction::create(eAggregateFunction aggrFunc, eType type)
  {
    switch (aggrFunc) {
      case COUNT:
        return std::make_shared<CountAggregationFunction>();
      case COUNT_STAR:
        return std::make_shared<RowCountAggregationFunction>();
      case MIN:
        return std::make_shared<MinAggregationFunction>(type);
      case MAX:
        return std::make_shared<MaxAggregationFunction>(type);
      case SUM:
        return std::make_shared<SumAggregationFunction>(type);
      case AVG:
        return std::make_shared<AvgAggregationFunction>(type);
      case ARBITRARY:
        return std::make_shared<ArbitraryAggregationFunction>(type);
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  AggregationFunction* CountAggregationFunction::clone(BlockPtr block) const
  {
    if (!block->hasSizeFor(sizeof(CountAggregationFunction))) {
      return nullptr;
    }
    AggregationFunction* tmp = new (block->getRawBuffer()) CountAggregationFunction();
    block->moveOffset(sizeof(CountAggregationFunction));
    return tmp;
  }

  void CountAggregationFunction::doInit()
  {
  }

  void CountAggregationFunction::doStep(const Variant& value)
  {
    if (!value.isNull()) {
      if (_count.isNull()) {
        _count = 1;
      } else {
        _count += 1;
      }
    }
  }

  const Variant& CountAggregationFunction::doFinalize()
  {
    return _count;
  }


  AggregationFunction* RowCountAggregationFunction::clone(BlockPtr block) const
  {
    if (!block->hasSizeFor(sizeof(RowCountAggregationFunction))) {
      return nullptr;
    }
    AggregationFunction* tmp = new (block->getRawBuffer()) RowCountAggregationFunction();
    block->moveOffset(sizeof(RowCountAggregationFunction));
    return tmp;
  }

  void RowCountAggregationFunction::doInit()
  {
  }

  void RowCountAggregationFunction::doStep(const Variant& value)
  {
    _count += 1;
  }

  const Variant& RowCountAggregationFunction::doFinalize()
  {
    return _count;
  }


  AggregationFunction* PaththroughAggregationFunction::clone(BlockPtr block) const
  {
    if (!block->hasSizeFor(sizeof(PaththroughAggregationFunction))) {
      return nullptr;
    }
    AggregationFunction* tmp = new (block->getRawBuffer()) PaththroughAggregationFunction(_suppress);
    block->moveOffset(sizeof(PaththroughAggregationFunction));
    return tmp;
  }

  void PaththroughAggregationFunction::doInit()
  {
  }

  void PaththroughAggregationFunction::doStep(const Variant& value)
  {
    if (_value.getType() == NONE) {
      _value = value;
      _value.disconnect();
    }
  }

  const Variant& PaththroughAggregationFunction::doFinalize()
  {
    return _value;
  }


  AggregationFunction* SumAggregationFunction::clone(BlockPtr block) const
  {
    if (!block->hasSizeFor(sizeof(SumAggregationFunction))) {
      return nullptr;
    }
    AggregationFunction* tmp = new (block->getRawBuffer()) SumAggregationFunction(_sum.getType());
    block->moveOffset(sizeof(SumAggregationFunction));
    return tmp;
  }

  void SumAggregationFunction::doInit()
  {
  }

  void SumAggregationFunction::doStep(const Variant& value)
  {
    if (!value.isNull()) {
      if (_sum.isNull()) {
        _sum = value;
      } else {
        _sum += value;
      }
    }
  }

  const Variant& SumAggregationFunction::doFinalize()
  {
    return _sum;
  }


  AggregationFunction* AvgAggregationFunction::clone(BlockPtr block) const
  {
    if (!block->hasSizeFor(sizeof(AvgAggregationFunction))) {
      return nullptr;
    }
    AggregationFunction* tmp = new (block->getRawBuffer()) AvgAggregationFunction(_sum.getType());
    block->moveOffset(sizeof(AvgAggregationFunction));
    return tmp;
  }

  void AvgAggregationFunction::doInit()
  {
  }

  void AvgAggregationFunction::doStep(const Variant& value)
  {
    if (!value.isNull()) {
      if (_sum.isNull()) {
        _sum = value;
      } else {
        _sum += value;
      }
      _count += 1;
    }
  }

  const Variant& AvgAggregationFunction::doFinalize()
  {
    if (!_sum.isNull()) {
      _sum /= _count;
    }
    return _sum;
  }


  AggregationFunction* MinAggregationFunction::clone(BlockPtr block) const
  {
    if (!block->hasSizeFor(sizeof(MinAggregationFunction))) {
      return nullptr;
    }
    AggregationFunction* tmp = new (block->getRawBuffer()) MinAggregationFunction(_value.getType());
    block->moveOffset(sizeof(MinAggregationFunction));
    return tmp;
  }

  void MinAggregationFunction::doInit()
  {
  }

  void MinAggregationFunction::doStep(const Variant& value)
  {
    if (!value.isNull()) {
      if (_value.isNull()) {
        _value = value;
      } else {
        if (value < _value) {
          _value = value;
          _value.disconnect();
        }
      }
    }
  }

  const Variant& MinAggregationFunction::doFinalize()
  {
    return _value;
  }


  AggregationFunction* MaxAggregationFunction::clone(BlockPtr block) const
  {
    if (!block->hasSizeFor(sizeof(MaxAggregationFunction))) {
      return nullptr;
    }
    AggregationFunction* tmp = new (block->getRawBuffer()) MaxAggregationFunction(_value.getType());
    block->moveOffset(sizeof(MaxAggregationFunction));
    return tmp;
  }

  void MaxAggregationFunction::doInit()
  {
  }

  void MaxAggregationFunction::doStep(const Variant& value)
  {
    if (!value.isNull()) {
      if (_value.isNull()) {
        _value = value;
      } else {
        if (_value < value) {
          _value = value;
          _value.disconnect();
        }
      }
    }
  }

  const Variant& MaxAggregationFunction::doFinalize()
  {
    return _value;
  }


  AggregationFunction* ArbitraryAggregationFunction::clone(BlockPtr block) const
  {
    if (!block->hasSizeFor(sizeof(ArbitraryAggregationFunction))) {
      return nullptr;
    }
    AggregationFunction* tmp = new (block->getRawBuffer()) ArbitraryAggregationFunction(_value.getType());
    block->moveOffset(sizeof(ArbitraryAggregationFunction));
    return tmp;
  }

  void ArbitraryAggregationFunction::doInit()
  {
  }

  void ArbitraryAggregationFunction::doStep(const Variant& value)
  {
    if (!value.isNull()) {
      if (_value.isNull()) {
        _value = value;
        _value.disconnect();
      }
    }
  }

  const Variant& ArbitraryAggregationFunction::doFinalize()
  {
    return _value;
  }
}
