
/*
 * Copyright (c) 2025 Drake Manzanares
 * Distributed under the MIT License.
 */

/**
 * @file BigInt10.hpp
 * @brief Arbitrary precision integer
 */

#ifndef SCH_INCLUDE_BigInt10_HPP_
#define SCH_INCLUDE_BigInt10_HPP_

#include <algorithm>
#include <cstdint>
#include <execution>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace sch {

enum class Sign10 : bool { negative, positive };

/**
 * @class BigInt10
 * @brief Arbitrary precision integer
 */
class BigInt10 {
  friend class BigInt;

public:
  BigInt10() = default;
  explicit BigInt10(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt10(const T val) : BigInt10(std::to_string(val)) {} // NOLINT
  ~BigInt10() = default;

  BigInt10(const BigInt10 &) = default;       // copy constructor
  BigInt10(BigInt10 &&) = default;            // move constructor
  BigInt10 &operator=(BigInt10 &&) = default; // move asSign10ment

  // Copy asSign10ment
  BigInt10 &operator=(const BigInt10 &) = default;
  BigInt10 &operator=(const char *str);
  BigInt10 &operator=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt10 &operator=(T val);

  bool operator==(const BigInt10 &rhs) const;
  bool operator!=(const BigInt10 &rhs) const;
  bool operator<(const BigInt10 &rhs) const;
  bool operator>(const BigInt10 &rhs) const;
  bool operator<=(const BigInt10 &rhs) const;
  bool operator>=(const BigInt10 &rhs) const;

  BigInt10 operator+(const BigInt10 &rhs) const;
  BigInt10 operator-(const BigInt10 &rhs) const;
  BigInt10 operator*(const BigInt10 &rhs) const;
  BigInt10 operator/(const BigInt10 &rhs) const;
  BigInt10 operator%(const BigInt10 &rhs) const;

  BigInt10 &operator+=(const BigInt10 &rhs);
  BigInt10 &operator+=(const char *str);
  BigInt10 &operator+=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt10 &operator+=(T val);

  BigInt10 &operator-=(const BigInt10 &rhs);
  BigInt10 &operator-=(const char *str);
  BigInt10 &operator-=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt10 &operator-=(T val);

  BigInt10 &operator*=(const BigInt10 &rhs);
  BigInt10 &operator*=(const char *str);
  BigInt10 &operator*=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt10 &operator*=(T val);

  BigInt10 &operator/=(const BigInt10 &rhs);
  BigInt10 &operator/=(const char *str);
  BigInt10 &operator/=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt10 &operator/=(T val);

  BigInt10 &operator%=(const BigInt10 &rhs);
  BigInt10 &operator%=(const char *str);
  BigInt10 &operator%=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt10 &operator%=(T val);

  BigInt10 &operator-();
  BigInt10 operator-() const;
  BigInt10 &operator++();
  BigInt10 &operator--();

  friend std::ostream &operator<<(std::ostream &os, const BigInt10 &b);

  void normalize();
  [[nodiscard]] std::string to_string() const;

private:
  // constants
  static constexpr uint64_t BASE = 10; ///< data is stored in base 10

  // private variables
  Sign10 _Sign10 = Sign10::positive; ///< Sign10 of the number
  std::vector<uint8_t> _data{};      ///< @note little endian order

  // Addition operator helpers ---------------------------------
  static void add(std::size_t &it_lhs, const BigInt10 &lhs, std::size_t &it_rhs,
                  const BigInt10 &rhs, bool &carry, BigInt10 &sum);
  static void a_carryDown(std::size_t &it, const BigInt10 &bint_8, bool &carry,
                          BigInt10 &sum);

  // Subtraction operator helpers ------------------------------
  static void subtract(std::size_t &it_lhs, BigInt10 &lhs, std::size_t &it_rhs,
                       const BigInt10 &rhs, BigInt10 &difference);
  static void s_carryDown(std::size_t &it, const BigInt10 &bint_8,
                          BigInt10 &difference);

  // Multiplication operator -----------------------------------
  static BigInt10 longMultiplication(const BigInt10 &bottom,
                                     const BigInt10 &top);

  // Division operator -----------------------------------------
  static std::pair<BigInt10, BigInt10> longDivision(const BigInt10 &dividend,
                                                    const BigInt10 &divisor);
};

//------------------------------------------------------------------------------
// Constructors

// String constructor
inline BigInt10::BigInt10(const std::string &str) {
  int offset = 0;           // to ignore negative Sign10, if it exists
  if (str.front() == '-') { // check for Sign10
    offset = 1;
    _Sign10 = Sign10::negative;
  }
  // ensure there are no other non-numeric characters
  if (!std::all_of(str.begin() + offset, str.end(), isdigit)) {
    throw std::invalid_argument(
        "BigInt10::BigInt10() : string contains non-numeric characters");
  }
  _data = std::vector<uint8_t>(str.rbegin(), str.rend() - offset);
  for (auto &digit : _data) {
    digit -= '0';
  }
  // the BigInt10 objects can sometimes be initialized with leading zeros
  // having leading zeros will cause incorrect comparisons between BigInt10s
  // best to get rid of them now
  this->normalize();
}

// COPY ASSign10MENT
// -------------------------------------------------------------

inline BigInt10 &BigInt10::operator=(const char *str) {
  *this = BigInt10{std::string(str)};
  return *this;
}

inline BigInt10 &BigInt10::operator=(const std::string &str) {
  *this = BigInt10{str};
  return *this;
}

template <typename T, typename> BigInt10 &BigInt10::operator=(const T val) {
  *this = BigInt10{val};
  return *this;
}

// EQUALITY OPERATORS-----------------------------------------------------------

inline bool BigInt10::operator==(const BigInt10 &rhs) const {
  return (_data == rhs._data && _Sign10 == rhs._Sign10); // NOLINT
}

// for comparison to zero

inline bool operator==(const BigInt10 &lhs, const int rhs) {
  return lhs == BigInt10{rhs};
}

// for comparison to zero

inline bool operator==(const int lhs, const BigInt10 &rhs) {
  return BigInt10{lhs} == rhs;
}

inline bool operator==(const BigInt10 &lhs, const char *str) {
  return lhs == BigInt10{std::string{str}};
}

inline bool operator==(const char *str, const BigInt10 &rhs) {
  return rhs == BigInt10{std::string{str}};
}

inline bool operator==(const BigInt10 &lhs, const std::string &str) {
  return lhs == BigInt10{str};
}

inline bool operator==(const std::string &str, const BigInt10 &rhs) {
  return rhs == BigInt10{str};
}

template <typename T, typename>
bool operator==(const BigInt10 &lhs, const T &rhs) {
  return lhs == BigInt10{rhs};
}

template <typename T, typename>
bool operator==(const T &lhs, const BigInt10 &rhs) {
  return BigInt10{lhs} == rhs;
}

inline bool BigInt10::operator!=(const BigInt10 &rhs) const {
  return !(*this == rhs);
}

inline bool operator!=(const BigInt10 &lhs, const char *str) {
  return lhs != BigInt10{std::string{str}};
}

inline bool operator!=(const char *str, const BigInt10 &rhs) {
  return rhs != BigInt10{std::string{str}};
}

inline bool operator!=(const BigInt10 &lhs, const std::string &str) {
  return lhs != BigInt10{str};
}

inline bool operator!=(const std::string &str, const BigInt10 &rhs) {
  return rhs != BigInt10{str};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator!=(const BigInt10 &lhs, const T val) {
  return lhs != BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator!=(const T val, const BigInt10 &rhs) {
  return rhs != BigInt10{val};
}

// RELATIONAL OPERATORS --------------------------------------------------------

// LESS THAN -------------------------------------------------------------------

inline bool BigInt10::operator<(const BigInt10 &rhs) const {
  // opposite Sign10 considerations ---------------------
  if (_Sign10 == Sign10::negative && rhs._Sign10 == Sign10::positive) {
    return true;
  }
  if (_Sign10 == Sign10::positive && rhs._Sign10 == Sign10::negative) {
    return false;
  }
  // same Sign10 considerations -------------------------
  // different lengths ------------------
  // if positive, shorter is smaller
  // if negative, longer is smaller
  if (_data.size() != rhs._data.size()) {
    return _Sign10 == Sign10::positive ? _data.size() < rhs._data.size()
                                       : rhs._data.size() < _data.size();
  }
  // same length ------------------------
  // if positive, lexicographically "smaller" is smaller
  // if negative, lexicographically "larger" is smaller
  return _Sign10 == Sign10::positive
             ? std::lexicographical_compare(_data.rbegin(), _data.rend(),
                                            rhs._data.rbegin(),
                                            rhs._data.rend())
             : std::lexicographical_compare(rhs._data.rbegin(),
                                            rhs._data.rend(), _data.rbegin(),
                                            _data.rend());
  // --------------------------------------------------
}

inline bool operator<(const BigInt10 &lhs, const char *str) {
  return lhs < BigInt10{std::string{str}};
}

inline bool operator<(const char *str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} < rhs;
}

inline bool operator<(const BigInt10 &lhs, const std::string &str) {
  return lhs < BigInt10{std::string{str}};
}

inline bool operator<(const std::string &str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} < rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<(const BigInt10 &lhs, const T val) {
  return lhs < BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<(const T val, const BigInt10 &rhs) {
  return BigInt10{val} < rhs;
}

// GREATER THAN ----------------------------------------------------------------

inline bool BigInt10::operator>(const BigInt10 &rhs) const {
  return rhs < *this;
}

inline bool operator>(const BigInt10 &lhs, const char *str) {
  return lhs > BigInt10{std::string{str}};
}

inline bool operator>(const char *str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} > rhs;
}

inline bool operator>(const BigInt10 &lhs, const std::string &str) {
  return lhs > BigInt10{str};
}

inline bool operator>(const std::string &str, const BigInt10 &rhs) {
  return BigInt10{str} > rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>(const BigInt10 &lhs, const T val) {
  return lhs > BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>(const T val, const BigInt10 &rhs) {
  return BigInt10{val} > rhs;
}

// LESS THAN OR EQUAL TO -------------------------------------------------------

inline bool BigInt10::operator<=(const BigInt10 &rhs) const {
  return !(*this > rhs);
}

inline bool operator<=(const BigInt10 &lhs, const char *str) {
  return lhs <= BigInt10{std::string{str}};
}

inline bool operator<=(const char *str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} <= rhs;
}

inline bool operator<=(const BigInt10 &lhs, const std::string &str) {
  return lhs <= BigInt10{str};
}

inline bool operator<=(const std::string &str, const BigInt10 &rhs) {
  return BigInt10{str} <= rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<=(const BigInt10 &lhs, const T val) {
  return lhs <= BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<=(const T val, const BigInt10 &rhs) {
  return BigInt10{val} <= rhs;
}

// GREATER THAN OR EQUAL TO ----------------------------------------------------

inline bool BigInt10::operator>=(const BigInt10 &rhs) const {
  return !(*this < rhs);
}

inline bool operator>=(const BigInt10 &lhs, const char *str) {
  return lhs >= BigInt10{std::string{str}};
}

inline bool operator>=(const char *str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} >= rhs;
}

inline bool operator>=(const BigInt10 &lhs, const std::string &str) {
  return lhs >= BigInt10{str};
}

inline bool operator>=(const std::string &str, const BigInt10 &rhs) {
  return BigInt10{str} >= rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>=(const BigInt10 &lhs, const T val) {
  return lhs >= BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>=(const T val, const BigInt10 &rhs) {
  return BigInt10{val} >= rhs;
}

// ADDITION OPERATOR -----------------------------------------------------------

inline BigInt10 BigInt10::operator+(const BigInt10 &rhs) const { // NOLINT
  // todo optimizations for adding to 0 or 1 and so on
  // Initially, addition and subtraction were implemented assuming two
  // non-negative integers. Sign10 handling was introduced afterward; the most
  // straightforward approach to implementation was(is?,were?) the
  // conditional statements below. This allows us to reuse the subtraction
  // (addition) logic.
  if (_Sign10 != rhs._Sign10) {
    if (_Sign10 == Sign10::negative) {
      return rhs - -*this;
    }
    if (rhs._Sign10 == Sign10::negative) {
      return *this - -rhs;
    }
  }
  if (_Sign10 == Sign10::negative && rhs._Sign10 == Sign10::negative) {
    return -(-*this + -rhs);
  }

  BigInt10 sum;
  bool carry = false;
  std::size_t it_lhs{0}; // iterate through the digits of the lhs
  std::size_t it_rhs{0}; // iterate through the digits of the rhs

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
inline void BigInt10::add(std::size_t &it_lhs, const BigInt10 &lhs,
                          std::size_t &it_rhs, const BigInt10 &rhs, bool &carry,
                          BigInt10 &sum) {
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
inline void BigInt10::a_carryDown(std::size_t &it, const BigInt10 &bint_8,
                                  bool &carry, BigInt10 &sum) {
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

inline BigInt10 operator+(const BigInt10 &lhs, const char *str) {
  return lhs + BigInt10{std::string{str}};
}

inline BigInt10 operator+(const char *str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} + rhs;
}

inline BigInt10 operator+(const BigInt10 &lhs, const std::string &str) {
  return lhs + BigInt10{str};
}

inline BigInt10 operator+(const std::string &str, const BigInt10 &rhs) {
  return BigInt10{str} + rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator+(const BigInt10 &lhs, const T val) {
  return lhs + BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator+(const T val, const BigInt10 &rhs) {
  return BigInt10{val} + rhs;
}

// SUBTRACTION OPERATOR --------------------------------------------------------

// is there a way to work around using copies to maintain constness?

inline BigInt10 BigInt10::operator-(const BigInt10 &rhs) const { // NOLINT
  // todo optimizations for subtracting to and from 0 or 1 and so on
  // Initially, addition and subtraction were implemented assuming two
  // non-negative integers. Sign10 handling was introduced afterward; the most
  // straightforward approach to implementation was(is?,were?) the conditional
  // statements below. This allows us to reuse the subtraction (addition) logic.
  if (*this == rhs) {
    return BigInt10{"0"};
  }
  if (_Sign10 != rhs._Sign10) {
    if (_Sign10 == Sign10::negative) {
      return -(-*this + rhs);
    }
    if (rhs._Sign10 == Sign10::negative) {
      return *this + -rhs;
    }
  }
  if (_Sign10 == Sign10::negative && rhs._Sign10 == Sign10::negative) {
    return -rhs - -*this;
  }

  BigInt10 difference{};
  BigInt10 _lhs{*this};  // mutable copy
  BigInt10 _rhs{rhs};    // mutable copy
  std::size_t it_lhs{0}; // iterate through the digits of the lhs
  std::size_t it_rhs{0}; // iterate through the digits of the rhs

  difference._data.reserve(_data.size() > rhs._data.size() ? _data.size()
                                                           : rhs._data.size());

  if (_rhs > _lhs) {
    difference._Sign10 =
        Sign10::negative; // otherwise dif Sign10 is pos. by default
  }
  if (difference._Sign10 == Sign10::positive) { // subtract rhs from lhs
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
inline void BigInt10::subtract(std::size_t &it_lhs, BigInt10 &lhs,
                               std::size_t &it_rhs, const BigInt10 &rhs,
                               BigInt10 &difference) {
  while (it_lhs < lhs._data.size() && it_rhs < rhs._data.size()) {
    if (lhs._data[it_lhs] < rhs._data[it_rhs]) {
      lhs._data[it_lhs] += 10;
      if (lhs._data[it_lhs + 1] != 0) {
        lhs._data[it_lhs + 1] -= 1;
      } else {
        std::size_t tmp_it{1};
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
inline void BigInt10::s_carryDown(std::size_t &it, const BigInt10 &bint_8,
                                  BigInt10 &difference) {
  while (it < bint_8._data.size()) {
    difference._data.push_back(bint_8._data[it]);
    ++it;
  }
}

inline BigInt10 operator-(const BigInt10 &lhs, const char *str) {
  return lhs - BigInt10{std::string{str}};
}

inline BigInt10 operator-(const char *str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} - rhs;
}

inline BigInt10 operator-(const BigInt10 &lhs, const std::string &str) {
  return lhs - BigInt10{str};
}

inline BigInt10 operator-(const std::string &str, const BigInt10 &rhs) {
  return BigInt10{str} - rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator-(const BigInt10 &lhs, const T val) {
  return lhs - BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator-(const T val, const BigInt10 &rhs) {
  return BigInt10{val} - rhs;
}

// MULTIPLICATION OPERATOR -----------------------------------------------------

inline BigInt10 BigInt10::operator*(const BigInt10 &rhs) const {
  // minimize number of operations by putting the shorter number on the "bottom"
  return _data.size() < rhs._data.size() ? longMultiplication(*this, rhs)
                                         : longMultiplication(rhs, *this);
}

/**
 * @brief School-book multiplication
 * @param bottom the number on the "bottom" of the long multiplication setup
 * @param top the number on the "top" of the long multiplication setup
 * @return the product of the two factors
 */
inline BigInt10 BigInt10::longMultiplication(const BigInt10 &bottom,
                                             const BigInt10 &top) {
  BigInt10 final_product{};       // the sum of the intermediate products
  std::vector<BigInt10> products; // the intermediate products
  uint8_t carry{0};               // what value is being carried?
  std::size_t latest_prod{0};     // latest intermediate product

  products.reserve(bottom._data.size());

  for (const auto &n_b : bottom._data) {
    // reserve does not initialize the objects
    products.emplace_back();
    products[latest_prod]._data.reserve(products.size() + top._data.size());

    // the intermediate products must be raised to the correct power of 10
    products[latest_prod]._data.resize(products.size() - 1, 0);

    // multiply the bottom digit by each of the top digits
    for (const auto &n_t : top._data) {
      products[latest_prod]._data.push_back(n_b * n_t + (carry ? carry : 0));
      if (products[latest_prod]._data.back() > BASE - 1) {
        carry = products[latest_prod]._data.back() / BASE;
        products[latest_prod]._data.back() %= BASE;
      } else {
        carry = 0;
      }
    }
    if (carry) { // final carry of the iteration
      products[latest_prod]._data.push_back(carry);
    }
    ++latest_prod; // moving on
    carry = 0;     // reset for the next intermediate product
  }
  final_product = std::reduce(std::execution::par, products.begin(),
                              products.end(), BigInt10{});
  final_product._Sign10 =
      bottom._Sign10 == top._Sign10 ? Sign10::positive : Sign10::negative;
  final_product.normalize();
  return final_product;
}

inline BigInt10 operator*(const BigInt10 &lhs, const char *str) {
  return lhs * BigInt10{std::string{str}};
}

inline BigInt10 operator*(const char *str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} * rhs;
}

inline BigInt10 operator*(const BigInt10 &lhs, const std::string &str) {
  return lhs * BigInt10{str};
}

inline BigInt10 operator*(const std::string &str, const BigInt10 &rhs) {
  return BigInt10{str} * rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator*(const BigInt10 &lhs, const T val) {
  return lhs * BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator*(const T val, const BigInt10 &rhs) {
  return BigInt10{val} * rhs;
}

// DIVISION and MODULO OPERATORS -----------------------------------------------

inline BigInt10 BigInt10::operator/(const BigInt10 &rhs) const {
  return longDivision(*this, rhs).first;
}

inline BigInt10 BigInt10::operator%(const BigInt10 &rhs) const {
  return longDivision(*this, rhs).second;
}

/**
 * @brief School-book division
 * @param dividend the number to be divided
 * @param divisor the number to divide by
 * @return {quotient,remainder}
 */
inline std::pair<BigInt10, BigInt10>
BigInt10::longDivision(const BigInt10 &dividend, const BigInt10 &divisor) {
  if (divisor == "0") {
    throw std::runtime_error(
        "BigInt10::operator/() : Division by zero is undefined");
  }

  BigInt10 m_dividend{dividend}; // mutable copy
  BigInt10 m_divisor{divisor};   // mutable copy
  BigInt10 quotient{};
  BigInt10 remainder{};

  m_dividend._Sign10 = Sign10::positive;
  m_divisor._Sign10 = Sign10::positive;

  auto chooseQuotientSign10 = [&dividend, &divisor] {
    return dividend._Sign10 == divisor._Sign10 ? Sign10::positive
                                               : Sign10::negative;
  };
  auto chooseRemainderSign10 = [&dividend, &remainder] {
    if (remainder == "0") {
      return Sign10::positive;
    }
    return dividend._Sign10 == Sign10::positive ? Sign10::positive
                                                : Sign10::negative;
  };

  if (m_divisor == m_dividend) {
    quotient = 1;
    quotient._Sign10 = chooseQuotientSign10();
    return {quotient, 0};
  }
  if (m_divisor > m_dividend) {
    return {0, dividend};
  }
  if (m_divisor == 1) {
    quotient = m_dividend;
    quotient._Sign10 = chooseQuotientSign10();
    return {quotient, 0};
  }

  std::vector<BigInt10> products(10);
  products[0] = 0;
  std::generate(std::next(products.begin()), products.end(),
                [acc = BigInt10{0}, &m_divisor]() mutable {
                  acc += m_divisor;
                  return acc;
                });

  // find the first place of the quotient
  auto it = m_dividend._data.rbegin();
  while (it != m_dividend._data.rend()) {
    // todo maybe little endian order is not the best? lol
    remainder._data.insert(remainder._data.begin(), *it);
    ++it;

    uint8_t multiple{};
    remainder.normalize();
    // narrowing conversion
    multiple = std::distance(products.begin(),
                             std::prev(std::upper_bound(
                                 products.begin(), products.end(), remainder)));
    quotient._data.emplace_back(multiple);
    remainder -= m_divisor * multiple;
  }

  std::reverse(quotient._data.begin(), quotient._data.end());
  quotient.normalize();
  quotient._Sign10 = chooseQuotientSign10();
  remainder._Sign10 = chooseRemainderSign10();
  return {quotient, remainder};
}

inline BigInt10 operator/(const BigInt10 &lhs, const char *str) {
  return lhs / BigInt10{std::string{str}};
}

inline BigInt10 operator/(const char *str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} / rhs;
}

inline BigInt10 operator/(const BigInt10 &lhs, const std::string &str) {
  return lhs / BigInt10{str};
}

inline BigInt10 operator/(const std::string &str, const BigInt10 &rhs) {
  return BigInt10{str} / rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator/(const BigInt10 &lhs, const T val) {
  return lhs / BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator/(const T val, const BigInt10 &rhs) {
  return BigInt10{val} / rhs;
}

inline BigInt10 operator%(const BigInt10 &lhs, const char *str) {
  return lhs % BigInt10{std::string{str}};
}

inline BigInt10 operator%(const char *str, const BigInt10 &rhs) {
  return BigInt10{std::string{str}} % rhs;
}

inline BigInt10 operator%(const BigInt10 &lhs, const std::string &str) {
  return lhs % BigInt10{str};
}

inline BigInt10 operator%(const std::string &str, const BigInt10 &rhs) {
  return BigInt10{str} % rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator%(const BigInt10 &lhs, const T val) {
  return lhs % BigInt10{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 operator%(const T val, const BigInt10 &rhs) {
  return BigInt10{val} % rhs;
}

// SELF ASSign10MENT OPERATORS
// ---------------------------------------------------

inline BigInt10 &BigInt10::operator+=(const BigInt10 &rhs) {
  *this = *this + rhs;
  return *this;
}

inline BigInt10 &BigInt10::operator+=(const char *str) {
  *this = *this + BigInt10{std::string{str}};
  return *this;
}

inline BigInt10 &BigInt10::operator+=(const std::string &str) {
  *this = *this + BigInt10{str};
  return *this;
}

template <typename T, typename> BigInt10 &BigInt10::operator+=(const T val) {
  *this = *this + BigInt10{val};
  return *this;
}

inline BigInt10 &BigInt10::operator-=(const BigInt10 &rhs) {
  *this = *this - rhs;
  return *this;
}

inline BigInt10 &BigInt10::operator-=(const char *str) {
  *this = *this - BigInt10{std::string{str}};
  return *this;
}

inline BigInt10 &BigInt10::operator-=(const std::string &str) {
  *this = *this - BigInt10{str};
  return *this;
}

template <typename T, typename> BigInt10 &BigInt10::operator-=(const T val) {
  *this = *this - BigInt10{val};
  return *this;
}

inline BigInt10 &BigInt10::operator*=(const BigInt10 &rhs) {
  *this = *this * rhs;
  return *this;
}

inline BigInt10 &BigInt10::operator*=(const char *str) {
  *this = *this * BigInt10{std::string{str}};
  return *this;
}

inline BigInt10 &BigInt10::operator*=(const std::string &str) {
  *this = *this * BigInt10{str};
  return *this;
}

template <typename T, typename> BigInt10 &BigInt10::operator*=(const T val) {
  *this = *this * BigInt10{val};
  return *this;
}

inline BigInt10 &BigInt10::operator/=(const BigInt10 &rhs) {
  *this = *this / rhs;
  return *this;
}

inline BigInt10 &BigInt10::operator/=(const char *str) {
  *this = *this / BigInt10{std::string{str}};
  return *this;
}

inline BigInt10 &BigInt10::operator/=(const std::string &str) {
  *this = *this / BigInt10{str};
  return *this;
}

template <typename T, typename> BigInt10 &BigInt10::operator/=(const T val) {
  *this = *this / BigInt10{val};
  return *this;
}

inline BigInt10 &BigInt10::operator%=(const BigInt10 &rhs) {
  *this = *this % rhs;
  return *this;
}

inline BigInt10 &BigInt10::operator%=(const char *str) {
  *this = *this % BigInt10{std::string{str}};
  return *this;
}

inline BigInt10 &BigInt10::operator%=(const std::string &str) {
  *this = *this % BigInt10{str};
  return *this;
}

template <typename T, typename> BigInt10 &BigInt10::operator%=(const T val) {
  *this = *this % BigInt10{val};
  return *this;
}

// ------------------------------------------------------------------------------
// Unary operators

inline BigInt10 &BigInt10::operator-() {
  _Sign10 = (_Sign10 == Sign10::positive ? Sign10::negative : Sign10::positive);
  return *this;
}

inline BigInt10 BigInt10::operator-() const {
  BigInt10 tmp = *this;
  tmp._Sign10 =
      (tmp._Sign10 == Sign10::positive ? Sign10::negative : Sign10::positive);
  return tmp;
}

//------------------------------------------------------------------------------
// Increment decrement operators

inline BigInt10 &BigInt10::operator++() { // NOLINT(*-no-recursion)
  if (_Sign10 == Sign10::negative) {
    // -X + 1 = -(X - 1) Use `operator--()` on the absolute value
    *this = - -- -*this;
    return *this;
  }
  std::size_t i{0};
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

inline BigInt10 &BigInt10::operator--() { // NOLINT(*-no-recursion)
  if (_Sign10 == Sign10::negative) {
    // -X - 1 = -(X + 1) Use `operator++()` on the absolute value
    *this = -++-*this;
    return *this;
  }

  // Normal case: subtract 1 from a positive BigInt10
  std::size_t i = 0;
  while (i < _data.size()) {
    if (_data[i] > 0) {
      --_data[i]; // No borrow needed
      break;
    }
    _data[i++] = 9; // Borrow needed
  }
  if (_data.back() == 0 && _data.size() != 1) {
    _data.pop_back();
  }
  return *this;
}

//------------------------------------------------------------------------------
// Stream operators

inline std::ostream &operator<<(std::ostream &os, const BigInt10 &b) {
  os << b.to_string();
  return os;
}

//------------------------------------------------------------------------------
// Member functions

inline void BigInt10::normalize() {
  while (_data.size() > 1 && _data.back() == 0) {
    _data.pop_back();
  }
  if (_data.empty() || _data == std::vector{uint8_t{0}}) {
    _Sign10 = Sign10::positive;
  }
}

/**
 * @return Base-10 string representation
 */
inline std::string BigInt10::to_string() const {
  if (_data.empty()) {
    return std::string{"0"};
  }
  std::string str;
  str.reserve(_data.size());
  if (_Sign10 == Sign10::negative) {
    str.push_back('-');
  }
  for (auto it = _data.rbegin(); it != _data.rend(); ++it) {
    str.push_back(static_cast<char>(*it + '0'));
  }
  return str;
}

} // namespace sch

#endif // SCH_INCLUDE_BigInt10_HPP_
