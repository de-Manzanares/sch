
/*
 * Copyright (c) 2025 Drake Manzanares
 * Distributed under the MIT License.
 */

/**
 * @file BigInt.hpp
 * @brief Arbitrary precision integer
 */

#ifndef SCH_INCLUDE_BigInt_HPP_
#define SCH_INCLUDE_BigInt_HPP_

#include "BigInt10.hpp"
#include "schmath.hpp"
#include "sign.h"
#include <algorithm>
#include <cstdint>
#include <execution>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace sch {

/**
 * @class BigInt
 * @brief Arbitrary precision integer
 */
class BigInt {
 public:
  BigInt() = default;
  explicit BigInt(const std::string &str);
  explicit BigInt(const char *cstr) : BigInt(std::string{cstr}) {}
  explicit BigInt(const std::string_view strv) : BigInt(std::string{strv}) {}
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt(const T val) : BigInt(std::to_string(val)) {} // NOLINT
  ~BigInt() = default;

  BigInt(const BigInt &) = default;       // copy constructor
  BigInt(BigInt &&) = default;            // move constructor
  BigInt &operator=(BigInt &&) = default; // move assignment

  // Copy assignment

  BigInt &operator=(const BigInt &) = default;

  template <typename T,
            typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
  BigInt &operator=(const T &val) {
    *this = BigInt{val};
    return *this;
  }

  // Comparison operators

  bool operator==(const BigInt &rhs) const;
  bool operator!=(const BigInt &rhs) const;
  bool operator<(const BigInt &rhs) const;
  bool operator>(const BigInt &rhs) const;
  bool operator<=(const BigInt &rhs) const;
  bool operator>=(const BigInt &rhs) const;

  BigInt operator+(const BigInt &rhs) const;
  BigInt operator-(const BigInt &rhs) const;
  BigInt operator*(const BigInt &rhs) const;
  BigInt operator/(const BigInt &rhs) const;
  BigInt operator%(const BigInt &rhs) const;

  template <typename T,
            typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
  BigInt &operator+=(const T &rhs) {
    *this = *this + BigInt{rhs};
    return *this;
  }

  template <typename T,
            typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
  BigInt &operator-=(const T &rhs) {
    *this = *this - BigInt{rhs};
    return *this;
  }

  template <typename T,
            typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
  BigInt &operator*=(const T &rhs) {
    *this = *this * BigInt{rhs};
    return *this;
  }

  BigInt operator-() &&;
  BigInt operator-() const &;
  BigInt &operator++();
  BigInt &operator--();

  friend std::ostream &operator<<(std::ostream &os, const BigInt &b);
  void normalize();
  [[nodiscard]] std::string to_string() const;

 private:
  // constants
  static constexpr std::uint64_t EXP = 18; // 10^EXP
  static constexpr std::uint64_t BASE = 1'000'000'000'000'000'000;
  static constexpr std::uint64_t K_MAX_DIGIT = 4294967296; // sqrt(2^64)-1

  // private variables
  sign _sign = sign::positive;          ///< sign of the number
  std::vector<std::uint64_t> _digits{}; ///< @note little endian order

  // ADDITION HELPERS ----------------------------------------
  static void add(std::size_t &it_lhs, const BigInt &lhs, std::size_t &it_rhs,
                  const BigInt &rhs, bool &carry, BigInt &sum);
  static void a_carryDown(std::size_t &it, const BigInt &bint_8, bool &carry,
                          BigInt &sum);

  // SUBTRACTION HELPERS -------------------------------------
  static void subtract(std::size_t &it_lhs, BigInt &lhs, std::size_t &it_rhs,
                       const BigInt &rhs, BigInt &difference);
  static void s_carryDown(std::size_t &it, const BigInt &bint_8,
                          BigInt &difference);

  // MULTIPLICATION -------------------------------------------
  [[nodiscard]] static std::string_view abs(const std::string &str);
  static BigInt karatsuba(std::string_view lhs, std::string_view rhs);
};

// CONSTRUCTOR -----------------------------------------------------------------

