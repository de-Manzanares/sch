
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
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt(const T val) : BigInt(std::to_string(val)) {} // NOLINT
  ~BigInt() = default;

  BigInt(const BigInt &) = default;       // copy constructor
  BigInt(BigInt &&) = default;            // move constructor
  BigInt &operator=(BigInt &&) = default; // move assignment

  // Copy assignment

  BigInt &operator=(const BigInt &) = default;
  BigInt &operator=(const char *str);
  BigInt &operator=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator=(T val);

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

  BigInt &operator+=(const BigInt &rhs);
  BigInt &operator+=(const char *str);
  BigInt &operator+=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator+=(T val);

  BigInt &operator-=(const BigInt &rhs);
  BigInt &operator-=(const char *str);
  BigInt &operator-=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator-=(T val);

  BigInt &operator*=(const BigInt &rhs);
  BigInt &operator*=(const char *str);
  BigInt &operator*=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator*=(T val);

  BigInt &operator/=(const BigInt &rhs);
  BigInt &operator/=(const char *str);
  BigInt &operator/=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator/=(T val);

  BigInt &operator%=(const BigInt &rhs);
  BigInt &operator%=(const char *str);
  BigInt &operator%=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator%=(T val);

  BigInt &operator-();
  BigInt operator-() const;
  BigInt &operator++();
  BigInt &operator--();

  friend std::ostream &operator<<(std::ostream &os, const BigInt &b);
  void normalize();
  [[nodiscard]] std::string to_string() const;

 private:
  // constants
  static constexpr std::uint64_t EXP = 9; // 10^EXP
  static constexpr std::uint64_t BASE = 1'000'000'000;

  // private variables
  sign _sign = sign::positive;          ///< sign of the number
  std::vector<std::uint64_t> _digits{}; ///< @note little endian order

  // ADDITION HELPERS ----------------------------------------
  static void add(std::size_t &it_lhs, const BigInt &lhs, std::size_t &it_rhs,
                  const BigInt &rhs, bool &carry, BigInt &sum);
  static void a_carryDown(std::size_t &it, const BigInt &bint_8, bool &carry,
                          BigInt &sum);
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
        "BigInt10::BigInt10() : string contains non-numeric characters");
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

inline BigInt &BigInt::operator=(const char *str) {
  *this = BigInt{std::string{str}};
  return *this;
}

inline BigInt &BigInt::operator=(const std::string &str) {
  *this = BigInt{str};
  return *this;
}

template <typename T, typename> BigInt &BigInt::operator=(T val) {
  *this = BigInt{val};
  return *this;
}

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

// ADDITION --------------------------------------------------------------------

