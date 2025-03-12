#ifndef SCH_INCLUDE_BIGINT_HPP_
#define SCH_INCLUDE_BIGINT_HPP_

#include "BigInt_8.hpp"
#include "sign.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace sch {

static constexpr uint64_t BASE = (1ULL << 32); // 2^32

class BigInt {
 public:
  BigInt() = default;
  explicit BigInt(const std::string &str);
  explicit BigInt(const std::vector<uint64_t> &vec);
  void normalize();
  friend std::ostream &operator<<(std::ostream &os, const BigInt &bint);

 private:
  sign _sign = sign::positive;
  std::vector<uint64_t> _digits{}; // little endian order
};

//------------------------------------------------------------------------------
// Constructor

inline BigInt::BigInt(const std::string &str) {

  std::string _str = str;

  while (!_str.empty() && _str != "0") {
    uint64_t remainder = 0;
    std::string quotient;

    // Perform manual division of _str by BASE
    for (const char c : _str) {
      const uint64_t current = remainder * 10 + (c - '0');
      // (*-narrowing-conversions)
      quotient.push_back((current / BASE) + '0'); // NOLINT
      remainder = current % BASE;
    }

    // Store remainder as the next base-2^32 digit
    _digits.push_back(remainder);

    // Remove leading zeros in quotient
    if (const size_t firstNonZero = quotient.find_first_not_of('0');
        firstNonZero != std::string::npos)
      _str = quotient.substr(firstNonZero);
    else
      _str = "0"; // Ensure loop terminates
  }
}
inline BigInt::BigInt(const std::vector<uint64_t> &vec) {
  for (const auto &digit : vec) {
    _digits.push_back(digit);
  }
}

//------------------------------------------------------------------------------
// Member Functions

inline void BigInt::normalize() {
  while (!_digits.empty() && _digits.back() == 0) {
    _digits.pop_back();
  }
}

//------------------------------------------------------------------------------
// Friend Functions
// Stream operators

inline std::ostream &operator<<(std::ostream &os, const BigInt &bint) {

  std::vector<BigInt_8> bint_8;
  bint_8.reserve(bint._digits.size());

  for (const auto &digit : bint._digits) {
    bint_8.emplace_back(std::to_string(digit));
  }
  for (size_t i = 0; i < bint_8.size(); i++) {
    bint_8[i] = bint_8[i] * pow(pow(BigInt_8{"2"}, 32), i);
  }
  BigInt_8 res;
  for (const auto &num : bint_8) {
    res += num;
  }
  os << res;

  return os;
}

} // namespace sch

#endif // SCH_INCLUDE_BIGINT_HPP_
