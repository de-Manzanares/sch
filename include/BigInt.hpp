
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
  static constexpr std::uint64_t EXP = 63;
  static constexpr std::uint64_t BASE = 1ULL << EXP; ///< base 2^EXP

  // private variables
  sign _sign = sign::positive;          ///< sign of the number
  std::vector<std::uint64_t> _digits{}; ///< @note little endian order
};

// CONSTRUCTORS ----------------------------------------------------------------

inline BigInt::BigInt(const std::string &str) {
  BigInt10 bstr{str};
  if (bstr._sign == sign::negative) {
    this->_sign = sign::negative;
  }
  bstr._sign = sign::positive;
  const BigInt10 base{BASE};
  while (bstr > base) {
    auto [quotient, remainder] = BigInt10::longDivision(bstr, BASE);
    _digits.push_back(std::stoull((remainder).to_string()));
    bstr = quotient;
  }
  _digits.push_back(std::stoull((bstr).to_string()));
}

inline std::string BigInt::to_string() const {
  BigInt10 b10;
  std::string str;
  for (int i = 0; i < _digits.size(); ++i) {
    b10 += _digits[i] * pow(pow(2, EXP), i);
  }
  if (_sign == sign::negative) {
    str.push_back('-');
  }
  str += b10.to_string();
  return str;
}

inline std::ostream &operator<<(std::ostream &os, const BigInt &b) {
  os << b.to_string();
  return os;
}

} // namespace sch

#endif // SCH_INCLUDE_BigInt_HPP_
