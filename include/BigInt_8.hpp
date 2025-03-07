#ifndef SCH_INCLUDE_BIGINT_8_HPP_
#define SCH_INCLUDE_BIGINT_8_HPP_

#include "sign.h"
#include <algorithm>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace sch {

class BigInt_8 {
 public:
  BigInt_8() = default;
  explicit BigInt_8(const std::string &str);
  ~BigInt_8() = default;

  BigInt_8(const BigInt_8 &) = default;
  BigInt_8(BigInt_8 &&) = default;
  BigInt_8 &operator=(const BigInt_8 &) = default;
  BigInt_8 &operator=(BigInt_8 &&) = default;

  bool operator==(const BigInt_8 &rhs) const;
  bool operator!=(const BigInt_8 &rhs) const;
  bool operator<(const BigInt_8 &rhs) const;
  bool operator>(const BigInt_8 &rhs) const;
  bool operator<=(const BigInt_8 &rhs) const;
  bool operator>=(const BigInt_8 &rhs) const;

  BigInt_8 operator+(const BigInt_8 &rhs) const;
  BigInt_8 operator-(const BigInt_8 &rhs) const;
  BigInt_8 operator*(const BigInt_8 &rhs) const;
  BigInt_8 operator/(const BigInt_8 &rhs) const;
  BigInt_8 operator%(const BigInt_8 &rhs) const;
  BigInt_8 &operator+=(const BigInt_8 &rhs);
  BigInt_8 &operator-=(const BigInt_8 &rhs);
  BigInt_8 &operator*=(const BigInt_8 &rhs);
  BigInt_8 &operator/=(const BigInt_8 &rhs);
  BigInt_8 &operator%=(const BigInt_8 &rhs);
  BigInt_8 &operator<<=(const BigInt_8 &rhs);
  BigInt_8 &operator>>=(const BigInt_8 &rhs);

  BigInt_8 &operator-();
  BigInt_8 operator-() const;
  BigInt_8 &operator++();
  BigInt_8 &operator--();

  friend std::ostream &operator<<(std::ostream &os, const BigInt_8 &b);

  void normalize();

 private:
  // constants
  static constexpr uint64_t BASE = 10; ///< data is stored in base 10

  // private variables
  sign _sign = sign::positive;  ///< sign of the number
  std::vector<uint8_t> _data{}; ///< @note little endian order

  // Addition operator helpers ---------------------------------
  static void add(size_t &it_lhs, const BigInt_8 &lhs, size_t &it_rhs,
                  const BigInt_8 &rhs, bool &carry, BigInt_8 &sum);
  static void a_carryDown(size_t &it, const BigInt_8 &bint_8, bool &carry,
                          BigInt_8 &sum);

  // Subtraction operator helpers ------------------------------
  static void subtract(size_t &it_lhs, BigInt_8 &lhs, size_t &it_rhs,
                       const BigInt_8 &rhs, BigInt_8 &difference);
  static void s_carryDown(size_t &it, const BigInt_8 &bint_8,
                          BigInt_8 &difference);
};

//------------------------------------------------------------------------------
// Constructors

// String constructor
inline BigInt_8::BigInt_8(const std::string &str) {
  int offset = 0;           // to ignore negative sign, if it exists
  if (str.front() == '-') { // check for sign
    offset = 1;
    _sign = sign::negative;
  }
  // ensure there are no other non-numeric characters
  if (!std::all_of(str.begin() + offset, str.end(), isdigit)) {
    throw std::invalid_argument(
        "BigInt_8::BigUInt_8(): string contains non-numeric characters");
  }
  _data = std::vector<uint8_t>(str.rbegin(), str.rend() - offset);
  for (auto &digit : _data) {
    digit -= '0';
  }
  // the BigInt objects can sometimes be initialized with leading zeros
  // having leading zeros will cause incorrect comparisons between BigInts
  // best to get rid of them now
  this->normalize();
}

//------------------------------------------------------------------------------
// Binary comparison operators

inline bool BigInt_8::operator==(const BigInt_8 &rhs) const {
  return (_data == rhs._data && _sign == rhs._sign);
}

inline bool BigInt_8::operator!=(const BigInt_8 &rhs) const {
  return !(*this == rhs);
}

