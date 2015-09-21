//
//  typeoperations.cpp
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

#include "typeoperations.h"

#include "types.h"

#include "base/date.h"
#include "base/float_helper.h"
#include "base/time_helper.h"

#include <cmath>
#include <map>
#include <stdexcept>


namespace csvsqldb
{

    template <typename RET, typename L, typename R>
    RET op_add(const L& lhs, const R& rhs)
    {
        return lhs + rhs;
    }

    template <typename RET, typename L, typename R>
    RET op_sub(const L& lhs, const R& rhs)
    {
        return lhs - rhs;
    }

    template <typename RET, typename L, typename R>
    RET op_mul(const L& lhs, const R& rhs)
    {
        return lhs * rhs;
    }

    template <typename RET, typename L, typename R>
    RET op_div(const L& lhs, const R& rhs)
    {
        return lhs / rhs;
    }

    template <typename L, typename R>
    L op_mod(const L& lhs, const R& rhs)
    {
        return lhs % rhs;
    }

    static double op_mod(const double& lhs, const double& rhs)
    {
        return std::fmod(lhs, rhs);
    }


    template <typename L, typename R>
    struct GreaterThen {
        bool operation(const L& lhs, const R& rhs) const
        {
            return lhs > rhs;
        }
    };

    template <>
    struct GreaterThen<StringType, StringType> {
        bool operation(const StringType& lhs, const StringType& rhs) const
        {
            return ::strcoll(lhs, rhs) > 0;
        }
    };

    template <>
    struct GreaterThen<csvsqldb::Date, StringType> {
        bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
        {
            return lhs > csvsqldb::dateFromString(rhs);
        }
    };

    template <>
    struct GreaterThen<StringType, csvsqldb::Date> {
        bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
        {
            return csvsqldb::dateFromString(lhs) > rhs;
        }
    };

    template <>
    struct GreaterThen<csvsqldb::Time, StringType> {
        bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
        {
            return lhs > csvsqldb::timeFromString(rhs);
        }
    };

    template <>
    struct GreaterThen<StringType, csvsqldb::Time> {
        bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
        {
            return csvsqldb::timeFromString(lhs) > rhs;
        }
    };

    template <>
    struct GreaterThen<csvsqldb::Timestamp, StringType> {
        bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
        {
            return lhs > csvsqldb::timestampFromString(rhs);
        }
    };

    template <>
    struct GreaterThen<StringType, csvsqldb::Timestamp> {
        bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
        {
            return csvsqldb::timestampFromString(lhs) > rhs;
        }
    };


    template <typename L, typename R>
    struct GreaterEqual {
        bool operation(const L& lhs, const R& rhs) const
        {
            return lhs >= rhs;
        }
    };

    template <>
    struct GreaterEqual<StringType, StringType> {
        bool operation(const StringType& lhs, const StringType& rhs) const
        {
            return ::strcoll(lhs, rhs) >= 0;
        }
    };

    template <>
    struct GreaterEqual<csvsqldb::Date, StringType> {
        bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
        {
            return lhs >= csvsqldb::dateFromString(rhs);
        }
    };

    template <>
    struct GreaterEqual<StringType, csvsqldb::Date> {
        bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
        {
            return csvsqldb::dateFromString(lhs) >= rhs;
        }
    };

    template <>
    struct GreaterEqual<csvsqldb::Time, StringType> {
        bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
        {
            return lhs >= csvsqldb::timeFromString(rhs);
        }
    };

    template <>
    struct GreaterEqual<StringType, csvsqldb::Time> {
        bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
        {
            return csvsqldb::timeFromString(lhs) >= rhs;
        }
    };

    template <>
    struct GreaterEqual<csvsqldb::Timestamp, StringType> {
        bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
        {
            return lhs >= csvsqldb::timestampFromString(rhs);
        }
    };

    template <>
    struct GreaterEqual<StringType, csvsqldb::Timestamp> {
        bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
        {
            return csvsqldb::timestampFromString(lhs) >= rhs;
        }
    };


    template <typename L, typename R>
    struct LessThen {
        bool operation(const L& lhs, const R& rhs) const
        {
            return lhs < rhs;
        }
    };

    template <>
    struct LessThen<StringType, StringType> {
        bool operation(const StringType& lhs, const StringType& rhs) const
        {
            return ::strcoll(lhs, rhs) < 0;
        }
    };

    template <>
    struct LessThen<csvsqldb::Date, StringType> {
        bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
        {
            return lhs < csvsqldb::dateFromString(rhs);
        }
    };

    template <>
    struct LessThen<StringType, csvsqldb::Date> {
        bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
        {
            return csvsqldb::dateFromString(lhs) < rhs;
        }
    };

    template <>
    struct LessThen<csvsqldb::Time, StringType> {
        bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
        {
            return lhs < csvsqldb::timeFromString(rhs);
        }
    };

    template <>
    struct LessThen<StringType, csvsqldb::Time> {
        bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
        {
            return csvsqldb::timeFromString(lhs) < rhs;
        }
    };

    template <>
    struct LessThen<csvsqldb::Timestamp, StringType> {
        bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
        {
            return lhs < csvsqldb::timestampFromString(rhs);
        }
    };

    template <>
    struct LessThen<StringType, csvsqldb::Timestamp> {
        bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
        {
            return csvsqldb::timestampFromString(lhs) < rhs;
        }
    };


    template <typename L, typename R>
    struct LessEqual {
        bool operation(const L& lhs, const R& rhs) const
        {
            return lhs <= rhs;
        }
    };

    template <>
    struct LessEqual<StringType, StringType> {
        bool operation(const StringType& lhs, const StringType& rhs) const
        {
            return ::strcoll(lhs, rhs) <= 0;
        }
    };

    template <>
    struct LessEqual<csvsqldb::Date, StringType> {
        bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
        {
            return lhs <= csvsqldb::dateFromString(rhs);
        }
    };

    template <>
    struct LessEqual<StringType, csvsqldb::Date> {
        bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
        {
            return csvsqldb::dateFromString(lhs) <= rhs;
        }
    };