inline BigInt BigInt::operator+(const BigInt &rhs) const { // NOLINT
  // todo optimizations for adding to 0 or 1 and so on
  // Initially, addition and subtraction were implemented assuming two
  // non-negative integers. Sign handling was introduced afterward; the most
  // straightforward approach to implementation was(is?,were?) the
  // conditional statements below. This allows us to reuse the subtraction
  // (addition) logic.
  // if (_sign != rhs._sign) {
  //   if (_sign == sign::negative) {
  //     return rhs - -*this;
  //   }
  //   if (rhs._sign == sign::negative) {
  //     return *this - -rhs;
  //   }
  // }
  // if (_sign == sign::negative && rhs._sign == sign::negative) {
  //   return -(-*this + -rhs);
  // }

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

// OVERLOADS -------------------------------------------------------------------

// COMPARISON OVERLOADS --------------------------------------------------------

// for comparison to zero

inline bool operator==(const BigInt &lhs, const int rhs) {
  return lhs == BigInt{rhs};
}

// for comparison to zero

inline bool operator==(const int lhs, const BigInt &rhs) {
  return BigInt{lhs} == rhs;
}

inline bool operator==(const BigInt &lhs, const char *str) {
  return lhs == BigInt{std::string{str}};
}

inline bool operator==(const char *str, const BigInt &rhs) {
  return rhs == BigInt{std::string{str}};
}

inline bool operator==(const BigInt &lhs, const std::string &str) {
  return lhs == BigInt{str};
}

inline bool operator==(const std::string &str, const BigInt &rhs) {
  return rhs == BigInt{str};
}

template <typename T, typename>
bool operator==(const BigInt &lhs, const T &rhs) {
  return lhs == BigInt{rhs};
}

template <typename T, typename>
bool operator==(const T &lhs, const BigInt &rhs) {
  return BigInt{lhs} == rhs;
}

inline bool operator!=(const BigInt &lhs, const char *str) {
  return lhs != BigInt{std::string{str}};
}

inline bool operator!=(const char *str, const BigInt &rhs) {
  return rhs != BigInt{std::string{str}};
}

inline bool operator!=(const BigInt &lhs, const std::string &str) {
  return lhs != BigInt{str};
}

inline bool operator!=(const std::string &str, const BigInt &rhs) {
  return rhs != BigInt{str};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator!=(const BigInt &lhs, const T val) {
  return lhs != BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator!=(const T val, const BigInt &rhs) {
  return rhs != BigInt{val};
}

inline bool operator<(const BigInt &lhs, const char *str) {
  return lhs < BigInt{std::string{str}};
}

inline bool operator<(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} < rhs;
}

inline bool operator<(const BigInt &lhs, const std::string &str) {
  return lhs < BigInt{str};
}

inline bool operator<(const std::string &str, const BigInt &rhs) {
  return BigInt{str} < rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<(const BigInt &lhs, const T val) {
  return lhs < BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<(const T val, const BigInt &rhs) {
  return BigInt{val} < rhs;
}

inline bool operator>(const BigInt &lhs, const char *str) {
  return lhs > BigInt{std::string{str}};
}

inline bool operator>(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} > rhs;
}

inline bool operator>(const BigInt &lhs, const std::string &str) {
  return lhs > BigInt{str};
}

inline bool operator>(const std::string &str, const BigInt &rhs) {
  return BigInt{str} > rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>(const BigInt &lhs, const T val) {
  return lhs > BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>(const T val, const BigInt &rhs) {
  return BigInt{val} > rhs;
}

inline bool operator<=(const BigInt &lhs, const char *str) {
  return lhs <= BigInt{std::string{str}};
}

inline bool operator<=(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} <= rhs;
}

inline bool operator<=(const BigInt &lhs, const std::string &str) {
  return lhs <= BigInt{str};
}

inline bool operator<=(const std::string &str, const BigInt &rhs) {
  return BigInt{str} <= rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<=(const BigInt &lhs, const T val) {
  return lhs <= BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<=(const T val, const BigInt &rhs) {
  return BigInt{val} <= rhs;
}

inline bool operator>=(const BigInt &lhs, const char *str) {
  return lhs >= BigInt{std::string{str}};
}

inline bool operator>=(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} >= rhs;
}

inline bool operator>=(const BigInt &lhs, const std::string &str) {
  return lhs >= BigInt{str};
}

inline bool operator>=(const std::string &str, const BigInt &rhs) {
  return BigInt{str} >= rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>=(const BigInt &lhs, const T val) {
  return lhs >= BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>=(const T val, const BigInt &rhs) {
  return BigInt{val} >= rhs;
}

// ARITHMETIC OVERLOADS --------------------------------------------------------

inline BigInt operator+(const BigInt &lhs, const char *str) {
  return lhs + BigInt{std::string{str}};
}

inline BigInt operator+(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} + rhs;
}

inline BigInt operator+(const BigInt &lhs, const std::string &str) {
  return lhs + BigInt{str};
}

inline BigInt operator+(const std::string &str, const BigInt &rhs) {
  return BigInt{str} + rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator+(const BigInt &lhs, const T val) {
  return lhs + BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator+(const T val, const BigInt &rhs) {
  return BigInt{val} + rhs;
}

} // namespace sch

#endif // SCH_INCLUDE_BigInt_HPP_
