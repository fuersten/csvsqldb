/***************************************************************************
 *                                                                         *
 *   Copyright (c) 2004-2007 by Athelas GmbH. All Rights Reserved.         *
 *                                                                         *
 *   The contents of this file are subject to the Mozilla Public License   *
 *   Version 1.1 (the "License"); you may not use this file except in      *
 *   compliance with the License. You may obtain a copy of the License at  *
 *   http://www.mozilla.org/MPL/                                           *
 *                                                                         *
 *   Software distributed under the License is distributed on an "AS IS"   *
 *   basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See   *
 *   the License for the specificlanguage governing rights and             *
 *   limitations under the License.                                        *
 *                                                                         *
 *   The Original Code is mpf.                                             *
 *                                                                         *
 *   The Initial Developer of the Original Code is Lars-Christian          *
 *   Fuerstenberg, with contributions from Steffen Schuemann. Portions     *
 *   created by Lars-Christian Fuerstenberg are Copyright (c) 2002-2003    *
 *   All Rights Reserved.                                                  *
 *                                                                         *
 ***************************************************************************/
 
#ifndef MPF_DECIMAL_H
#define MPF_DECIMAL_H

#include <mpfcore/inc.h>

#include <mpfcore/arithmetic_types.h>

#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <cmath>
#include <climits>


namespace mpf
{

/** The Decimal can be used for financial calculations with the given precision and scale.
 */
template<int P, int S>
class Decimal
{
public:
    BOOST_STATIC_ASSERT( P <= std::numeric_limits<mpf::Int64>::digits10 );
    BOOST_STATIC_ASSERT( S <= P );
    Decimal() {}
    Decimal(const Decimal& ref) : _val( ref._val ) {}
    explicit Decimal(int m, int n=0) : _val( m, n ) {}
    explicit Decimal(const double& val) : _val( val ) {}
    explicit Decimal(const mpf::Int64& val) : _val( val ) {}

    ~Decimal() {}

    Decimal& operator=(const Decimal& ref)
    {
        _val = ref._val;
        return *this;
    }

    Decimal operator-() const
    {
        return Decimal<P,S>( -_val );
    }

    Decimal& operator+=(const Decimal& val)
    {
        _val += val._val;
        return *this;
    }

    Decimal operator+(const Decimal& rhs) const
    {
        return _val + rhs._val;
    }

    Decimal& operator-=(const Decimal& val)
    {
        _val -= val._val;
        return *this;
    }

    Decimal operator-(const Decimal& rhs) const
    {
        return _val - rhs._val;
    }

	Decimal& operator*=(const Decimal& val)
    {
        _val *= val._val;
        return *this;
    }

    Decimal operator*(const Decimal& rhs) const
    {
        return _val * rhs._val;
    }

	Decimal& operator/=(const Decimal& val)
    {
        _val /= val._val;
        return *this;
    }

    Decimal operator/(const Decimal& rhs) const
    {
        return _val / rhs._val;
    }

	bool operator==(const Decimal& rhs) const
    {
        return _val == rhs._val;
    }

	bool operator!=(const Decimal& rhs) const
    {
        return _val != rhs._val;
    }

    bool operator< (const Decimal& rhs) const
    {
        return _val < rhs._val;
    }

    bool operator> (const Decimal& rhs) const
    {
        return _val > rhs._val;
    }

    bool operator<= (const Decimal& rhs) const
    {
        return _val <= rhs._val;
    }

    bool operator>= (const Decimal& rhs) const
    {
        return _val >= rhs._val;
    }

    double asDouble() const { return double(_val.rep())/_val.scale(); }

    int precision() const { return P; }
    int scale() const { return S; }

    mpf::Int64 characteristic() const { return _val.characteristic(); }
    mpf::Int64 fraction() const
    {
        mpf::Int64 f = _val.fraction();
        return f<0 ? -f : f; 
    }

    std::string format() const
    {
        static std::string form = std::string("%d.%0") + boost::lexical_cast<std::string>( S ) + "d";
        return (boost::format(form) % characteristic() % fraction()).str();
    }

private:
    Decimal(fixed_decimal<S,mpf::Int64> internal) : _val( internal ) {}
    fixed_decimal<S,mpf::Int64> _val;
};


}


#endif // MPF_DECIMAL_H