    template <>
    struct LessEqual<csvsqldb::Time, StringType> {
        bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
        {
            return lhs <= csvsqldb::timeFromString(rhs);
        }
    };

    template <>
    struct LessEqual<StringType, csvsqldb::Time> {
        bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
        {
            return csvsqldb::timeFromString(lhs) <= rhs;
        }
    };

    template <>
    struct LessEqual<csvsqldb::Timestamp, StringType> {
        bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
        {
            return lhs <= csvsqldb::timestampFromString(rhs);
        }
    };

    template <>
    struct LessEqual<StringType, csvsqldb::Timestamp> {
        bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
        {
            return csvsqldb::timestampFromString(lhs) <= rhs;
        }
    };


    template <typename L, typename R>
    struct CompareEqual {
        bool operation(const L& lhs, const R& rhs) const
        {
            return lhs == rhs;
        }
    };

    template <>
    struct CompareEqual<double, double> {
        bool operation(const double& lhs, const double& rhs) const
        {
            return csvsqldb::compare(lhs, rhs);
        }
    };

    template <>
    struct CompareEqual<StringType, StringType> {
        bool operation(const StringType& lhs, const StringType& rhs) const
        {
            return ::strcoll(lhs, rhs) == 0;
        }
    };

    template <>
    struct CompareEqual<csvsqldb::Date, StringType> {
        bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
        {
            return lhs == csvsqldb::dateFromString(rhs);
        }
    };

    template <>
    struct CompareEqual<StringType, csvsqldb::Date> {
        bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
        {
            return csvsqldb::dateFromString(lhs) == rhs;
        }
    };

    template <>
    struct CompareEqual<csvsqldb::Time, StringType> {
        bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
        {
            return lhs == csvsqldb::timeFromString(rhs);
        }
    };

    template <>
    struct CompareEqual<StringType, csvsqldb::Time> {
        bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
        {
            return csvsqldb::timeFromString(lhs) == rhs;
        }
    };

    template <>
    struct CompareEqual<csvsqldb::Timestamp, StringType> {
        bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
        {
            return lhs == csvsqldb::timestampFromString(rhs);
        }
    };

    template <>
    struct CompareEqual<StringType, csvsqldb::Timestamp> {
        bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
        {
            return csvsqldb::timestampFromString(lhs) == rhs;
        }
    };


    template <typename L, typename R>
    struct CompareNotEqual {
        bool operation(const L& lhs, const R& rhs) const
        {
            return lhs != rhs;
        }
    };

    template <>
    struct CompareNotEqual<double, double> {
        bool operation(const double& lhs, const double& rhs) const
        {
            return !csvsqldb::compare(lhs, rhs);
        }
    };

    template <>
    struct CompareNotEqual<StringType, StringType> {
        bool operation(const StringType& lhs, const StringType& rhs) const
        {
            return ::strcoll(lhs, rhs) != 0;
        }
    };

    template <>
    struct CompareNotEqual<csvsqldb::Date, StringType> {
        bool operation(const csvsqldb::Date& lhs, const StringType& rhs) const
        {
            return lhs != csvsqldb::dateFromString(rhs);
        }
    };

    template <>
    struct CompareNotEqual<StringType, csvsqldb::Date> {
        bool operation(const StringType& lhs, const csvsqldb::Date& rhs) const
        {
            return csvsqldb::dateFromString(lhs) != rhs;
        }
    };

    template <>
    struct CompareNotEqual<csvsqldb::Time, StringType> {
        bool operation(const csvsqldb::Time& lhs, const StringType& rhs) const
        {
            return lhs != csvsqldb::timeFromString(rhs);
        }
    };

    template <>
    struct CompareNotEqual<StringType, csvsqldb::Time> {
        bool operation(const StringType& lhs, const csvsqldb::Time& rhs) const
        {
            return csvsqldb::timeFromString(lhs) != rhs;
        }
    };

    template <>
    struct CompareNotEqual<csvsqldb::Timestamp, StringType> {
        bool operation(const csvsqldb::Timestamp& lhs, const StringType& rhs) const
        {
            return lhs != csvsqldb::timestampFromString(rhs);
        }
    };

    template <>
    struct CompareNotEqual<StringType, csvsqldb::Timestamp> {
        bool operation(const StringType& lhs, const csvsqldb::Timestamp& rhs) const
        {
            return csvsqldb::timestampFromString(lhs) != rhs;
        }
    };


    template <typename L, typename R>
    struct And {
        bool operation(const L& lhs, const R& rhs) const
        {
            return static_cast<bool>(lhs) && static_cast<bool>(rhs);
        }
    };

    template <typename L>
    struct And<L, StringType> {
        bool operation(const L& lhs, const StringType& rhs) const
        {
            return static_cast<bool>(lhs) && rhs;
        }
    };

    template <typename R>
    struct And<StringType, R> {
        bool operation(const StringType& lhs, const R& rhs) const
        {
            return lhs && static_cast<bool>(rhs);
        }
    };

    template <>
    struct And<StringType, StringType> {
        bool operation(const StringType& lhs, const StringType& rhs) const
        {
            return lhs && rhs;
        }
    };

    template <typename L, typename R>
    struct Or {
        bool operation(const L& lhs, const R& rhs) const
        {
            return static_cast<bool>(lhs) || static_cast<bool>(rhs);
        }
    };

    template <typename L>
    struct Or<L, StringType> {
        bool operation(const L& lhs, const StringType& rhs) const
        {
            return static_cast<bool>(lhs) || rhs;
        }
    };

    template <typename R>
    struct Or<StringType, R> {
        bool operation(const StringType& lhs, const R& rhs) const
        {
            return lhs || static_cast<bool>(rhs);
        }
    };

    template <>
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

    template <typename R>
    R op_minus(const R& rhs)
    {
        return -rhs;
    }

    template <typename R>
    R op_plus(const R& rhs)
    {
        return +rhs;
    }

