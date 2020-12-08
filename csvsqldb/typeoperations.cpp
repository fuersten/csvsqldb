//
//  typeoperations.cpp
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

#include "typeoperations.h"

#include "base/date.h"
#include "base/float_helper.h"
#include "base/time_helper.h"
#include "types.h"

#include <cmath>
#include <map>
#include <stdexcept>


namespace csvsqldb
{
  template<typename RET, typename L, typename R>
  RET op_add(const L& lhs, const R& rhs)
  {
    return lhs + rhs;
  }

  template<typename RET, typename L, typename R>
  RET op_sub(const L& lhs, const R& rhs)
  {
    return lhs - rhs;
  }

  template<typename RET, typename L, typename R>
  RET op_mul(const L& lhs, const R& rhs)
  {
    return lhs * rhs;
  }

  template<typename RET, typename L, typename R>
  RET op_div(const L& lhs, const R& rhs)
  {
    return lhs / rhs;
  }

  template<typename L, typename R>
  L op_mod(const L& lhs, const R& rhs)
  {
    return lhs % rhs;
  }

  template<>
  double op_mod(const double& lhs, const double& rhs)
  {
    return std::fmod(lhs, rhs);
  }


  template<typename L, typename R>
  struct GreaterThen {
    bool operation(const L& lhs, const R& rhs) const
    {
      return lhs > rhs;
    }
  };

  template<>
  struct GreaterThen<StringType, StringType> {
    bool operation(const StringType& lhs, const StringType& rhs) const
    {
      return ::strcoll(lhs, rhs) > 0;
    }
  };

  template<>
  struct GreaterThen<csvsqldb::Date, StringType> {
    bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
    {
      return lhs > csvsqldb::dateFromString(rhs);
    }
  };

  template<>
  struct GreaterThen<StringType, csvsqldb::Date> {
    bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
    {
      return csvsqldb::dateFromString(lhs) > rhs;
    }
  };

  template<>
  struct GreaterThen<csvsqldb::Time, StringType> {
    bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
    {
      return lhs > csvsqldb::timeFromString(rhs);
    }
  };

  template<>
  struct GreaterThen<StringType, csvsqldb::Time> {
    bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
    {
      return csvsqldb::timeFromString(lhs) > rhs;
    }
  };

  template<>
  struct GreaterThen<csvsqldb::Timestamp, StringType> {
    bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
    {
      return lhs > csvsqldb::timestampFromString(rhs);
    }
  };

  template<>
  struct GreaterThen<StringType, csvsqldb::Timestamp> {
    bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
    {
      return csvsqldb::timestampFromString(lhs) > rhs;
    }
  };


  template<typename L, typename R>
  struct GreaterEqual {
    bool operation(const L& lhs, const R& rhs) const
    {
      return lhs >= rhs;
    }
  };

  template<>
  struct GreaterEqual<StringType, StringType> {
    bool operation(const StringType& lhs, const StringType& rhs) const
    {
      return ::strcoll(lhs, rhs) >= 0;
    }
  };

  template<>
  struct GreaterEqual<csvsqldb::Date, StringType> {
    bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
    {
      return lhs >= csvsqldb::dateFromString(rhs);
    }
  };

  template<>
  struct GreaterEqual<StringType, csvsqldb::Date> {
    bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
    {
      return csvsqldb::dateFromString(lhs) >= rhs;
    }
  };

  template<>
  struct GreaterEqual<csvsqldb::Time, StringType> {
    bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
    {
      return lhs >= csvsqldb::timeFromString(rhs);
    }
  };

  template<>
  struct GreaterEqual<StringType, csvsqldb::Time> {
    bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
    {
      return csvsqldb::timeFromString(lhs) >= rhs;
    }
  };

  template<>
  struct GreaterEqual<csvsqldb::Timestamp, StringType> {
    bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
    {
      return lhs >= csvsqldb::timestampFromString(rhs);
    }
  };

  template<>
  struct GreaterEqual<StringType, csvsqldb::Timestamp> {
    bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
    {
      return csvsqldb::timestampFromString(lhs) >= rhs;
    }
  };


  template<typename L, typename R>
  struct LessThen {
    bool operation(const L& lhs, const R& rhs) const
    {
      return lhs < rhs;
    }
  };

  template<>
  struct LessThen<StringType, StringType> {
    bool operation(const StringType& lhs, const StringType& rhs) const
    {
      return ::strcoll(lhs, rhs) < 0;
    }
  };

  template<>
  struct LessThen<csvsqldb::Date, StringType> {
    bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
    {
      return lhs < csvsqldb::dateFromString(rhs);
    }
  };

  template<>
  struct LessThen<StringType, csvsqldb::Date> {
    bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
    {
      return csvsqldb::dateFromString(lhs) < rhs;
    }
  };

  template<>
  struct LessThen<csvsqldb::Time, StringType> {
    bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
    {
      return lhs < csvsqldb::timeFromString(rhs);
    }
  };

  template<>
  struct LessThen<StringType, csvsqldb::Time> {
    bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
    {
      return csvsqldb::timeFromString(lhs) < rhs;
    }
  };

  template<>
  struct LessThen<csvsqldb::Timestamp, StringType> {
    bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
    {
      return lhs < csvsqldb::timestampFromString(rhs);
    }
  };

  template<>
  struct LessThen<StringType, csvsqldb::Timestamp> {
    bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
    {
      return csvsqldb::timestampFromString(lhs) < rhs;
    }
  };


  template<typename L, typename R>
  struct LessEqual {
    bool operation(const L& lhs, const R& rhs) const
    {
      return lhs <= rhs;
    }
  };

  template<>
  struct LessEqual<StringType, StringType> {
    bool operation(const StringType& lhs, const StringType& rhs) const
    {
      return ::strcoll(lhs, rhs) <= 0;
    }
  };

  template<>
  struct LessEqual<csvsqldb::Date, StringType> {
    bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
    {
      return lhs <= csvsqldb::dateFromString(rhs);
    }
  };

  template<>
  struct LessEqual<StringType, csvsqldb::Date> {
    bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
    {
      return csvsqldb::dateFromString(lhs) <= rhs;
    }
  };

  template<>
  struct LessEqual<csvsqldb::Time, StringType> {
    bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
    {
      return lhs <= csvsqldb::timeFromString(rhs);
    }
  };

