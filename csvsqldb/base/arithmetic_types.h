
#include <mpfcore/inc.h>
#include <mpfcore/types.h>

#include <iostream>


/******************************************************************************
CLASS arithmetic_operator_helper
    for classes that represent numeric types (types that are like numbers), and
    which implement a basic set of operators
        commutative + and *, non-commutative - and /, and relations < and ==
    and implement a basic set of methods
        operator+=, -=, *=, /=, ==, and <
    this class adds additional operator methods
        operator+, -, *, /, unary-, !=, <=, >, >=
    with the usual meanings
******************************************************************************/
template<typename T>
class arithmetic_operator_helper
{
public:
  //  arithmetical operators
  friend T operator-(T const& lhs)
  {
    T res(-1);
    res *= lhs;
    return res;
  }
  friend T operator+(T const& lhs, T const& rhs)
  {
    T res(lhs);
    res += rhs;
    return res;
  }
  friend T operator-(T const& lhs, T const& rhs)
  {
    T res(lhs);
    res -= rhs;
    return res;
  }
  friend T operator*(T const& lhs, T const& rhs)
  {
    T res(lhs);
    res *= rhs;
    return res;
  }
  friend T operator/(T const& lhs, T const& rhs)
  {
    T res(lhs);
    res /= rhs;
    return res;
  }

  template<typename R>
  friend T operator+(R lhs, T const& rhs)
  {
    T res(lhs);
    res += rhs;
    return res;
  }

  template<typename R>
  friend T operator-(R lhs, T const& rhs)
  {
    T res(lhs);
    res -= rhs;
    return res;
  }

  template<typename R>
  friend T operator*(R lhs, T const& rhs)
  {
    T res(lhs);
    res *= rhs;
    return res;
  }

  template<typename R>
  friend T operator/(R lhs, T const& rhs)
  {
    T res(lhs);
    res /= rhs;
    return res;
  }

  //  comparison operators
  friend bool operator!=(T const& lhs, T const& rhs)
  {
    return !(lhs == rhs);
  }
  friend bool operator>=(T const& lhs, T const& rhs)
  {
    return !(lhs < rhs);
  }
  friend bool operator<=(T const& lhs, T const& rhs)
  {
    return !(rhs < lhs);
  }
  friend bool operator>(T const& lhs, T const& rhs)
  {
    return (rhs < lhs);
  }

  //  more helpers

};  // end class arithmetic_operator_helper

template<typename T>
T absT(T v)
{
  return v < 0 ? -v : v;
}

template<typename T>
int signT(T v)
{
  return v < 0 ? -1 : 1;
}


/******************************************************************************
CLASS exp_10 -- template helper for computing powers of 10
ARGUMENTS
    N   --  exponent
IMPLEMENTATION
    exp_10 computes powers of 10 recursively using a specialization for 0 to
    end recursion
******************************************************************************/
template<int N, typename T = int>
struct exp_10 {
  static const T value = exp_10<N - 1, T>::value * 10;
};
template<>
struct exp_10<0, int> {
  static const int value = 1;
};
template<>
struct exp_10<0, mpf::Int64> {
  static const mpf::Int64 value = 1;
};

/******************************************************************************
CLASS fixed_decimal --  exact decimal arithmetic type
    This fixed decimal representation provides a subset of rational numbers
    evenly spaced between a maximum and minimum value which facilitates exact
    decimal computations.
ARGUMENTS
    N   --  number of decimal places
    T   --  underlying integral representation type
TEMPORARY Things to add to fixed_decimal
        o   accept other sizes of fixed decimal arguments
******************************************************************************/
template<int N = 2, typename T = int>
class fixed_decimal : public arithmetic_operator_helper<fixed_decimal<N, T>>
{
public:
  typedef T rep_t;

  static const T base_ = 10;
  static const T scale_ = exp_10<N, T>::value;
  static const T fraction_digits_ = N;

  explicit fixed_decimal(int m = 0, int n = 0)
  : rep_((static_cast<T>(m) * scale_ + (n % scale_) * signT(m)))
  {
  }
  explicit fixed_decimal(unsigned int u)
  : rep_((u * scale_))
  {
  }
  explicit fixed_decimal(unsigned long ul)
  : rep_((ul * scale_))
  {
  }
  explicit fixed_decimal(double f)
  : rep_((T)(f * scale_ + 0.5))
  {
  }

  //  arithmetical operators
  fixed_decimal& operator-()
  {
    fixed_decimal tmp(-1);
    tmp *= this;
    return tmp;
  }
  fixed_decimal& operator+=(fixed_decimal const& that)
  {
    this->rep_ += that.rep_;
    return *this;
  }
  fixed_decimal& operator-=(fixed_decimal const& that)
  {
    this->rep_ -= that.rep_;
    return *this;
  }
  fixed_decimal& operator*=(fixed_decimal const& that);
  fixed_decimal& operator/=(fixed_decimal const& that);

  //  comparison operators
  bool operator==(fixed_decimal const& rhs) const
  {
    return this->rep_ == rhs.rep_;
  }
  bool operator<(fixed_decimal const& rhs) const
  {
    return this->rep_ < rhs.rep_;
  }

  //  conversions
  rep_t rep() const
  {
    return rep_;
  }
  rep_t characteristic() const
  {
    return rep_ / scale_;
  }
  rep_t fraction() const
  {
    return rep_ % scale_;
  }
  rep_t scale() const
  {
    return scale_;
  }
  fixed_decimal& operator=(int i)
  {
    rep_ = i * scale_;
    return *this;
  }
  fixed_decimal& operator=(double d)
  {
    rep_ = (T)(d * scale_ + 0.5);
    return *this;
  }

private:
  rep_t rep_;
};  // end template class fixed_decimal

template<int N, typename T>
inline fixed_decimal<N, T>& fixed_decimal<N, T>::operator*=(fixed_decimal<N, T> const& that)
{
  int s = signT(this->rep_) * signT(that.rep_);
  T c1 = absT(characteristic()), f1 = absT(fraction());
  T c2 = absT(that.characteristic()), f2 = absT(that.fraction());
  T t = (f1 * f2 + scale_ / 2) / scale_;
  t = (c1 * f2 + f1 * c2 + t);
  t = c1 * c2 * scale_ + t;
  this->rep_ = t * s;
  return *this;
}

template<int N, typename T>
inline fixed_decimal<N, T>& fixed_decimal<N, T>::operator/=(fixed_decimal<N, T> const& that)
{
  int s = signT(this->rep_) * signT(that.rep_);
  T c1 = absT(characteristic()), f1 = absT(fraction());
  T v2 = absT(that.rep_);
  T t = c1 * scale_ / v2;
  T r = c1 * scale_ % v2;
  t = t * scale_ + (r + f1) * scale_ / v2;
  this->rep_ = t * s;
  return *this;
}

template<int N, typename T>
std::ostream& operator<<(std::ostream& o, fixed_decimal<N, T> n)
{
  o << n.characteristic() << '.';
  T f = absT(n.fraction());
  for (int i = n.scale() / 10; i > 0; f %= i, i /= 10)
    o << (char)('0' + f / i);
  return o;
}