    template <typename RET, typename R>
    struct Cast {
        RET operation(const R& rhs) const
        {
            return static_cast<RET>(rhs);
        }
    };

    template <>
    struct Cast<bool, StringType> {
        bool operation(const StringType& rhs) const
        {
            return rhs;
        }
    };

    template <>
    struct Cast<double, StringType> {
        double operation(const StringType& rhs) const
        {
            return ::atof(rhs);
        }
    };

    template <>
    struct Cast<int64_t, StringType> {
        int64_t operation(const StringType& rhs) const
        {
            return ::atol(rhs);
        }
    };

    template <>
    struct Cast<csvsqldb::Date, StringType> {
        csvsqldb::Date operation(const StringType& rhs) const
        {
            return csvsqldb::dateFromString(rhs);
        }
    };

    template <>
    struct Cast<csvsqldb::Time, StringType> {
        csvsqldb::Time operation(const StringType& rhs) const
        {
            return csvsqldb::timeFromString(rhs);
        }
    };

    template <>
    struct Cast<csvsqldb::Timestamp, StringType> {
        csvsqldb::Timestamp operation(const StringType& rhs) const
        {
            return csvsqldb::timestampFromString(rhs);
        }
    };

    template <>
    struct Cast<csvsqldb::Timestamp, csvsqldb::Date> {
        csvsqldb::Timestamp operation(const csvsqldb::Date& rhs) const
        {
            return csvsqldb::Timestamp(rhs);
        }
    };