  template<>
  struct LessEqual<StringType, csvsqldb::Time> {
    bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
    {
      return csvsqldb::timeFromString(lhs) <= rhs;
    }
  };

  template<>
  struct LessEqual<csvsqldb::Timestamp, StringType> {
    bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
    {
      return lhs <= csvsqldb::timestampFromString(rhs);
    }
  };

  template<>
  struct LessEqual<StringType, csvsqldb::Timestamp> {
    bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
    {
      return csvsqldb::timestampFromString(lhs) <= rhs;
    }
  };


  template<typename L, typename R>
  struct CompareEqual {
    bool operation(const L& lhs, const R& rhs) const
    {
      return lhs == rhs;
    }
  };

  template<>
  struct CompareEqual<double, double> {
    bool operation(const double& lhs, const double& rhs) const
    {
      return csvsqldb::Approx(lhs) == rhs;
    }
  };

  template<>
  struct CompareEqual<StringType, StringType> {
    bool operation(const StringType& lhs, const StringType& rhs) const
    {
      return ::strcoll(lhs, rhs) == 0;
    }
  };

  template<>
  struct CompareEqual<csvsqldb::Date, StringType> {
    bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
    {
      return lhs == csvsqldb::dateFromString(rhs);
    }
  };

  template<>
  struct CompareEqual<StringType, csvsqldb::Date> {
    bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
    {
      return csvsqldb::dateFromString(lhs) == rhs;
    }
  };

  template<>
  struct CompareEqual<csvsqldb::Time, StringType> {
    bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
    {
      return lhs == csvsqldb::timeFromString(rhs);
    }
  };

  template<>
  struct CompareEqual<StringType, csvsqldb::Time> {
    bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
    {
      return csvsqldb::timeFromString(lhs) == rhs;
    }
  };

  template<>
  struct CompareEqual<csvsqldb::Timestamp, StringType> {
    bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
    {
      return lhs == csvsqldb::timestampFromString(rhs);
    }
  };

  template<>
  struct CompareEqual<StringType, csvsqldb::Timestamp> {
    bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
    {
      return csvsqldb::timestampFromString(lhs) == rhs;
    }
  };


  template<typename L, typename R>
  struct CompareNotEqual {
    bool operation(const L& lhs, const R& rhs) const
    {
      return lhs != rhs;
    }
  };

  template<>
  struct CompareNotEqual<double, double> {
    bool operation(const double& lhs, const double& rhs) const
    {
      return csvsqldb::Approx(lhs) != rhs;
    }
  };

  template<>
  struct CompareNotEqual<StringType, StringType> {
    bool operation(const StringType& lhs, const StringType& rhs) const
    {
      return ::strcoll(lhs, rhs) != 0;
    }
  };

  template<>
  struct CompareNotEqual<csvsqldb::Date, StringType> {
    bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
    {
      return lhs != csvsqldb::dateFromString(rhs);
    }
  };

  template<>
  struct CompareNotEqual<StringType, csvsqldb::Date> {
    bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
    {
      return csvsqldb::dateFromString(lhs) != rhs;
    }
  };

  template<>
  struct CompareNotEqual<csvsqldb::Time, StringType> {
    bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
    {
      return lhs != csvsqldb::timeFromString(rhs);
    }
  };

  template<>
  struct CompareNotEqual<StringType, csvsqldb::Time> {
    bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
    {
      return csvsqldb::timeFromString(lhs) != rhs;
    }
  };

  template<>
  struct CompareNotEqual<csvsqldb::Timestamp, StringType> {
    bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
    {
      return lhs != csvsqldb::timestampFromString(rhs);
    }
  };

  template<>
  struct CompareNotEqual<StringType, csvsqldb::Timestamp> {
    bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
    {
      return csvsqldb::timestampFromString(lhs) != rhs;
    }
  };


  template<typename L, typename R>
  struct And {
    bool operation(const L& lhs, const R& rhs) const
    {
      return static_cast<bool>(lhs) && static_cast<bool>(rhs);
    }
  };

  template<typename L>
  struct And<L, StringType> {
    bool operation(const L& lhs, const StringType& rhs) const
    {
      return static_cast<bool>(lhs) && rhs;
    }
  };

  template<typename R>
  struct And<StringType, R> {
    bool operation(const StringType& lhs, const R& rhs) const
    {
      return lhs && static_cast<bool>(rhs);
    }
  };

  template<>
  struct And<StringType, StringType> {
    bool operation(const StringType& lhs, const StringType& rhs) const
    {
      return lhs && rhs;
    }
  };

  template<typename L, typename R>
  struct Or {
    bool operation(const L& lhs, const R& rhs) const
    {
      return static_cast<bool>(lhs) || static_cast<bool>(rhs);
    }
  };

  template<typename L>
  struct Or<L, StringType> {
    bool operation(const L& lhs, const StringType& rhs) const
    {
      return static_cast<bool>(lhs) || rhs;
    }
  };

  template<typename R>
  struct Or<StringType, R> {
    bool operation(const StringType& lhs, const R& rhs) const
    {
      return lhs || static_cast<bool>(rhs);
    }
  };

  template<>
  struct Or<StringType, StringType> {
    bool operation(const StringType& lhs, const StringType& rhs) const
    {
      return lhs || rhs;
    }
  };

  static bool op_not(const bool& rhs)
  {
    return !rhs;
  }

  template<typename R>
  R op_minus(const R& rhs)
  {
    return -rhs;
  }

  template<typename R>
  R op_plus(const R& rhs)
  {
    return +rhs;
  }

  template<typename RET, typename R>
  struct Cast {
    RET operation(const R& rhs) const
    {
      return static_cast<RET>(rhs);
    }
  };

  template<>
  struct Cast<bool, StringType> {
    bool operation(const StringType& rhs) const
    {
      return rhs;
    }
  };

  template<>
  struct Cast<double, StringType> {
    double operation(const StringType& rhs) const
    {
      return ::atof(rhs);
    }
  };

  template<>
  struct Cast<int64_t, StringType> {
    int64_t operation(const StringType& rhs) const
    {
      return ::atol(rhs);
    }
  };

  template<>
  struct Cast<csvsqldb::Date, StringType> {
    csvsqldb::Date operation(const StringType& rhs) const
    {
      return csvsqldb::dateFromString(rhs);
    }
  };

