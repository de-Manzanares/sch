
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
  static constexpr std::uint64_t EXP = 9; // 10^EXP
  static constexpr std::uint64_t BASE = 1'000'000'000;

  // private variables
  sign _sign = sign::positive;          ///< sign of the number
  std::vector<std::uint64_t> _digits{}; ///< @note little endian order
};

// CONSTRUCTORS ----------------------------------------------------------------

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

inline std::ostream &operator<<(std::ostream &os, const BigInt &b) {
  os << b.to_string();
  return os;
}

} // namespace sch

#endif // SCH_INCLUDE_BigInt_HPP_