    template <>
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
            if(_op < key._op) {
                return true;
            } else if(_op == key._op && _lhs < key._lhs) {
                return true;
            } else if(_op == key._op && _lhs == key._lhs && _rhs < key._rhs) {
                return true;
            }
            return false;
        }

        eOperationType _op;
        eType _lhs;
        eType _rhs;
    };

    struct BinaryOperation;
    typedef std::shared_ptr<const BinaryOperation> BinaryOperationPtr;

    struct BinaryOperation {
        virtual eOperationType opType() const = 0;
        virtual eType lhsType() const = 0;
        virtual eType rhsType() const = 0;
        virtual eType retType() const = 0;

        Variant execute(const Variant& lhs, const Variant& rhs) const
        {
            if(lhs.isNull() || rhs.isNull()) {
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

    template <eOperationType OP_TYPE, typename RET, typename LHS, typename RHS>
    struct BinaryOperationBase : public BinaryOperation {
        virtual eOperationType opType() const
        {
            return OP_TYPE;
        }

        virtual eType lhsType() const
        {
            return ctype2eType<LHS>();
        }

        virtual eType rhsType() const
        {
            return ctype2eType<RHS>();
        }

        virtual eType retType() const
        {
            return ctype2eType<RET>();
        }

    private:
        virtual Variant doExecute(const Variant& lhs, const Variant& rhs) const
        {
            return Variant(operation(ValueGetter<LHS>::getValue(lhs), ValueGetter<RHS>::getValue(rhs)));
        }

        virtual Variant doHandleNull(const Variant& lhs, const Variant& rhs) const
        {
            return Variant(retType());
        }

        virtual RET operation(const LHS& lhs, const RHS& rhs) const = 0;
    };

    template <typename RET, typename CAST, typename LHS, typename RHS>
    struct OperationAdd : public BinaryOperationBase<OP_ADD, RET, LHS, RHS> {
    private:
        virtual RET operation(const LHS& lhs, const RHS& rhs) const
        {
            return op_add<RET, CAST, CAST>(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }
    };

    template <typename RET, typename CAST, typename LHS, typename RHS>
    struct OperationSub : public BinaryOperationBase<OP_SUB, RET, LHS, RHS> {
    private:
        virtual RET operation(const LHS& lhs, const RHS& rhs) const
        {
            return op_sub<RET, CAST, CAST>(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }
    };

    template <typename RET, typename CAST, typename LHS, typename RHS>
    struct OperationMul : public BinaryOperationBase<OP_MUL, RET, LHS, RHS> {
    private:
        virtual RET operation(const LHS& lhs, const RHS& rhs) const
        {
            return op_mul<RET, CAST, CAST>(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }
    };

    template <typename RET, typename CAST, typename LHS, typename RHS>
    struct OperationDiv : public BinaryOperationBase<OP_DIV, RET, LHS, RHS> {
    private:
        virtual RET operation(const LHS& lhs, const RHS& rhs) const
        {
            return op_div<RET, CAST, CAST>(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }
    };

    template <typename RET, typename CAST, typename LHS, typename RHS>
    struct OperationMod : public BinaryOperationBase<OP_MOD, RET, LHS, RHS> {
    private:
        virtual RET operation(const LHS& lhs, const RHS& rhs) const
        {
            return op_mod(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }
    };

    template <typename CAST, typename LHS, typename RHS>
    struct OperationGTCast : public BinaryOperationBase<OP_GT, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _greater.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }

        GreaterThen<CAST, CAST> _greater;
    };

    template <typename RET, typename LHS, typename RHS>
    struct OperationGT : public BinaryOperationBase<OP_GT, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _greater.operation(lhs, rhs);
        }

        GreaterThen<LHS, RHS> _greater;
    };

    template <typename CAST, typename LHS, typename RHS>
    struct OperationGECast : public BinaryOperationBase<OP_GE, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _greater.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }

        GreaterEqual<CAST, CAST> _greater;
    };

    template <typename RET, typename LHS, typename RHS>
    struct OperationGE : public BinaryOperationBase<OP_GE, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _greater.operation(lhs, rhs);
        }

        GreaterEqual<LHS, RHS> _greater;
    };

    template <typename CAST, typename LHS, typename RHS>
    struct OperationLTCast : public BinaryOperationBase<OP_LT, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _lesser.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }

        LessThen<CAST, CAST> _lesser;
    };

    template <typename RET, typename LHS, typename RHS>
    struct OperationLT : public BinaryOperationBase<OP_LT, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _lesser.operation(lhs, rhs);
        }

        LessThen<LHS, RHS> _lesser;
    };

    template <typename CAST, typename LHS, typename RHS>
    struct OperationLECast : public BinaryOperationBase<OP_LE, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _lesser.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }

        LessEqual<CAST, CAST> _lesser;
    };

    template <typename RET, typename LHS, typename RHS>
    struct OperationLE : public BinaryOperationBase<OP_LE, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _lesser.operation(lhs, rhs);
        }

        LessEqual<LHS, RHS> _lesser;
    };

    template <typename CAST, typename LHS, typename RHS>
    struct OperationEQCast : public BinaryOperationBase<OP_EQ, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _compare.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }

        CompareEqual<CAST, CAST> _compare;
    };

    template <typename CAST, typename LHS, typename RHS>
    struct OperationEQ : public BinaryOperationBase<OP_EQ, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _compare.operation(lhs, rhs);
        }

        CompareEqual<LHS, RHS> _compare;
    };

    template <typename CAST, typename LHS, typename RHS>
    struct OperationNEQCast : public BinaryOperationBase<OP_NEQ, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _compare.operation(static_cast<const CAST&>(lhs), static_cast<const CAST&>(rhs));
        }

        CompareNotEqual<CAST, CAST> _compare;
    };

    template <typename CAST, typename LHS, typename RHS>
    struct OperationNEQ : public BinaryOperationBase<OP_NEQ, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _compare.operation(lhs, rhs);
        }

        CompareNotEqual<LHS, RHS> _compare;
    };

    template <typename LHS, typename RHS>
    struct OperationIs : public BinaryOperationBase<OP_IS, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return lhs && rhs;
        }

        virtual Variant doHandleNull(const Variant& lhs, const Variant& rhs) const
        {
            if(rhs.isNull()) {
                return Variant(lhs.isNull());
            } else if(lhs.isNull()) {
                return Variant(false);
            }
            return Variant(lhs.asBool() && rhs.asBool());
        }
    };

    template <>
    struct OperationIs<double, bool> : public BinaryOperationBase<OP_IS, bool, double, bool> {
    private:
        virtual bool operation(const double& lhs, const bool& rhs) const
        {
            return !csvsqldb::compare(lhs, 0.0) && rhs;
        }

        virtual Variant doHandleNull(const Variant& lhs, const Variant& rhs) const
        {
            if(rhs.isNull()) {
                return Variant(lhs.isNull());
            } else if(lhs.isNull()) {
                return Variant(false);
            }
            return Variant(lhs.asBool() && rhs.asBool());
        }
    };

    template <typename LHS, typename RHS>
    struct OperationIsNot : public BinaryOperationBase<OP_ISNOT, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return !(lhs && rhs);
        }

        virtual Variant doHandleNull(const Variant& lhs, const Variant& rhs) const
        {
            if(rhs.isNull()) {
                return Variant(!lhs.isNull());
            } else if(lhs.isNull()) {
                return Variant(true);
            }
            return Variant(!(lhs.asBool() && rhs.asBool()));
        }
    };

    template <>
    struct OperationIsNot<double, bool> : public BinaryOperationBase<OP_ISNOT, bool, double, bool> {
    private:
        virtual bool operation(const double& lhs, const bool& rhs) const
        {
            return !(!csvsqldb::compare(lhs, 0.0) && rhs);
        }

        virtual Variant doHandleNull(const Variant& lhs, const Variant& rhs) const
        {
            if(rhs.isNull()) {
                return Variant(!lhs.isNull());
            } else if(lhs.isNull()) {
                return Variant(true);
            }
            return Variant(!(lhs.asBool() && rhs.asBool()));
        }
    };

    template <typename LHS, typename RHS>
    struct OperationAnd : public BinaryOperationBase<OP_AND, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _and.operation(lhs, rhs);
        }

        virtual Variant doHandleNull(const Variant& lhs, const Variant& rhs) const
        {
            if(lhs.isNull() && !rhs.isNull() && !rhs.asBool()) {
                return Variant(false);
            }
            if(rhs.isNull() && !lhs.isNull() && !lhs.asBool()) {
                return Variant(false);
            }
            return Variant(BOOLEAN);
        }

        And<LHS, RHS> _and;
    };

    template <typename LHS, typename RHS>
    struct OperationOr : public BinaryOperationBase<OP_OR, bool, LHS, RHS> {
    private:
        virtual bool operation(const LHS& lhs, const RHS& rhs) const
        {
            return _or.operation(lhs, rhs);
        }

        virtual Variant doHandleNull(const Variant& lhs, const Variant& rhs) const
        {
            if(!lhs.isNull() && rhs.isNull() && lhs.asBool()) {
                return Variant(true);
            }
            if(!rhs.isNull() && lhs.isNull() && rhs.asBool()) {
                return Variant(true);
            }
            return Variant(BOOLEAN);
        }

        Or<LHS, RHS> _or;
    };

    template <typename RET, typename L, typename R>
    RET op_concat(const L& lhs, const R& rhs);

    template <>
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

    template <>
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

    template <>
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

    template <>
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

    template <>
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

    template <>
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

    template <>
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

    template <>
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

    template <>
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

    template <>
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

    template <>
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

    template <typename RET, typename CAST, typename LHS, typename RHS>
    struct OperationConcat : public BinaryOperationBase<OP_CONCAT, RET, LHS, RHS> {
    private:
        virtual Variant doExecute(const Variant& lhs, const Variant& rhs) const
        {
            return Variant(operation(ValueGetter<LHS>::getValue(lhs), ValueGetter<RHS>::getValue(rhs)), true);
        }

        virtual RET operation(const LHS& lhs, const RHS& rhs) const
        {
            return op_concat<RET>(lhs, rhs);
        }
    };


    typedef std::map<OperationKey, BinaryOperationPtr> BinaryOperationType;
    BinaryOperationType g_binaryOperations;

    Variant binaryOperation(eOperationType op, const Variant& lhs, const Variant& rhs)
    {
        BinaryOperationType::iterator iter = g_binaryOperations.find(OperationKey(op, lhs.getType(), rhs.getType()));

        if(iter != g_binaryOperations.end()) {
            return iter->second->execute(lhs, rhs);
        }
        throw std::runtime_error(
        "cannot execute binary operation " + operationTypeToString(op) + " on types " + typeToString(lhs.getType()) + " and "
        + typeToString(rhs.getType()));
    }

    eType inferTypeOfBinaryOperation(eOperationType op, eType lhs, eType rhs)
    {
        BinaryOperationType::iterator iter = g_binaryOperations.find(OperationKey(op, lhs, rhs));

        if(iter != g_binaryOperations.end()) {
            return iter->second->retType();
        }
        throw std::runtime_error(
        "cannot infer type of binary operation " + operationTypeToString(op) + " on types " + typeToString(lhs) + " and " + typeToString(rhs));
    }


    struct UnaryOperation;
    typedef std::shared_ptr<const UnaryOperation> UnaryOperationPtr;

    struct UnaryOperation {
        virtual eOperationType opType() const = 0;
        virtual eType rhsType() const = 0;
        virtual eType retType() const = 0;

        Variant execute(const Variant& rhs) const
        {
            if(rhs.isNull()) {
                return Variant(retType());
            }
            return doExecute(rhs);
        }

        OperationKey key() const
        {
            return OperationKey(opType(), retType(), rhsType());
        }

        virtual ~UnaryOperation()
        {
        }

    private:
        virtual Variant doExecute(const Variant& rhs) const = 0;
    };

    template <eOperationType OP_TYPE, typename RET, typename RHS>
    struct UnaryOperationBase : public UnaryOperation {
        virtual eOperationType opType() const
        {
            return OP_TYPE;
        }

        virtual eType rhsType() const
        {
            return ctype2eType<RHS>();
        }

        virtual eType retType() const
        {
            return ctype2eType<RET>();
        }

    private:
        virtual Variant doExecute(const Variant& rhs) const
        {
            return operation(ValueGetter<RHS>::getValue(rhs));
        }

        virtual RET operation(const RHS& rhs) const = 0;
    };

    template <typename RHS>
    struct OperationNot : public UnaryOperationBase<OP_NOT, bool, RHS> {
    private:
        virtual bool operation(const RHS& rhs) const
        {
            return op_not(static_cast<bool>(rhs));
        }
    };

    template <typename RHS>
    struct OperationMinus : public UnaryOperationBase<OP_MINUS, RHS, RHS> {
    private:
        virtual RHS operation(const RHS& rhs) const
        {
            return op_minus(rhs);
        }
    };

    template <typename RHS>
    struct OperationPlus : public UnaryOperationBase<OP_PLUS, RHS, RHS> {
    private:
        virtual RHS operation(const RHS& rhs) const
        {
            return op_plus(rhs);
        }
    };

    template <typename CAST, typename RHS>
    struct OperationCast : public UnaryOperationBase<OP_CAST, CAST, RHS> {
    private:
        virtual CAST operation(const RHS& rhs) const
        {
            return _cast.operation(rhs);
        }

        Cast<CAST, RHS> _cast;
    };

    template <typename CAST>
    struct OperationNullCast : public UnaryOperationBase<OP_CAST, CAST, NoneType> {
    private:
        virtual CAST operation(const NoneType& rhs) const
        {
            CSVSQLDB_THROW(std::runtime_error, "Should have returned a null value before getting here");
        }
    };


    typedef std::map<OperationKey, UnaryOperationPtr> UnaryOperationType;
    UnaryOperationType g_unaryOperations;

    Variant unaryOperation(eOperationType op, eType retType, const Variant& rhs)
    {
        UnaryOperationType::iterator iter = g_unaryOperations.find(OperationKey(op, retType, rhs.getType()));

        if(iter != g_unaryOperations.end()) {
            return iter->second->execute(rhs);
        }
        if(op == OP_CAST) {
            throw std::runtime_error("cannot cast from type " + typeToString(rhs.getType()) + " to type " + typeToString(retType));
        } else {
            throw std::runtime_error("cannot execute unary operation " + operationTypeToString(op) + " on type "
                                     + typeToString(rhs.getType()));
        }
    }

    eType inferTypeOfUnaryOperation(eOperationType op, eType retType, eType rhs)
    {
        UnaryOperationType::iterator iter = g_unaryOperations.find(OperationKey(op, retType, rhs));

        if(iter != g_unaryOperations.end()) {
            return iter->second->retType();
        }
        if(op == OP_CAST) {
            throw std::runtime_error("cannot infer cast from type " + typeToString(rhs) + " to type " + typeToString(retType));
        } else {
            throw std::runtime_error("cannot infer type of unary operation " + operationTypeToString(op) + " on type " + typeToString(rhs));
        }
    }


    void initTypeSystem()
    {
        typedef OperationAdd<int64_t, int64_t, int64_t, int64_t> op_add_ii;
        typedef OperationAdd<double, double, int64_t, double> op_add_if;
        typedef OperationAdd<double, double, double, int64_t> op_add_fi;
        typedef OperationAdd<double, double, double, double> op_add_ff;

        typedef OperationSub<int64_t, int64_t, int64_t, int64_t> op_sub_ii;
        typedef OperationSub<double, double, int64_t, double> op_sub_if;
        typedef OperationSub<double, double, double, int64_t> op_sub_fi;
        typedef OperationSub<double, double, double, double> op_sub_ff;
        typedef OperationSub<int64_t, csvsqldb::Date, csvsqldb::Date, csvsqldb::Date> op_sub_dd;
        typedef OperationSub<int64_t, csvsqldb::Time, csvsqldb::Time, csvsqldb::Time> op_sub_tt;
        typedef OperationSub<int64_t, csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp> op_sub_zz;

        typedef OperationMul<int64_t, int64_t, int64_t, int64_t> op_mul_ii;
        typedef OperationMul<double, double, int64_t, double> op_mul_if;
        typedef OperationMul<double, double, double, int64_t> op_mul_fi;
        typedef OperationMul<double, double, double, double> op_mul_ff;

        typedef OperationDiv<int64_t, int64_t, int64_t, int64_t> op_div_ii;
        typedef OperationDiv<double, double, int64_t, double> op_div_if;
        typedef OperationDiv<double, double, double, int64_t> op_div_fi;
        typedef OperationDiv<double, double, double, double> op_div_ff;

        typedef OperationMod<int64_t, int64_t, int64_t, int64_t> op_mod_ii;
        typedef OperationMod<double, double, int64_t, double> op_mod_if;
        typedef OperationMod<double, double, double, int64_t> op_mod_fi;
        typedef OperationMod<double, double, double, double> op_mod_ff;

        typedef OperationGTCast<int64_t, int64_t, int64_t> op_gt_ii;
        typedef OperationGTCast<double, int64_t, double> op_gt_if;
        typedef OperationGTCast<double, double, int64_t> op_gt_fi;
        typedef OperationGTCast<double, double, double> op_gt_ff;
        typedef OperationGTCast<StringType, StringType, StringType> op_gt_ss;
        typedef OperationGT<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date> op_gt_dd;
        typedef OperationGT<csvsqldb::Date, csvsqldb::Date, StringType> op_gt_ds;
        typedef OperationGT<csvsqldb::Date, StringType, csvsqldb::Date> op_gt_sd;
        typedef OperationGT<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time> op_gt_tt;
        typedef OperationGT<csvsqldb::Time, csvsqldb::Time, StringType> op_gt_ts;
        typedef OperationGT<csvsqldb::Time, StringType, csvsqldb::Time> op_gt_st;
        typedef OperationGT<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp> op_gt_zz;
        typedef OperationGT<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType> op_gt_zs;
        typedef OperationGT<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp> op_gt_sz;

        typedef OperationGECast<int64_t, int64_t, int64_t> op_ge_ii;
        typedef OperationGECast<double, int64_t, double> op_ge_if;
        typedef OperationGECast<double, double, int64_t> op_ge_fi;
        typedef OperationGECast<double, double, double> op_ge_ff;
        typedef OperationGECast<StringType, StringType, StringType> op_ge_ss;
        typedef OperationGE<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date> op_ge_dd;
        typedef OperationGE<csvsqldb::Date, csvsqldb::Date, StringType> op_ge_ds;
        typedef OperationGE<csvsqldb::Date, StringType, csvsqldb::Date> op_ge_sd;
        typedef OperationGE<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time> op_ge_tt;
        typedef OperationGE<csvsqldb::Time, csvsqldb::Time, StringType> op_ge_ts;
        typedef OperationGE<csvsqldb::Time, StringType, csvsqldb::Time> op_ge_st;
        typedef OperationGE<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp> op_ge_zz;
        typedef OperationGE<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType> op_ge_zs;
        typedef OperationGE<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp> op_ge_sz;

        typedef OperationLTCast<int64_t, int64_t, int64_t> op_lt_ii;
        typedef OperationLTCast<double, int64_t, double> op_lt_if;
        typedef OperationLTCast<double, double, int64_t> op_lt_fi;
        typedef OperationLTCast<double, double, double> op_lt_ff;
        typedef OperationLTCast<StringType, StringType, StringType> op_lt_ss;
        typedef OperationLT<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date> op_lt_dd;
        typedef OperationLT<csvsqldb::Date, csvsqldb::Date, StringType> op_lt_ds;
        typedef OperationLT<csvsqldb::Date, StringType, csvsqldb::Date> op_lt_sd;
        typedef OperationLT<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time> op_lt_tt;
        typedef OperationLT<csvsqldb::Time, csvsqldb::Time, StringType> op_lt_ts;
        typedef OperationLT<csvsqldb::Time, StringType, csvsqldb::Time> op_lt_st;
        typedef OperationLT<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp> op_lt_zz;
        typedef OperationLT<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType> op_lt_zs;
        typedef OperationLT<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp> op_lt_sz;

        typedef OperationLECast<int64_t, int64_t, int64_t> op_le_ii;
        typedef OperationLECast<double, int64_t, double> op_le_if;
        typedef OperationLECast<double, double, int64_t> op_le_fi;
        typedef OperationLECast<double, double, double> op_le_ff;
        typedef OperationLECast<StringType, StringType, StringType> op_le_ss;
        typedef OperationLE<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date> op_le_dd;
        typedef OperationLE<csvsqldb::Date, csvsqldb::Date, StringType> op_le_ds;
        typedef OperationLE<csvsqldb::Date, StringType, csvsqldb::Date> op_le_sd;
        typedef OperationLE<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time> op_le_tt;
        typedef OperationLE<csvsqldb::Time, csvsqldb::Time, StringType> op_le_ts;
        typedef OperationLE<csvsqldb::Time, StringType, csvsqldb::Time> op_le_st;
        typedef OperationLE<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp> op_le_zz;
        typedef OperationLE<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType> op_le_zs;
        typedef OperationLE<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp> op_le_sz;

        typedef OperationEQCast<bool, bool, bool> op_eq_bb;
        typedef OperationEQCast<bool, bool, int64_t> op_eq_bi;
        typedef OperationEQCast<bool, int64_t, bool> op_eq_ib;
        typedef OperationEQCast<bool, bool, double> op_eq_bf;
        typedef OperationEQCast<bool, double, bool> op_eq_fb;
        typedef OperationEQCast<int64_t, int64_t, int64_t> op_eq_ii;
        typedef OperationEQCast<double, int64_t, double> op_eq_if;
        typedef OperationEQCast<double, double, int64_t> op_eq_fi;
        typedef OperationEQCast<double, double, double> op_eq_ff;
        typedef OperationEQCast<StringType, StringType, StringType> op_eq_ss;
        typedef OperationEQ<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date> op_eq_dd;
        typedef OperationEQ<csvsqldb::Date, csvsqldb::Date, StringType> op_eq_ds;
        typedef OperationEQ<csvsqldb::Date, StringType, csvsqldb::Date> op_eq_sd;
        typedef OperationEQ<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time> op_eq_tt;
        typedef OperationEQ<csvsqldb::Time, csvsqldb::Time, StringType> op_eq_ts;
        typedef OperationEQ<csvsqldb::Time, StringType, csvsqldb::Time> op_eq_st;
        typedef OperationEQ<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp> op_eq_zz;
        typedef OperationEQ<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType> op_eq_zs;
        typedef OperationEQ<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp> op_eq_sz;

        typedef OperationNEQCast<bool, bool, bool> op_neq_bb;
        typedef OperationNEQCast<bool, bool, int64_t> op_neq_bi;
        typedef OperationNEQCast<bool, int64_t, bool> op_neq_ib;
        typedef OperationNEQCast<bool, bool, double> op_neq_bf;
        typedef OperationNEQCast<bool, double, bool> op_neq_fb;
        typedef OperationNEQCast<int64_t, int64_t, int64_t> op_neq_ii;
        typedef OperationNEQCast<double, int64_t, double> op_neq_if;
        typedef OperationNEQCast<double, double, int64_t> op_neq_fi;
        typedef OperationNEQCast<double, double, double> op_neq_ff;
        typedef OperationNEQCast<StringType, StringType, StringType> op_neq_ss;
        typedef OperationNEQ<csvsqldb::Date, csvsqldb::Date, csvsqldb::Date> op_neq_dd;
        typedef OperationNEQ<csvsqldb::Date, csvsqldb::Date, StringType> op_neq_ds;
        typedef OperationNEQ<csvsqldb::Date, StringType, csvsqldb::Date> op_neq_sd;
        typedef OperationNEQ<csvsqldb::Time, csvsqldb::Time, csvsqldb::Time> op_neq_tt;
        typedef OperationNEQ<csvsqldb::Time, csvsqldb::Time, StringType> op_neq_ts;
        typedef OperationNEQ<csvsqldb::Time, StringType, csvsqldb::Time> op_neq_st;
        typedef OperationNEQ<csvsqldb::Timestamp, csvsqldb::Timestamp, csvsqldb::Timestamp> op_neq_zz;
        typedef OperationNEQ<csvsqldb::Timestamp, csvsqldb::Timestamp, StringType> op_neq_zs;
        typedef OperationNEQ<csvsqldb::Timestamp, StringType, csvsqldb::Timestamp> op_neq_sz;

        typedef OperationIs<bool, bool> op_is_bb;
        typedef OperationIs<int64_t, bool> op_is_ib;
        typedef OperationIs<double, bool> op_is_fb;
        typedef OperationIs<StringType, bool> op_is_sb;
        typedef OperationIs<csvsqldb::Date, bool> op_is_db;
        typedef OperationIs<csvsqldb::Time, bool> op_is_tb;
        typedef OperationIs<csvsqldb::Timestamp, bool> op_is_zb;

        typedef OperationIsNot<bool, bool> op_isnot_bb;
        typedef OperationIsNot<int64_t, bool> op_isnot_ib;
        typedef OperationIsNot<double, bool> op_isnot_fb;
        typedef OperationIsNot<StringType, bool> op_isnot_sb;
        typedef OperationIsNot<csvsqldb::Date, bool> op_isnot_db;
        typedef OperationIsNot<csvsqldb::Time, bool> op_isnot_tb;
        typedef OperationIsNot<csvsqldb::Timestamp, bool> op_isnot_zb;

        typedef OperationAnd<bool, bool> op_and_bb;
        typedef OperationAnd<bool, int64_t> op_and_bi;
        typedef OperationAnd<int64_t, bool> op_and_ib;
        typedef OperationAnd<double, bool> op_and_fb;
        typedef OperationAnd<bool, double> op_and_bf;
        typedef OperationAnd<int64_t, int64_t> op_and_ii;
        typedef OperationAnd<int64_t, double> op_and_if;
        typedef OperationAnd<double, int64_t> op_and_fi;
        typedef OperationAnd<double, double> op_and_ff;
        typedef OperationAnd<bool, StringType> op_and_bs;
        typedef OperationAnd<StringType, bool> op_and_sb;
        typedef OperationAnd<int64_t, StringType> op_and_is;
        typedef OperationAnd<StringType, int64_t> op_and_si;
        typedef OperationAnd<double, StringType> op_and_fs;
        typedef OperationAnd<StringType, double> op_and_sf;
        typedef OperationAnd<StringType, StringType> op_and_ss;
        typedef OperationAnd<csvsqldb::Date, bool> op_and_db;
        typedef OperationAnd<bool, csvsqldb::Date> op_and_bd;
        typedef OperationAnd<csvsqldb::Date, csvsqldb::Date> op_and_dd;
        typedef OperationAnd<csvsqldb::Time, bool> op_and_tb;
        typedef OperationAnd<bool, csvsqldb::Time> op_and_bt;
        typedef OperationAnd<csvsqldb::Time, csvsqldb::Time> op_and_tt;

        typedef OperationOr<bool, bool> op_or_bb;
        typedef OperationOr<bool, int64_t> op_or_bi;
        typedef OperationOr<int64_t, bool> op_or_ib;
        typedef OperationOr<double, bool> op_or_fb;
        typedef OperationOr<bool, double> op_or_bf;
        typedef OperationOr<int64_t, int64_t> op_or_ii;
        typedef OperationOr<int64_t, double> op_or_if;
        typedef OperationOr<double, int64_t> op_or_fi;
        typedef OperationOr<double, double> op_or_ff;
        typedef OperationOr<bool, StringType> op_or_bs;
        typedef OperationOr<StringType, bool> op_or_sb;
        typedef OperationOr<int64_t, StringType> op_or_is;
        typedef OperationOr<StringType, int64_t> op_or_si;
        typedef OperationOr<double, StringType> op_or_fs;
        typedef OperationOr<StringType, double> op_or_sf;
        typedef OperationOr<StringType, StringType> op_or_ss;

        typedef OperationConcat<StringType, StringType, StringType, StringType> op_concat_ss;
        typedef OperationConcat<StringType, StringType, StringType, int64_t> op_concat_sl;
        typedef OperationConcat<StringType, StringType, int64_t, StringType> op_concat_ls;
        typedef OperationConcat<StringType, StringType, StringType, double> op_concat_sf;
        typedef OperationConcat<StringType, StringType, double, StringType> op_concat_fs;
        typedef OperationConcat<StringType, StringType, csvsqldb::Date, StringType> op_concat_ds;
        typedef OperationConcat<StringType, StringType, StringType, csvsqldb::Date> op_concat_sd;
        typedef OperationConcat<StringType, StringType, csvsqldb::Time, StringType> op_concat_ts;
        typedef OperationConcat<StringType, StringType, StringType, csvsqldb::Time> op_concat_st;
        typedef OperationConcat<StringType, StringType, csvsqldb::Timestamp, StringType> op_concat_zs;
        typedef OperationConcat<StringType, StringType, StringType, csvsqldb::Timestamp> op_concat_sz;

        if(g_binaryOperations.empty()) {
            BinaryOperationPtr operation;
            operation = std::make_shared<op_add_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_add_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_add_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_add_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_sub_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_sub_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_sub_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_sub_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_sub_dd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_sub_tt>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_sub_zz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_mul_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_mul_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_mul_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_mul_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_div_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_div_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_div_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_div_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_mod_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_mod_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_mod_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_mod_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_gt_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_ss>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_dd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_ds>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_sd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_tt>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_ts>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_st>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_zz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_zs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_gt_sz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_ge_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_ss>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_dd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_ds>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_sd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_tt>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_ts>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_st>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_zz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_zs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_ge_sz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_lt_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_ss>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_dd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_ds>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_sd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_tt>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_ts>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_st>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_zz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_zs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_lt_sz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_le_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_ss>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_dd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_ds>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_sd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_tt>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_ts>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_st>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_zz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_zs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_le_sz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_eq_bb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_bi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_ib>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_bf>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_fb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_ss>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_dd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_ds>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_sd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_tt>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_ts>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_st>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_zz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_zs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_eq_sz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_neq_bb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_bi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_ib>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_bf>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_fb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_ss>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_dd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_ds>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_sd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_tt>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_ts>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_st>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_zz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_zs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_neq_sz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_is_bb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_is_ib>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_is_fb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_is_sb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_is_db>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_is_tb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_is_zb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_isnot_bb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_isnot_ib>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_isnot_fb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_isnot_sb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_isnot_db>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_isnot_tb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_isnot_zb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_and_bb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_bi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_ib>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_fb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_bf>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_bs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_sb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_is>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_si>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_fs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_sf>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_ss>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_bd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_db>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_dd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_bt>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_tb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_and_tt>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_or_bb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_bi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_ib>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_fb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_bf>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_ii>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_if>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_fi>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_ff>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_bs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_sb>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_is>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_si>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_fs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_sf>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_or_ss>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_concat_ss>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_sl>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_ls>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_sf>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_fs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_ds>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_sd>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_ts>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_st>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_zs>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_concat_sz>();
            g_binaryOperations.insert(std::make_pair(operation->key(), operation));
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        typedef OperationNot<bool> op_not_b;
        typedef OperationNot<int64_t> op_not_i;
        typedef OperationNot<double> op_not_f;

        typedef OperationMinus<int64_t> op_minus_i;
        typedef OperationMinus<double> op_minus_f;

        typedef OperationPlus<int64_t> op_plus_i;
        typedef OperationPlus<double> op_plus_f;

        typedef OperationCast<int64_t, int64_t> op_cast_ii;
        typedef OperationCast<int64_t, double> op_cast_fi;
        typedef OperationCast<double, double> op_cast_ff;
        typedef OperationCast<double, int64_t> op_cast_if;
        typedef OperationCast<int64_t, bool> op_cast_bi;
        typedef OperationCast<bool, bool> op_cast_bb;
        typedef OperationCast<bool, int64_t> op_cast_ib;
        // typedef OperationCast<bool, double> op_cast_fb;
        typedef OperationCast<StringType, StringType> op_cast_ss;
        typedef OperationCast<bool, StringType> op_cast_sb;
        typedef OperationCast<double, StringType> op_cast_sf;
        typedef OperationCast<int64_t, StringType> op_cast_si;
        typedef OperationCast<csvsqldb::Date, StringType> op_cast_sd;
        typedef OperationCast<csvsqldb::Time, StringType> op_cast_st;
        typedef OperationCast<csvsqldb::Timestamp, StringType> op_cast_sz;
        typedef OperationCast<csvsqldb::Timestamp, csvsqldb::Date> op_cast_dz;
        typedef OperationCast<csvsqldb::Timestamp, csvsqldb::Time> op_cast_tz;
        typedef OperationNullCast<bool> op_null_cast_nb;
        typedef OperationNullCast<int64_t> op_null_cast_ni;
        typedef OperationNullCast<double> op_null_cast_nf;
        typedef OperationNullCast<StringType> op_null_cast_ns;
        typedef OperationNullCast<csvsqldb::Date> op_null_cast_nd;
        typedef OperationNullCast<csvsqldb::Time> op_null_cast_nt;
        typedef OperationNullCast<csvsqldb::Timestamp> op_null_cast_nz;

        if(g_unaryOperations.empty()) {
            UnaryOperationPtr operation;
            operation = std::make_shared<op_not_b>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_not_i>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_not_f>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_minus_i>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_minus_f>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_plus_i>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_plus_f>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));

            operation = std::make_shared<op_cast_ii>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_fi>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_ff>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_if>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_bb>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_bi>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_ib>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_ss>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_sb>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_sf>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_si>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_sd>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_st>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_sz>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_dz>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_cast_tz>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_null_cast_nb>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_null_cast_ni>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_null_cast_nf>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_null_cast_ns>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_null_cast_nd>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_null_cast_nt>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
            operation = std::make_shared<op_null_cast_nz>();
            g_unaryOperations.insert(std::make_pair(operation->key(), operation));
        }
    }
}