  template<>
  struct Cast<csvsqldb::Time, StringType> {
    csvsqldb::Time operation(const StringType& rhs) const
    {
      return csvsqldb::timeFromString(rhs);
    }
  };

  template<>
  struct Cast<csvsqldb::Timestamp, StringType> {
    csvsqldb::Timestamp operation(const StringType& rhs) const
    {
      return csvsqldb::timestampFromString(rhs);
    }
  };

  template<>
  struct Cast<csvsqldb::Timestamp, csvsqldb::Date> {
    csvsqldb::Timestamp operation(const csvsqldb::Date& rhs) const
    {
      return csvsqldb::Timestamp(rhs);
    }
  };

  template<>
  struct Cast<csvsqldb::Timestamp, csvsqldb::Time> {
    csvsqldb::Timestamp operation(const csvsqldb::Time& rhs) const
    {
      return csvsqldb::Timestamp(rhs);
    }
  };


  struct OperationKey {
    OperationKey(eOperationType op, eType lhs, eType rhs)
    : _op(op)
    , _lhs(lhs)
    , _rhs(rhs)
    {
    }

    bool operator<(const OperationKey& key) const
    {
      if (_op < key._op) {
        return true;
      } else if (_op == key._op && _lhs < key._lhs) {
        return true;
      } else if (_op == key._op && _lhs == key._lhs && _rhs < key._rhs) {
        return true;
      }
      return false;
    }

    eOperationType _op;
    eType _lhs;
    eType _rhs;
  };

  struct BinaryOperation;
  using BinaryOperationPtr = std::unique_ptr<const BinaryOperation>;

  struct BinaryOperation {
    virtual eOperationType opType() const = 0;
    virtual eType lhsType() const = 0;
    virtual eType rhsType() const = 0;
    virtual eType retType() const = 0;

    Variant execute(const Variant& lhs, const Variant& rhs) const
    {
      if (lhs.isNull() || rhs.isNull()) {
        return doHandleNull(lhs, rhs);
      }
      return doExecute(lhs, rhs);
    }

    OperationKey key() const
    {
      return OperationKey(opType(), lhsType(), rhsType());
    }

    virtual ~BinaryOperation()
    {
    }

  private:
    virtual Variant doExecute(const Variant& lhs, const Variant& rhs) const = 0;
    virtual Variant doHandleNull(const Variant& lhs, const Variant& rhs) const = 0;
  };

  template<eOperationType OP_TYPE, typename RET, typename LHS, typename RHS>
  struct BinaryOperationBase : public BinaryOperation {
    eOperationType opType() const override
    {
      return OP_TYPE;
    }

    eType lhsType() const override
    {
      return ctype2eType<LHS>();
    }

    eType rhsType() const override
    {
      return ctype2eType<RHS>();
    }

    eType retType() const override
    {
      return ctype2eType<RET>();
    }

  private:
    Variant doExecute(const Variant& lhs, const Variant& rhs) const override
    {
      return Variant(operation(ValueGetter<LHS>::getValue(lhs), ValueGetter<RHS>::getValue(rhs)));
    }

    Variant doHandleNull(const Variant& lhs, const Variant& rhs) const override
    {
      return Variant(retType());
    }

    virtual RET operation(const LHS& lhs, const RHS& rhs) const = 0;
  };

  template<typename RET, typename CAST, typename LHS, typename RHS>
  struct OperationAdd : public BinaryOperationBase<OP_ADD, RET, LHS, RHS> {
  private:
    RET operation(const LHS& lhs, const RHS& rhs) const override
    {
      return op_add<RET, CAST, CAST>(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }
  };

  template<typename RET, typename CAST, typename LHS, typename RHS>
  struct OperationSub : public BinaryOperationBase<OP_SUB, RET, LHS, RHS> {
  private:
    RET operation(const LHS& lhs, const RHS& rhs) const override
    {
      return op_sub<RET, CAST, CAST>(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }
  };

  template<typename RET, typename CAST, typename LHS, typename RHS>
  struct OperationMul : public BinaryOperationBase<OP_MUL, RET, LHS, RHS> {
  private:
    RET operation(const LHS& lhs, const RHS& rhs) const override
    {
      return op_mul<RET, CAST, CAST>(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }
  };

  template<typename RET, typename CAST, typename LHS, typename RHS>
  struct OperationDiv : public BinaryOperationBase<OP_DIV, RET, LHS, RHS> {
  private:
    RET operation(const LHS& lhs, const RHS& rhs) const override
    {
      return op_div<RET, CAST, CAST>(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }
  };

  template<typename RET, typename CAST, typename LHS, typename RHS>
  struct OperationMod : public BinaryOperationBase<OP_MOD, RET, LHS, RHS> {
  private:
    RET operation(const LHS& lhs, const RHS& rhs) const override
    {
      return op_mod(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }
  };

  template<typename CAST, typename LHS, typename RHS>
  struct OperationGTCast : public BinaryOperationBase<OP_GT, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _greater.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }

    GreaterThen<CAST, CAST> _greater;
  };

  template<typename RET, typename LHS, typename RHS>
  struct OperationGT : public BinaryOperationBase<OP_GT, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _greater.operation(lhs, rhs);
    }

    GreaterThen<LHS, RHS> _greater;
  };

  template<typename CAST, typename LHS, typename RHS>
  struct OperationGECast : public BinaryOperationBase<OP_GE, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _greater.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }

