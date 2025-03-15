
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
  static constexpr uint64_t BASE = 10; ///< data is stored in base 10

  // private variables
  sign _sign = sign::positive;  ///< sign of the number
  std::vector<uint8_t> _data{}; ///< @note little endian order

  // Addition operator helpers ---------------------------------
  static void add(std::size_t &it_lhs, const BigInt &lhs, std::size_t &it_rhs,
                  const BigInt &rhs, bool &carry, BigInt &sum);
  static void a_carryDown(std::size_t &it, const BigInt &bint_8, bool &carry,
                          BigInt &sum);

  // Subtraction operator helpers ------------------------------
  static void subtract(std::size_t &it_lhs, BigInt &lhs, std::size_t &it_rhs,
                       const BigInt &rhs, BigInt &difference);
  static void s_carryDown(std::size_t &it, const BigInt &bint_8,
                          BigInt &difference);

  // Multiplication operator -----------------------------------
  static BigInt longMultiplication(const BigInt &bottom, const BigInt &top);

  // Division operator -----------------------------------------
  static std::pair<BigInt, BigInt> longDivision(const BigInt &dividend,
                                                const BigInt &divisor);
};

} // namespace sch

#endif // SCH_INCLUDE_BigInt_HPP_