inline bool BigInt_8::operator<(const BigInt_8 &rhs) const {
  // opposite sign considerations ---------------------
  if (_sign == sign::negative && rhs._sign == sign::positive) {
    return true;
  }
  if (_sign == sign::positive && rhs._sign == sign::negative) {
    return false;
  }
  // same sign considerations -------------------------
  // different lengths ------------------
  // if positive, shorter is smaller
  // if negative, longer is smaller
  if (_data.size() != rhs._data.size()) {
    return (_sign == sign::positive ? _data.size() < rhs._data.size()
                                    : rhs._data.size() < _data.size());
  }
  // same length ------------------------
  // if positive, lexicographically "smaller" is smaller
  // if negative, lexicographically "larger" is smaller
  return (
      _sign == sign::positive
          ? std::lexicographical_compare(_data.rbegin(), _data.rend(),
                                         rhs._data.rbegin(), rhs._data.rend())
          : std::lexicographical_compare(rhs._data.rbegin(), rhs._data.rend(),
                                         _data.rbegin(), _data.rend()));
  // --------------------------------------------------
}

inline bool BigInt_8::operator>(const BigInt_8 &rhs) const {
  return rhs < *this;
}

inline bool BigInt_8::operator<=(const BigInt_8 &rhs) const {
  return !(*this > rhs);
}

inline bool BigInt_8::operator>=(const BigInt_8 &rhs) const {
  return !(*this < rhs);
}

// ADDITION OPERATOR -----------------------------------------------------------

inline BigInt_8 BigInt_8::operator+(const BigInt_8 &rhs) const {
  // todo optimizations for adding to 0 or 1 and so on
  // Initially, addition and subtraction were implemented assuming two
  // non-negative integers. Sign handling was introduced afterward; the most
  // straightforward approach to implementation was(is?,were?) the
  // conditional statements below. This allows us to reuse the subtraction
  // (addition) logic.
  if (_sign != rhs._sign) {
    if (_sign == sign::negative) {
      return rhs - -*this;
    }
    if (rhs._sign == sign::negative) {
      return *this - -rhs;
    }
  }
  if (_sign == sign::negative && rhs._sign == sign::negative) {
    return -(-*this + -rhs);
  }

  BigInt_8 sum;
  bool carry = false;
  size_t it_lhs{0}; // iterate through the digits of the lhs
  size_t it_rhs{0}; // iterate through the digits of the rhs

  sum._data.reserve(_data.size() > rhs._data.size() ? _data.size()
                                                    : rhs._data.size());

  add(it_lhs, *this, it_rhs, rhs, carry, sum);
  a_carryDown(it_lhs, *this, carry, sum);
  a_carryDown(it_rhs, rhs, carry, sum);

  if (carry) { // final carry
    sum._data.push_back(1);
  }
  sum.normalize();
  return sum;
}

/**
 * @brief School-book addition.
 * @param[in,out] it_lhs iterate through the lhs digits
 * @param[in] lhs the left-hand-side addend
 * @param[in,out] it_rhs iterate through the rhs digits
 * @param[in] rhs the right-hand-side addend
 * @param[in,out] carry carry 1?
 * @param[in,out] sum the sum
 */
inline void BigInt_8::add(size_t &it_lhs, const BigInt_8 &lhs, size_t &it_rhs,
                          const BigInt_8 &rhs, bool &carry, BigInt_8 &sum) {
  while (it_lhs < lhs._data.size() && it_rhs < rhs._data.size()) {
    sum._data.push_back(lhs._data[it_lhs] + rhs._data[it_rhs] +
                        (carry ? 1 : 0));
    if (sum._data.back() > BASE - 1) {
      carry = true;
      sum._data.back() -= BASE;
    } else {
      carry = false;
    }
    ++it_lhs;
    ++it_rhs;
  }
}

/**
 * @brief School-book addition -- performs all the x + 0 columns
 * @param[in,out] it iterate through the digits
 * @param bint_8 the number we are iterating through
 * @param[in,out] carry carry 1?
 * @param[in,out] sum the sum
 */
inline void BigInt_8::a_carryDown(size_t &it, const BigInt_8 &bint_8,
                                  bool &carry, BigInt_8 &sum) {
  while (it < bint_8._data.size()) {
    sum._data.push_back(bint_8._data[it] + (carry ? 1 : 0));
    if (sum._data.back() > BASE - 1) {
      carry = true;
      sum._data.back() -= BASE;
    } else {
      carry = false;
    }
    ++it;
  }
}