inline BigInt::BigInt(const std::string &str) {
  int minusSignOffset = 0;  // to ignore negative sign, if it exists
  if (str.front() == '-') { // check for sign
    minusSignOffset = 1;
    _sign = sign::negative;
  }
  // ensure there are no other non-numeric characters
  if (!std::all_of(str.begin() + minusSignOffset, str.end(), isdigit)) {
    throw std::invalid_argument(
        "BigInt::BigInt() : string contains non-numeric characters");
  }

  _digits.reserve(str.size() / EXP + 1);

  // to grab the first undersized chunk
  const std::size_t chunkOffset = (str.size() - minusSignOffset) % EXP;
  // start grabbing the regular-sized chunks from here
  const std::size_t offset = chunkOffset + minusSignOffset;

  // grab the first undersized chunk
  if (chunkOffset != 0) {
    _digits.emplace_back(std::stoull(
        std::string{str.begin() + minusSignOffset, str.begin() + offset}));
  }

  // grab the rest of the chunks
  for (std::size_t i = offset; i < str.size(); i += EXP) {
    _digits.emplace_back(std::stoll(str.substr(i, EXP)));
  }

  // little endian order lol
  std::reverse(_digits.begin(), _digits.end());
  this->normalize(); // to remove empty elements
}

// COPY ASSIGNMENT -------------------------------------------------------------

// COMPARISON OPERATORS --------------------------------------------------------

inline bool BigInt::operator==(const BigInt &rhs) const {
  // NOLINTNEXTLINE _sign field is definitely initialized
  return _digits == rhs._digits && _sign == rhs._sign;
}

inline bool BigInt::operator!=(const BigInt &rhs) const {
  return !(*this == rhs);
}

inline bool BigInt::operator<(const BigInt &rhs) const {
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
  if (_digits.size() != rhs._digits.size()) {
    return (_sign == sign::positive ? _digits.size() < rhs._digits.size()
                                    : rhs._digits.size() < _digits.size());
  }
  // same length ------------------------
  // if positive, lexicographically "smaller" is smaller
  // if negative, lexicographically "larger" is smaller
  return (_sign == sign::positive
              ? std::lexicographical_compare(_digits.rbegin(), _digits.rend(),
                                             rhs._digits.rbegin(),
                                             rhs._digits.rend())
              : std::lexicographical_compare(rhs._digits.rbegin(),
                                             rhs._digits.rend(),
                                             _digits.rbegin(), _digits.rend()));
  // --------------------------------------------------
}

inline bool BigInt::operator>(const BigInt &rhs) const { return rhs < *this; }

inline bool BigInt::operator<=(const BigInt &rhs) const {
  return !(*this > rhs);
}

inline bool BigInt::operator>=(const BigInt &rhs) const {
  return !(*this < rhs);
}

// ARITHMETIC OPERATORS --------------------------------------------------------

// UNARY MINUS -----------------------------------------------------------------

inline BigInt BigInt::operator-() && {
  _sign = _sign == sign::positive ? sign::negative : sign::positive;
  return std::move(*this);
}

inline BigInt BigInt::operator-() const & {
  BigInt tmp = *this;
  tmp._sign = (tmp._sign == sign::positive ? sign::negative : sign::positive);
  return tmp;
}

// ADDITION --------------------------------------------------------------------

