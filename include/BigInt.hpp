#ifndef SCH_INCLUDE_BIGINT_HPP_
#define SCH_INCLUDE_BIGINT_HPP_

#include "sign.h"

#include <cstdint>
#include <string>
#include <vector>

namespace sch {

static constexpr uint64_t BASE = (1ULL << 32); // 2^32

class BigInt {
 public:
  BigInt() = default;
  explicit BigInt(const std::string &str);

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
      quotient.push_back((current / BASE) + '0');
      remainder = current % BASE;
    }

    // Store remainder as the next base-2^32 digit
    _digits.push_back(remainder);

    // Remove leading zeros in quotient
    size_t firstNonZero = quotient.find_first_not_of('0');
    if (firstNonZero != std::string::npos)
      _str = quotient.substr(firstNonZero);
    else
      _str = "0"; // Ensure loop terminates
  }
}

} // namespace sch

#endif // SCH_INCLUDE_BIGINT_HPP_