// SUBTRACTION OPERATOR --------------------------------------------------------

// is there a way to work around using copies to maintain constness?
inline BigInt_8 BigInt_8::operator-(const BigInt_8 &rhs) const {
  // todo optimizations for subtracting to and from 0 or 1 and so on
  // Initially, addition and subtraction were implemented assuming two
  // non-negative integers. Sign handling was introduced afterward; the most
  // straightforward approach to implementation was(is?,were?) the conditional
  // statements below. This allows us to reuse the subtraction (addition) logic.
  if (*this == rhs) {
    return BigInt_8{"0"};
  }
  if (_sign != rhs._sign) {
    if (_sign == sign::negative) {
      return -(-(*this) + rhs);
    }
    if (rhs._sign == sign::negative) {
      return *this + (-(rhs));
    }
  }
  if (_sign == sign::negative && rhs._sign == sign::negative) {
    return -(rhs) - (-(*this));
  }

  BigInt_8 difference{};
  BigInt_8 _lhs{*this}; // mutable copy
  BigInt_8 _rhs{rhs};   // mutable copy
  size_t it_lhs{0};     // iterate through the digits of the lhs
  size_t it_rhs{0};     // iterate through the digits of the rhs

  difference._data.reserve(_data.size() > rhs._data.size() ? _data.size()
                                                           : rhs._data.size());

  if (_rhs > _lhs) {
    difference._sign = sign::negative; // otherwise dif sign is pos. by default
  }
  if (difference._sign == sign::positive) { // subtract rhs from lhs
    subtract(it_lhs, _lhs, it_rhs, _rhs, difference);
  } else { // subtract lhs from rhs
    subtract(it_rhs, _rhs, it_lhs, _lhs, difference);
  }
  s_carryDown(it_lhs, _lhs, difference);
  s_carryDown(it_rhs, _rhs, difference);

  difference.normalize();
  return difference;
}

/**
 * @brief School-book subtraction
 * @param[in,out] it_lhs iterate through the digits of the lhs
 * @param[in,out] lhs the minuend
 * @param[in,out] it_rhs iterate through the digits of the rhs
 * @param rhs the subtrahend
 * @param[in,out] difference the difference
 */
inline void BigInt_8::subtract(size_t &it_lhs, BigInt_8 &lhs, size_t &it_rhs,
                               const BigInt_8 &rhs, BigInt_8 &difference) {
  while (it_lhs < lhs._data.size() && it_rhs < rhs._data.size()) {
    if (lhs._data[it_lhs] < rhs._data[it_rhs]) {
      lhs._data[it_lhs] += 10;
      if (lhs._data[it_lhs + 1] != 0) {
        lhs._data[it_lhs + 1] -= 1;
      } else {
        size_t tmp_it{1};
        while ((it_lhs + tmp_it) < lhs._data.size() - 1 &&
               lhs._data[it_lhs + tmp_it] == 0) {
          lhs._data[it_lhs + tmp_it] = 9;
          ++tmp_it;
        }
        lhs._data[it_lhs + tmp_it] -= 1;
      }
    }
    difference._data.push_back(lhs._data[it_lhs] - rhs._data[it_rhs]);
    ++it_lhs;
    ++it_rhs;
  }
}

/**
 * School-book subtraction -- performs all the x - 0 columns
 * @param[in,out] it iterate through the digits
 * @param bint_8 the number we are iterating through
 * @param[in,out] difference the difference
 */
inline void BigInt_8::s_carryDown(size_t &it, const BigInt_8 &bint_8,
                                  BigInt_8 &difference) {
  while (it < bint_8._data.size()) {
    difference._data.push_back(bint_8._data[it]);
    ++it;
  }
}

//------------------------------------------------------------------------------