    GreaterEqual<CAST, CAST> _greater;
  };

  template<typename RET, typename LHS, typename RHS>
  struct OperationGE : public BinaryOperationBase<OP_GE, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _greater.operation(lhs, rhs);
    }

    GreaterEqual<LHS, RHS> _greater;
  };

  template<typename CAST, typename LHS, typename RHS>
  struct OperationLTCast : public BinaryOperationBase<OP_LT, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _lesser.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }

    LessThen<CAST, CAST> _lesser;
  };

  template<typename RET, typename LHS, typename RHS>
  struct OperationLT : public BinaryOperationBase<OP_LT, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _lesser.operation(lhs, rhs);
    }

    LessThen<LHS, RHS> _lesser;
  };

  template<typename CAST, typename LHS, typename RHS>
  struct OperationLECast : public BinaryOperationBase<OP_LE, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _lesser.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }

    LessEqual<CAST, CAST> _lesser;
  };

  template<typename RET, typename LHS, typename RHS>
  struct OperationLE : public BinaryOperationBase<OP_LE, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _lesser.operation(lhs, rhs);
    }

    LessEqual<LHS, RHS> _lesser;
  };

  template<typename CAST, typename LHS, typename RHS>
  struct OperationEQCast : public BinaryOperationBase<OP_EQ, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _compare.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }

    CompareEqual<CAST, CAST> _compare;
  };

  template<typename CAST, typename LHS, typename RHS>
  struct OperationEQ : public BinaryOperationBase<OP_EQ, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _compare.operation(lhs, rhs);
    }

    CompareEqual<LHS, RHS> _compare;
  };

  template<typename CAST, typename LHS, typename RHS>
  struct OperationNEQCast : public BinaryOperationBase<OP_NEQ, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _compare.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
    }

    CompareNotEqual<CAST, CAST> _compare;
  };

  template<typename CAST, typename LHS, typename RHS>
  struct OperationNEQ : public BinaryOperationBase<OP_NEQ, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _compare.operation(lhs, rhs);
    }

    CompareNotEqual<LHS, RHS> _compare;
  };

  template<typename LHS, typename RHS>
  struct OperationIs : public BinaryOperationBase<OP_IS, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return lhs && rhs;
    }

    Variant doHandleNull(const Variant& lhs, const Variant& rhs) const override
    {
      if (rhs.isNull()) {
        return Variant(lhs.isNull());
      } else if (lhs.isNull()) {
        return Variant(false);
      }
      return Variant(lhs.asBool() && rhs.asBool());
    }
  };

  template<>
  struct OperationIs<double, bool> : public BinaryOperationBase<OP_IS, bool, double, bool> {
  private:
    bool operation(const double& lhs, const bool& rhs) const override
    {
      return (csvsqldb::Approx(lhs) != 0.0) && rhs;
    }

    Variant doHandleNull(const Variant& lhs, const Variant& rhs) const override
    {
      if (rhs.isNull()) {
        return Variant(lhs.isNull());
      } else if (lhs.isNull()) {
        return Variant(false);
      }
      return Variant(lhs.asBool() && rhs.asBool());
    }
  };

  template<typename LHS, typename RHS>
  struct OperationIsNot : public BinaryOperationBase<OP_ISNOT, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return !(lhs && rhs);
    }

    Variant doHandleNull(const Variant& lhs, const Variant& rhs) const override
    {
      if (rhs.isNull()) {
        return Variant(!lhs.isNull());
      } else if (lhs.isNull()) {
        return Variant(true);
      }
      return Variant(!(lhs.asBool() && rhs.asBool()));
    }
  };

  template<>
  struct OperationIsNot<double, bool> : public BinaryOperationBase<OP_ISNOT, bool, double, bool> {
  private:
    bool operation(const double& lhs, const bool& rhs) const override
    {
      return !((csvsqldb::Approx(lhs) != 0.0) && rhs);
    }

    Variant doHandleNull(const Variant& lhs, const Variant& rhs) const override
    {
      if (rhs.isNull()) {
        return Variant(!lhs.isNull());
      } else if (lhs.isNull()) {
        return Variant(true);
      }
      return Variant(!(lhs.asBool() && rhs.asBool()));
    }
  };

  template<typename LHS, typename RHS>
  struct OperationAnd : public BinaryOperationBase<OP_AND, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _and.operation(lhs, rhs);
    }

    Variant doHandleNull(const Variant& lhs, const Variant& rhs) const override
    {
      if (lhs.isNull() && !rhs.isNull() && !rhs.asBool()) {
        return Variant(false);
      }
      if (rhs.isNull() && !lhs.isNull() && !lhs.asBool()) {
        return Variant(false);
      }
      return Variant(BOOLEAN);
    }

    And<LHS, RHS> _and;
  };

  template<typename LHS, typename RHS>
  struct OperationOr : public BinaryOperationBase<OP_OR, bool, LHS, RHS> {
  private:
    bool operation(const LHS& lhs, const RHS& rhs) const override
    {
      return _or.operation(lhs, rhs);
    }

    Variant doHandleNull(const Variant& lhs, const Variant& rhs) const override
    {
      if (!lhs.isNull() && rhs.isNull() && lhs.asBool()) {
        return Variant(true);
      }
      if (!rhs.isNull() && lhs.isNull() && rhs.asBool()) {
        return Variant(true);
      }
      return Variant(BOOLEAN);
    }

    Or<LHS, RHS> _or;
  };

  template<typename RET, typename L, typename R>
  RET op_concat(const L& lhs, const R& rhs);

  template<>
  StringType op_concat<StringType>(const StringType& lhs, const StringType& rhs)
  {
    std::size_t llen = ::strlen(lhs);
    std::size_t rlen = ::strlen(rhs);
    char* c = new char[llen + rlen + 1];
    ::strncpy(&c[0], lhs, llen);
    ::strncpy(&c[llen], rhs, rlen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const StringType& lhs, const int64_t& rhs)
  {
    std::size_t llen = ::strlen(lhs);
    std::string rval(std::to_string(rhs));
    std::size_t rlen = rval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], lhs, llen);
    ::strncpy(&c[llen], rval.c_str(), rlen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const int64_t& lhs, const StringType& rhs)
  {
    std::size_t rlen = ::strlen(rhs);
    std::string lval(std::to_string(lhs));
    std::size_t llen = lval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], rhs, rlen);
    ::strncpy(&c[rlen], lval.c_str(), llen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const StringType& lhs, const double& rhs)
  {
    std::size_t llen = ::strlen(lhs);
    std::string rval(std::to_string(rhs));
    std::size_t rlen = rval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], lhs, llen);
    ::strncpy(&c[llen], rval.c_str(), rlen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const double& lhs, const StringType& rhs)
  {
    std::size_t rlen = ::strlen(rhs);
    std::string lval(std::to_string(lhs));
    std::size_t llen = lval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], rhs, rlen);
    ::strncpy(&c[rlen], lval.c_str(), llen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const StringType& lhs, const csvsqldb::Date& rhs)
  {
    std::size_t llen = ::strlen(lhs);
    std::string rval(rhs.format("%F"));
    std::size_t rlen = rval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], lhs, llen);
    ::strncpy(&c[llen], rval.c_str(), rlen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const csvsqldb::Date& lhs, const StringType& rhs)
  {
    std::size_t rlen = ::strlen(rhs);
    std::string lval(lhs.format("%F"));
    std::size_t llen = lval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], rhs, rlen);
    ::strncpy(&c[rlen], lval.c_str(), llen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const StringType& lhs, const csvsqldb::Time& rhs)
  {
    std::size_t llen = ::strlen(lhs);
    std::string rval(rhs.format("%H:%M:%S"));
    std::size_t rlen = rval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], lhs, llen);
    ::strncpy(&c[llen], rval.c_str(), rlen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const csvsqldb::Time& lhs, const StringType& rhs)
  {
    std::size_t rlen = ::strlen(rhs);
    std::string lval(lhs.format("%H:%M:%S"));
    std::size_t llen = lval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], rhs, rlen);
    ::strncpy(&c[rlen], lval.c_str(), llen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const StringType& lhs, const csvsqldb::Timestamp& rhs)
  {
    std::size_t llen = ::strlen(lhs);
    std::string rval(rhs.format("%Y-%m-%dT%H:%M:%S"));
    std::size_t rlen = rval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], lhs, llen);
    ::strncpy(&c[llen], rval.c_str(), rlen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<>
  StringType op_concat<StringType>(const csvsqldb::Timestamp& lhs, const StringType& rhs)
  {
    std::size_t rlen = ::strlen(rhs);
    std::string lval(lhs.format("%Y-%m-%dT%H:%M:%S"));
    std::size_t llen = lval.length();
    StringType c = new char[llen + rlen + 1];
    ::strncpy(&c[0], rhs, rlen);
    ::strncpy(&c[rlen], lval.c_str(), llen);
    c[llen + rlen] = '\0';
    return c;
  }

  template<typename RET, typename CAST, typename LHS, typename RHS>
  struct OperationConcat : public BinaryOperationBase<OP_CONCAT, RET, LHS, RHS> {
  private:
    Variant doExecute(const Variant& lhs, const Variant& rhs) const override
    {
      return Variant(operation(ValueGetter<LHS>::getValue(lhs), ValueGetter<RHS>::getValue(rhs)), true);
    }

    RET operation(const LHS& lhs, const RHS& rhs) const override
    {
      return op_concat<RET>(lhs, rhs);
    }
  };


  using BinaryOperationType = std::map<OperationKey, BinaryOperationPtr>;
  BinaryOperationType g_binaryOperations;

  Variant binaryOperation(eOperationType op, const Variant& lhs, const Variant& rhs)
  {
    const auto iter = g_binaryOperations.find(OperationKey(op, lhs.getType(), rhs.getType()));

    if (iter != g_binaryOperations.end()) {
      return iter->second->execute(lhs, rhs);
    }
    throw std::runtime_error("cannot execute binary operation " + operationTypeToString(op) + " on types " +
                             typeToString(lhs.getType()) + " and " + typeToString(rhs.getType()));
  }

  eType inferTypeOfBinaryOperation(eOperationType op, eType lhs, eType rhs)
  {
    const auto iter = g_binaryOperations.find(OperationKey(op, lhs, rhs));

    if (iter != g_binaryOperations.end()) {
      return iter->second->retType();
    }
    throw std::runtime_error("cannot infer type of binary operation " + operationTypeToString(op) + " on types " +
                             typeToString(lhs) + " and " + typeToString(rhs));
  }


  struct UnaryOperation;
  using UnaryOperationPtr = std::unique_ptr<const UnaryOperation>;

  struct UnaryOperation {
    virtual eOperationType opType() const = 0;
    virtual eType rhsType() const = 0;
    virtual eType retType() const = 0;

    Variant execute(const Variant& rhs) const
    {
      if (rhs.isNull()) {
        return Variant(retType());
      }
      return doExecute(rhs);
    }

    OperationKey key() const
    {
      return OperationKey(opType(), retType(), rhsType());
    }

    virtual ~UnaryOperation() = default;

  private:
    virtual Variant doExecute(const Variant& rhs) const = 0;
  };

  template<eOperationType OP_TYPE, typename RET, typename RHS>
  struct UnaryOperationBase : public UnaryOperation {
    eOperationType opType() const override
    {
      return OP_TYPE;
    }

    eType rhsType() const override
    {
      return ctype2eType<RHS>();
    }

    eType retType() const override
    {
      return ctype2eType<RET>();
    }

  private:
    Variant doExecute(const Variant& rhs) const override
    {
      return operation(ValueGetter<RHS>::getValue(rhs));
    }

    virtual RET operation(const RHS& rhs) const = 0;
  };

  template<typename RHS>
  struct OperationNot : public UnaryOperationBase<OP_NOT, bool, RHS> {
  private:
    bool operation(const RHS& rhs) const override
    {
      return op_not(static_cast<bool>(rhs));
    }
  };

  template<typename RHS>
  struct OperationMinus : public UnaryOperationBase<OP_MINUS, RHS, RHS> {
  private:
    RHS operation(const RHS& rhs) const override
    {
      return op_minus(rhs);
    }
  };

  template<typename RHS>
  struct OperationPlus : public UnaryOperationBase<OP_PLUS, RHS, RHS> {
  private:
    RHS operation(const RHS& rhs) const override
    {
      return op_plus(rhs);
    }
  };

  template<typename CAST, typename RHS>
  struct OperationCast : public UnaryOperationBase<OP_CAST, CAST, RHS> {
  private:
    CAST operation(const RHS& rhs) const override
    {
      return _cast.operation(rhs);
    }

    Cast<CAST, RHS> _cast;
  };

  template<typename CAST>
  struct OperationNullCast : public UnaryOperationBase<OP_CAST, CAST, NoneType> {
  private:
    CAST operation(const NoneType& rhs) const override
    {
      CSVSQLDB_THROW(std::runtime_error, "Should have returned a null value before getting here");
    }
  };


  using UnaryOperationType = std::map<OperationKey, UnaryOperationPtr>;
  UnaryOperationType g_unaryOperations;

  Variant unaryOperation(eOperationType op, eType retType, const Variant& rhs)
  {
    const auto iter = g_unaryOperations.find(OperationKey(op, retType, rhs.getType()));

    if (iter != g_unaryOperations.end()) {
      return iter->second->execute(rhs);
    }
    if (op == OP_CAST) {
      throw std::runtime_error("cannot cast from type " + typeToString(rhs.getType()) + " to type " + typeToString(retType));
    } else {
      throw std::runtime_error("cannot execute unary operation " + operationTypeToString(op) + " on type " +
                               typeToString(rhs.getType()));
    }
  }

  eType inferTypeOfUnaryOperation(eOperationType op, eType retType, eType rhs)
  {
    const auto iter = g_unaryOperations.find(OperationKey(op, retType, rhs));

    if (iter != g_unaryOperations.end()) {
      return iter->second->retType();
    }
    if (op == OP_CAST) {
      throw std::runtime_error("cannot infer cast from type " + typeToString(rhs) + " to type " + typeToString(retType));
    } else {
      throw std::runtime_error("cannot infer type of unary operation " + operationTypeToString(op) + " on type " +
                               typeToString(rhs));
    }
  }


  void initTypeSystem()
  {
    using op_add_ii = OperationAdd<int64_t, int64_t, int64_t, int64_t>;
    using op_add_if = OperationAdd<double, double, int64_t, double>;
    using op_add_fi = OperationAdd<double, double, double, int64_t>;
    using op_add_ff = OperationAdd<double, double, double, double>;

    using op_sub_ii = OperationSub<int64_t, int64_t, int64_t, int64_t>;
    using op_sub_if = OperationSub<double, double, int64_t, double>;
    using op_sub_fi = OperationSub<double, double, double, int64_t>;
    using op_sub_ff = OperationSub<double, double, double, double>;
    using op_sub_dd = OperationSub<int64_t, csvsqldb::Date, csvsqldb::Date, csvsqldb::Date>;
    using op_sub_tt = OperationSub<int64_t, csvsqldb::Time, csvsqldb::Time, csvsqldb::Time>;
    using op_sub_zz = OperationSub<int64_t, csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp>;

    using op_mul_ii = OperationMul<int64_t, int64_t, int64_t, int64_t>;
    using op_mul_if = OperationMul<double, double, int64_t, double>;
    using op_mul_fi = OperationMul<double, double, double, int64_t>;
    using op_mul_ff = OperationMul<double, double, double, double>;

    using op_div_ii = OperationDiv<int64_t, int64_t, int64_t, int64_t>;
    using op_div_if = OperationDiv<double, double, int64_t, double>;
    using op_div_fi = OperationDiv<double, double, double, int64_t>;
    using op_div_ff = OperationDiv<double, double, double, double>;

    using op_mod_ii = OperationMod<int64_t, int64_t, int64_t, int64_t>;
    using op_mod_if = OperationMod<double, double, int64_t, double>;
    using op_mod_fi = OperationMod<double, double, double, int64_t>;
    using op_mod_ff = OperationMod<double, double, double, double>;

    using op_gt_ii = OperationGTCast<int64_t, int64_t, int64_t>;
    using op_gt_if = OperationGTCast<double, int64_t, double>;
    using op_gt_fi = OperationGTCast<double, double, int64_t>;
    using op_gt_ff = OperationGTCast<double, double, double>;
    using op_gt_ss = OperationGTCast<StringType, StringType, StringType>;
    using op_gt_dd = OperationGT<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date>;
    using op_gt_ds = OperationGT<csvsqldb::Date, csvsqldb::Date, StringType>;
    using op_gt_sd = OperationGT<csvsqldb::Date, StringType, csvsqldb::Date>;
    using op_gt_tt = OperationGT<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time>;
    using op_gt_ts = OperationGT<csvsqldb::Time, csvsqldb::Time, StringType>;
    using op_gt_st = OperationGT<csvsqldb::Time, StringType, csvsqldb::Time>;
    using op_gt_zz = OperationGT<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp>;
    using op_gt_zs = OperationGT<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType>;
    using op_gt_sz = OperationGT<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp>;

    using op_ge_ii = OperationGECast<int64_t, int64_t, int64_t>;
    using op_ge_if = OperationGECast<double, int64_t, double>;
    using op_ge_fi = OperationGECast<double, double, int64_t>;
    using op_ge_ff = OperationGECast<double, double, double>;
    using op_ge_ss = OperationGECast<StringType, StringType, StringType>;
    using op_ge_dd = OperationGE<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date>;
    using op_ge_ds = OperationGE<csvsqldb::Date, csvsqldb::Date, StringType>;
    using op_ge_sd = OperationGE<csvsqldb::Date, StringType, csvsqldb::Date>;
    using op_ge_tt = OperationGE<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time>;
    using op_ge_ts = OperationGE<csvsqldb::Time, csvsqldb::Time, StringType>;
    using op_ge_st = OperationGE<csvsqldb::Time, StringType, csvsqldb::Time>;
    using op_ge_zz = OperationGE<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp>;
    using op_ge_zs = OperationGE<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType>;
    using op_ge_sz = OperationGE<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp>;

    using op_lt_ii = OperationLTCast<int64_t, int64_t, int64_t>;
    using op_lt_if = OperationLTCast<double, int64_t, double>;
    using op_lt_fi = OperationLTCast<double, double, int64_t>;
    using op_lt_ff = OperationLTCast<double, double, double>;
    using op_lt_ss = OperationLTCast<StringType, StringType, StringType>;
    using op_lt_dd = OperationLT<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date>;
    using op_lt_ds = OperationLT<csvsqldb::Date, csvsqldb::Date, StringType>;
    using op_lt_sd = OperationLT<csvsqldb::Date, StringType, csvsqldb::Date>;
    using op_lt_tt = OperationLT<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time>;
    using op_lt_ts = OperationLT<csvsqldb::Time, csvsqldb::Time, StringType>;
    using op_lt_st = OperationLT<csvsqldb::Time, StringType, csvsqldb::Time>;
    using op_lt_zz = OperationLT<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp>;
    using op_lt_zs = OperationLT<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType>;
    using op_lt_sz = OperationLT<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp>;

    using op_le_ii = OperationLECast<int64_t, int64_t, int64_t>;
    using op_le_if = OperationLECast<double, int64_t, double>;
    using op_le_fi = OperationLECast<double, double, int64_t>;
    using op_le_ff = OperationLECast<double, double, double>;
    using op_le_ss = OperationLECast<StringType, StringType, StringType>;
    using op_le_dd = OperationLE<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date>;
    using op_le_ds = OperationLE<csvsqldb::Date, csvsqldb::Date, StringType>;
    using op_le_sd = OperationLE<csvsqldb::Date, StringType, csvsqldb::Date>;
    using op_le_tt = OperationLE<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time>;
    using op_le_ts = OperationLE<csvsqldb::Time, csvsqldb::Time, StringType>;
    using op_le_st = OperationLE<csvsqldb::Time, StringType, csvsqldb::Time>;
    using op_le_zz = OperationLE<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp>;
    using op_le_zs = OperationLE<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType>;
    using op_le_sz = OperationLE<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp>;

    using op_eq_bb = OperationEQCast<bool, bool, bool>;
    using op_eq_bi = OperationEQCast<bool, bool, int64_t>;
    using op_eq_ib = OperationEQCast<bool, int64_t, bool>;
    using op_eq_bf = OperationEQCast<bool, bool, double>;
    using op_eq_fb = OperationEQCast<bool, double, bool>;
    using op_eq_ii = OperationEQCast<int64_t, int64_t, int64_t>;
    using op_eq_if = OperationEQCast<double, int64_t, double>;
    using op_eq_fi = OperationEQCast<double, double, int64_t>;
    using op_eq_ff = OperationEQCast<double, double, double>;
    using op_eq_ss = OperationEQCast<StringType, StringType, StringType>;
    using op_eq_dd = OperationEQ<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date>;
    using op_eq_ds = OperationEQ<csvsqldb::Date, csvsqldb::Date, StringType>;
    using op_eq_sd = OperationEQ<csvsqldb::Date, StringType, csvsqldb::Date>;
    using op_eq_tt = OperationEQ<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time>;
    using op_eq_ts = OperationEQ<csvsqldb::Time, csvsqldb::Time, StringType>;
    using op_eq_st = OperationEQ<csvsqldb::Time, StringType, csvsqldb::Time>;
    using op_eq_zz = OperationEQ<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp>;
    using op_eq_zs = OperationEQ<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType>;
    using op_eq_sz = OperationEQ<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp>;

    using op_neq_bb = OperationNEQCast<bool, bool, bool>;
    using op_neq_bi = OperationNEQCast<bool, bool, int64_t>;
    using op_neq_ib = OperationNEQCast<bool, int64_t, bool>;
    using op_neq_bf = OperationNEQCast<bool, bool, double>;
    using op_neq_fb = OperationNEQCast<bool, double, bool>;
    using op_neq_ii = OperationNEQCast<int64_t, int64_t, int64_t>;
    using op_neq_if = OperationNEQCast<double, int64_t, double>;
    using op_neq_fi = OperationNEQCast<double, double, int64_t>;
    using op_neq_ff = OperationNEQCast<double, double, double>;
    using op_neq_ss = OperationNEQCast<StringType, StringType, StringType>;
    using op_neq_dd = OperationNEQ<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date>;
    using op_neq_ds = OperationNEQ<csvsqldb::Date, csvsqldb::Date, StringType>;
    using op_neq_sd = OperationNEQ<csvsqldb::Date, StringType, csvsqldb::Date>;
    using op_neq_tt = OperationNEQ<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time>;
    using op_neq_ts = OperationNEQ<csvsqldb::Time, csvsqldb::Time, StringType>;
    using op_neq_st = OperationNEQ<csvsqldb::Time, StringType, csvsqldb::Time>;
    using op_neq_zz = OperationNEQ<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp>;
    using op_neq_zs = OperationNEQ<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType>;
    using op_neq_sz = OperationNEQ<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp>;

    using op_is_bb = OperationIs<bool, bool>;
    using op_is_ib = OperationIs<int64_t, bool>;
    using op_is_fb = OperationIs<double, bool>;
    using op_is_sb = OperationIs<StringType, bool>;
    using op_is_db = OperationIs<csvsqldb::Date, bool>;
    using op_is_tb = OperationIs<csvsqldb::Time, bool>;
    using op_is_zb = OperationIs<csvsqldb::Timestamp, bool>;

    using op_isnot_bb = OperationIsNot<bool, bool>;
    using op_isnot_ib = OperationIsNot<int64_t, bool>;
    using op_isnot_fb = OperationIsNot<double, bool>;
    using op_isnot_sb = OperationIsNot<StringType, bool>;
    using op_isnot_db = OperationIsNot<csvsqldb::Date, bool>;
    using op_isnot_tb = OperationIsNot<csvsqldb::Time, bool>;
    using op_isnot_zb = OperationIsNot<csvsqldb::Timestamp, bool>;

    using op_and_bb = OperationAnd<bool, bool>;
    using op_and_bi = OperationAnd<bool, int64_t>;
    using op_and_ib = OperationAnd<int64_t, bool>;
    using op_and_fb = OperationAnd<double, bool>;
    using op_and_bf = OperationAnd<bool, double>;
    using op_and_ii = OperationAnd<int64_t, int64_t>;
    using op_and_if = OperationAnd<int64_t, double>;
    using op_and_fi = OperationAnd<double, int64_t>;
    using op_and_ff = OperationAnd<double, double>;
    using op_and_bs = OperationAnd<bool, StringType>;
    using op_and_sb = OperationAnd<StringType, bool>;
    using op_and_is = OperationAnd<int64_t, StringType>;
    using op_and_si = OperationAnd<StringType, int64_t>;
    using op_and_fs = OperationAnd<double, StringType>;
    using op_and_sf = OperationAnd<StringType, double>;
    using op_and_ss = OperationAnd<StringType, StringType>;
    using op_and_db = OperationAnd<csvsqldb::Date, bool>;
    using op_and_bd = OperationAnd<bool, csvsqldb::Date>;
    using op_and_dd = OperationAnd<csvsqldb::Date, csvsqldb::Date>;
    using op_and_tb = OperationAnd<csvsqldb::Time, bool>;
    using op_and_bt = OperationAnd<bool, csvsqldb::Time>;
    using op_and_tt = OperationAnd<csvsqldb::Time, csvsqldb::Time>;

    using op_or_bb = OperationOr<bool, bool>;
    using op_or_bi = OperationOr<bool, int64_t>;
    using op_or_ib = OperationOr<int64_t, bool>;
    using op_or_fb = OperationOr<double, bool>;
    using op_or_bf = OperationOr<bool, double>;
    using op_or_ii = OperationOr<int64_t, int64_t>;
    using op_or_if = OperationOr<int64_t, double>;
    using op_or_fi = OperationOr<double, int64_t>;
    using op_or_ff = OperationOr<double, double>;
    using op_or_bs = OperationOr<bool, StringType>;
    using op_or_sb = OperationOr<StringType, bool>;
    using op_or_is = OperationOr<int64_t, StringType>;
    using op_or_si = OperationOr<StringType, int64_t>;
    using op_or_fs = OperationOr<double, StringType>;
    using op_or_sf = OperationOr<StringType, double>;
    using op_or_ss = OperationOr<StringType, StringType>;

    using op_concat_ss = OperationConcat<StringType, StringType, StringType, StringType>;
    using op_concat_sl = OperationConcat<StringType, StringType, StringType, int64_t>;
    using op_concat_ls = OperationConcat<StringType, StringType, int64_t, StringType>;
    using op_concat_sf = OperationConcat<StringType, StringType, StringType, double>;
    using op_concat_fs = OperationConcat<StringType, StringType, double, StringType>;
    using op_concat_ds = OperationConcat<StringType, StringType, csvsqldb::Date, StringType>;
    using op_concat_sd = OperationConcat<StringType, StringType, StringType, csvsqldb::Date>;
    using op_concat_ts = OperationConcat<StringType, StringType, csvsqldb::Time, StringType>;
    using op_concat_st = OperationConcat<StringType, StringType, StringType, csvsqldb::Time>;
    using op_concat_zs = OperationConcat<StringType, StringType, csvsqldb::Timestamp, StringType>;
    using op_concat_sz = OperationConcat<StringType, StringType, StringType, csvsqldb::Timestamp>;

    if (g_binaryOperations.empty()) {
      BinaryOperationPtr operation;
      operation = std::make_unique<op_add_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_add_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_add_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_add_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_sub_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_sub_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_sub_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_sub_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_sub_dd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_sub_tt>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_sub_zz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_mul_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_mul_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_mul_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_mul_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_div_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_div_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_div_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_div_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_mod_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_mod_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_mod_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_mod_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_gt_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_ss>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_dd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_ds>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_sd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_tt>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_ts>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_st>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_zz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_zs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_gt_sz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_ge_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_ss>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_dd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_ds>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_sd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_tt>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_ts>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_st>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_zz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_zs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_ge_sz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_lt_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_ss>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_dd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_ds>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_sd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_tt>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_ts>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_st>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_zz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_zs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_lt_sz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_le_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_ss>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_dd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_ds>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_sd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_tt>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_ts>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_st>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_zz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_zs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_le_sz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_eq_bb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_bi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_ib>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_bf>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_fb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_ss>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_dd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_ds>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_sd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_tt>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_ts>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_st>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_zz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_zs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_eq_sz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_neq_bb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_bi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_ib>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_bf>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_fb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_ss>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_dd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_ds>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_sd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_tt>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_ts>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_st>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_zz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_zs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_neq_sz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_is_bb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_is_ib>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_is_fb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_is_sb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_is_db>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_is_tb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_is_zb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_isnot_bb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_isnot_ib>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_isnot_fb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_isnot_sb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_isnot_db>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_isnot_tb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_isnot_zb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_and_bb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_bi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_ib>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_fb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_bf>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_bs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_sb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_is>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_si>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_fs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_sf>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_ss>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_bd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_db>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_dd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_bt>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_tb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_and_tt>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_or_bb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_bi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_ib>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_fb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_bf>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_ii>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_if>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_fi>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_ff>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_bs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_sb>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_is>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_si>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_fs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_sf>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_or_ss>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_concat_ss>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_sl>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_ls>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_sf>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_fs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_ds>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_sd>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_ts>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_st>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_zs>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_concat_sz>();
      g_binaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    using op_not_b = OperationNot<bool>;
    using op_not_i = OperationNot<int64_t>;
    using op_not_f = OperationNot<double>;

    using op_minus_i = OperationMinus<int64_t>;
    using op_minus_f = OperationMinus<double>;

    using op_plus_i = OperationPlus<int64_t>;
    using op_plus_f = OperationPlus<double>;

    using op_cast_ii = OperationCast<int64_t, int64_t>;
    using op_cast_fi = OperationCast<int64_t, double>;
    using op_cast_ff = OperationCast<double, double>;
    using op_cast_if = OperationCast<double, int64_t>;
    using op_cast_bi = OperationCast<int64_t, bool>;
    using op_cast_bb = OperationCast<bool, bool>;
    using op_cast_ib = OperationCast<bool, int64_t>;
    // using op_cast_fb = OperationCast<bool, double>;
    using op_cast_ss = OperationCast<StringType, StringType>;
    using op_cast_sb = OperationCast<bool, StringType>;
    using op_cast_sf = OperationCast<double, StringType>;
    using op_cast_si = OperationCast<int64_t, StringType>;
    using op_cast_sd = OperationCast<csvsqldb::Date, StringType>;
    using op_cast_st = OperationCast<csvsqldb::Time, StringType>;
    using op_cast_sz = OperationCast<csvsqldb::Timestamp, StringType>;
    using op_cast_dz = OperationCast<csvsqldb::Timestamp, csvsqldb::Date>;
    using op_cast_tz = OperationCast<csvsqldb::Timestamp, csvsqldb::Time>;
    using op_null_cast_nb = OperationNullCast<bool>;
    using op_null_cast_ni = OperationNullCast<int64_t>;
    using op_null_cast_nf = OperationNullCast<double>;
    using op_null_cast_ns = OperationNullCast<StringType>;
    using op_null_cast_nd = OperationNullCast<csvsqldb::Date>;
    using op_null_cast_nt = OperationNullCast<csvsqldb::Time>;
    using op_null_cast_nz = OperationNullCast<csvsqldb::Timestamp>;

    if (g_unaryOperations.empty()) {
      UnaryOperationPtr operation;
      operation = std::make_unique<op_not_b>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_not_i>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_not_f>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_minus_i>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_minus_f>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_plus_i>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_plus_f>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));

      operation = std::make_unique<op_cast_ii>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_fi>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_ff>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_if>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_bb>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_bi>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_ib>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_ss>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_sb>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_sf>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_si>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_sd>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_st>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_sz>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_dz>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_cast_tz>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_null_cast_nb>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_null_cast_ni>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_null_cast_nf>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_null_cast_ns>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_null_cast_nd>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_null_cast_nt>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
      operation = std::make_unique<op_null_cast_nz>();
      g_unaryOperations.insert(std::make_pair(operation->key(), std::move(operation)));
    }
  }
}