inline BigInt BigInt::operator+(const BigInt &rhs) const { // NOLINT
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

  BigInt sum;
  bool carry = false;
  std::size_t it_lhs{0}; // iterate through the digits of the lhs
  std::size_t it_rhs{0}; // iterate through the digits of the rhs

  sum._digits.reserve(_digits.size() > rhs._digits.size() ? _digits.size()
                                                          : rhs._digits.size());

  add(it_lhs, *this, it_rhs, rhs, carry, sum);
  a_carryDown(it_lhs, *this, carry, sum);
  a_carryDown(it_rhs, rhs, carry, sum);

  if (carry) { // final carry
    sum._digits.push_back(1);
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
inline void BigInt::add(std::size_t &it_lhs, const BigInt &lhs,
                        std::size_t &it_rhs, const BigInt &rhs, bool &carry,
                        BigInt &sum) {
  while (it_lhs < lhs._digits.size() && it_rhs < rhs._digits.size()) {
    sum._digits.push_back(lhs._digits[it_lhs] + rhs._digits[it_rhs] +
                          (carry ? 1 : 0));
    if (sum._digits.back() > BASE - 1) {
      carry = true;
      sum._digits.back() -= BASE;
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
inline void BigInt::a_carryDown(std::size_t &it, const BigInt &bint_8,
                                bool &carry, BigInt &sum) {
  while (it < bint_8._digits.size()) {
    sum._digits.push_back(bint_8._digits[it] + (carry ? 1 : 0));
    if (sum._digits.back() > BASE - 1) {
      carry = true;
      sum._digits.back() -= BASE;
    } else {
      carry = false;
    }
    ++it;
  }
}

// SUBTRACTION -----------------------------------------------------------------

// is there a way to work around using copies to maintain constness?

inline BigInt BigInt::operator-(const BigInt &rhs) const { // NOLINT
  // todo optimizations for subtracting to and from 0 or 1 and so on
  // Initially, addition and subtraction were implemented assuming two
  // non-negative integers. Sign handling was introduced afterward; the most
  // straightforward approach to implementation was(is?,were?) the conditional
  // statements below. This allows us to reuse the subtraction (addition) logic.
  if (*this == rhs) {
    return BigInt{0};
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

  BigInt difference{};
  BigInt m_lhs{*this};   // mutable copy
  BigInt m_rhs{rhs};     // mutable copy
  std::size_t it_lhs{0}; // iterate through the digits of the lhs
  std::size_t it_rhs{0}; // iterate through the digits of the rhs

  difference._digits.reserve(_digits.size() > rhs._digits.size()
                                 ? _digits.size()
                                 : rhs._digits.size());

  if (m_rhs > m_lhs) {
    difference._sign = sign::negative; // otherwise dif sign is pos. by default
  }
  if (difference._sign == sign::positive) { // subtract rhs from lhs
    subtract(it_lhs, m_lhs, it_rhs, m_rhs, difference);
  } else { // subtract lhs from rhs
    subtract(it_rhs, m_rhs, it_lhs, m_lhs, difference);
  }
  s_carryDown(it_lhs, m_lhs, difference);
  s_carryDown(it_rhs, m_rhs, difference);

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
inline void BigInt::subtract(std::size_t &it_lhs, BigInt &lhs,
                             std::size_t &it_rhs, const BigInt &rhs,
                             BigInt &difference) {
  while (it_lhs < lhs._digits.size() && it_rhs < rhs._digits.size()) {
    if (lhs._digits[it_lhs] < rhs._digits[it_rhs]) {
      lhs._digits[it_lhs] += BASE;
      if (lhs._digits[it_lhs + 1] != 0) {
        lhs._digits[it_lhs + 1] -= 1;
      } else {
        std::size_t tmp_it{1};
        while ((it_lhs + tmp_it) < lhs._digits.size() - 1 &&
               lhs._digits[it_lhs + tmp_it] == 0) {
          lhs._digits[it_lhs + tmp_it] = BASE - 1;
          ++tmp_it;
        }
        lhs._digits[it_lhs + tmp_it] -= 1;
      }
    }
    difference._digits.push_back(lhs._digits[it_lhs] - rhs._digits[it_rhs]);
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
inline void BigInt::s_carryDown(std::size_t &it, const BigInt &bint_8,
                                BigInt &difference) {
  while (it < bint_8._digits.size()) {
    difference._digits.push_back(bint_8._digits[it]);
    ++it;
  }
}

// MULTIPLICATION --------------------------------------------------------------

inline std::string_view BigInt::abs(const std::string &str) {
  if (str.front() == '-') {
    return std::string_view{str}.substr(1);
  }
  return std::string_view{str};
}

inline BigInt BigInt::karatsuba(const std::string_view lhs, // NOLINT recursion
                                const std::string_view rhs) {
  if (std::all_of(lhs.begin(), lhs.end(),
                  [](const char c) { return c == '0'; }) ||
      std::all_of(rhs.begin(), rhs.end(),
                  [](const char c) { return c == '0'; })) {
    return BigInt{0};
  }

  if (lhs.size() < 10 && rhs.size() < 10) {
    return std::stoull(std::string{lhs}) * std::stoull(std::string{rhs});
  }

  const std::size_t m = std::max(lhs.size(), rhs.size());
  const std::size_t n = m / 2;
  std::string_view a;
  std::string_view b;
  std::string_view c;
  std::string_view d;
  if (lhs.size() > n) {
    a = lhs.substr(0, lhs.size() - n);
    b = lhs.substr(lhs.size() - n);
  } else {
    a = "0";
    b = lhs;
  }
  if (rhs.size() > n) {
    c = rhs.substr(0, rhs.size() - n);
    d = rhs.substr(rhs.size() - n);
  } else {
    c = "0";
    d = rhs;
  }
  const BigInt ac = karatsuba(a, c);
  const BigInt bd = karatsuba(b, d);
  const BigInt sum_ad_bc = karatsuba((BigInt{a} + BigInt{b}).to_string(),
                                     (BigInt{c} + BigInt{d}).to_string()) -
                           ac - bd;
  return BigInt{ac.to_string() + std::string(2 * n, '0')} +
         BigInt{sum_ad_bc.to_string() + std::string(n, '0')} + bd;
}

inline BigInt BigInt::operator*(const BigInt &rhs) const {
  if (*this == 0 || rhs == 0) {
    return 0;
  }
  return _sign == rhs._sign
             ? karatsuba(abs(this->to_string()), abs(rhs.to_string()))
             : -karatsuba(abs(this->to_string()), abs(rhs.to_string()));
}

// MEMBER FUNCTIONS ------------------------------------------------------------

inline void BigInt::normalize() {
  while (_digits.size() > 1 && _digits.back() == 0) {
    _digits.pop_back();
  }
  if (_digits.empty() || (_digits.size() == 1 && _digits.front() == 0)) {
    _sign = sign::positive;
  }
}

inline std::string BigInt::to_string() const {
  std::string str;
  if (_sign == sign::negative) {
    str += "-";
  }
  // the first "digit" doesn't need leading zeros
  str += std::to_string(*_digits.rbegin());

  for (auto it = _digits.rbegin() + 1; it != _digits.rend(); ++it) {
    std::string tmp{std::to_string(*it)};
    while (tmp.size() < EXP) {
      tmp.insert(0, 1, '0'); // leading zeros for each chunk
    }
    str += tmp;
  }
  return str;
}

// FRIEND FUNCTIONS ------------------------------------------------------------

inline std::ostream &operator<<(std::ostream &os, const BigInt &b) {
  os << b.to_string();
  return os;
}

// TEMPLATED OPERATORS ---------------------------------------------------------

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator==(const BigInt &lhs, const T &val) {
  return lhs == BigInt{val};
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator==(const T &val, const BigInt &rhs) {
  return BigInt{val} == rhs;
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator!=(const BigInt &lhs, const T &val) {
  return lhs != BigInt{val};
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator!=(const T &val, const BigInt &rhs) {
  return BigInt{val} != rhs;
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator<(const BigInt &lhs, const T &val) {
  return lhs < BigInt{val};
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator<(const T &val, const BigInt &rhs) {
  return BigInt{val} < rhs;
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator>(const BigInt &lhs, const T &val) {
  return lhs > BigInt{val};
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator>(const T &val, const BigInt &rhs) {
  return BigInt{val} > rhs;
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator<=(const BigInt &lhs, const T &val) {
  return lhs <= BigInt{val};
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator<=(const T &val, const BigInt &rhs) {
  return BigInt{val} <= rhs;
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator>=(const BigInt &lhs, const T &val) {
  return lhs >= BigInt{val};
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
bool operator>=(const T &val, const BigInt &rhs) {
  return BigInt{val} >= rhs;
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
BigInt operator+(const BigInt &lhs, const T &val) {
  return lhs + BigInt{val};
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
BigInt operator+(const T &val, const BigInt &rhs) {
  return BigInt{val} + rhs;
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
BigInt operator-(const BigInt &lhs, const T &val) {
  return lhs - BigInt{val};
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
BigInt operator-(const T &val, const BigInt &rhs) {
  return BigInt{val} - rhs;
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
BigInt operator*(const BigInt &lhs, const T val) {
  return lhs * BigInt{val};
}

template <typename T,
          typename = std::enable_if_t<std::is_constructible_v<BigInt, T>>>
BigInt operator*(const T &val, const BigInt &rhs) {
  return BigInt{val} * rhs;
}

} // namespace sch

#endif // SCH_INCLUDE_BigInt_HPP_