inline BigInt_8 BigInt_8::operator*(const BigInt_8 &rhs) const {
  // naive implementation
  // schoolbook algorithm -- should be faster than repeated addition
  std::vector<BigInt_8> sums;
  sums.reserve(_data.size() + rhs._data.size());

  BigInt_8 product{};
  bool carry_bool = false;
  uint8_t carry_val{};
  size_t sum_index{0};

  // multiply digits and carry
  for (const auto &digit0 : _data) {
    sums.emplace_back(); // reserve does not actually initialize any data
    // pad with zeros
    if (sums.size() > 1) {
      for (size_t i = 0; i < sums.size() - 1; ++i) {
        sums[sum_index]._data.push_back(0);
      }
    }
    for (const auto &digit1 : rhs._data) {
      sums[sum_index]._data.push_back(digit0 * digit1 +
                                      (carry_bool ? carry_val : 0));
      if (sums[sum_index]._data.back() > 9) {
        carry_bool = true;
        carry_val = sums[sum_index]._data.back() / 10;
        sums[sum_index]._data.back() %= 10;
      } else {
        carry_bool = false;
      }
    }
    if (carry_bool) {
      sums[sum_index]._data.push_back(carry_val);
    }
    ++sum_index;
    carry_bool = false;
    carry_val = 0;
  }
  for (const auto &sum : sums) {
    product += sum;
  }
  product._sign = (_sign == rhs._sign) ? sign::positive : sign::negative;
  product.normalize();
  return product;
}

inline BigInt_8 &BigInt_8::operator+=(const BigInt_8 &rhs) {
  *this = *this + rhs;
  return *this;
}

inline BigInt_8 &BigInt_8::operator-=(const BigInt_8 &rhs) {
  *this = *this - rhs;
  return *this;
}

inline BigInt_8 &BigInt_8::operator*=(const BigInt_8 &rhs) {
  *this = *this * rhs;
  return *this;
}

// ------------------------------------------------------------------------------
// Unary operators

inline BigInt_8 &BigInt_8::operator-() {
  _sign = (_sign == sign::positive ? sign::negative : sign::positive);
  return *this;
}

inline BigInt_8 BigInt_8::operator-() const {
  BigInt_8 tmp = *this;
  tmp._sign = (tmp._sign == sign::positive ? sign::negative : sign::positive);
  return tmp;
}

//------------------------------------------------------------------------------
// Increment decrement operators

inline BigInt_8 &BigInt_8::operator++() {
  size_t i{0};
  // while there is carry propagation
  while (i < _data.size() && ++_data[i] == 10) {
    // propagate
    _data[i++] = 0;
  }
  // if we reach the end, execute the final carry
  if (i == _data.size()) {
    _data.push_back(1);
  }
  return *this;
}

inline BigInt_8 &BigInt_8::operator--() {

  if (_sign == sign::negative) {
    // -X - 1 = -(X + 1) → Use `operator++()` on the absolute value
    *this = -(-*this + BigInt_8{"1"});
    return *this;
  }

  // Normal case: subtract 1 from a positive BigInt
  size_t i = 0;
  while (i < _data.size()) {
    if (_data[i] > 0) {
      --_data[i]; // No borrow needed
      break;
    }
    _data[i++] = 9; // Borrow needed
  }
  return *this;
}

//------------------------------------------------------------------------------
// Stream operators

inline std::ostream &operator<<(std::ostream &os, const BigInt_8 &b) {
  if (b._data.empty()) {
    os << "0";
    return os;
  }
  if (b._sign == sign::negative) {
    os << '-';
  }
  for (auto it = b._data.rbegin(); it != b._data.rend(); ++it) {
    os << static_cast<char>(*it + '0');
  }
  return os;
}

//------------------------------------------------------------------------------
// Member functions

inline void BigInt_8::normalize() {
  while (_data.size() > 1 && _data.back() == 0) {
    _data.pop_back();
  }
  if (_data.empty() || _data == std::vector{uint8_t{0}}) {
    _sign = sign::positive;
  }
}

//------------------------------------------------------------------------------
// Non-member functions

inline BigInt_8 pow(const BigInt_8 &base, const uint exp) {
  BigInt_8 _base = base;
  uint _exp = exp;
  if (BigInt_8(std::to_string(_exp)) == BigInt_8(std::string{'0'})) {
    return BigInt_8(std::string{'1'});
  }
  if (base == BigInt_8(std::string{'0'})) {
    return BigInt_8(std::string{'0'});
  }
  BigInt_8 res{std::string{'1'}};
  while (_exp > 0) {
    if (_exp % 2 == 1) {
      res *= _base;
    }
    _base *= _base;
    _exp /= 2;
  }
  return res;
}

} // namespace sch

#endif // SCH_INCLUDE_BIGINT_8_HPP_
