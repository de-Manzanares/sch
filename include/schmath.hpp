
/*
 * Copyright (c) 2025 Drake Manzanares
 * Distributed under the MIT License.
 */

#ifndef SCH_INCLUDE_SCH_ALL_HPP_
#define SCH_INCLUDE_SCH_ALL_HPP_

#include "BigInt10.hpp"
#include "sign.h"

namespace sch {

/**
 * @tparam T A built-in integral type (signed or unsigned).
 *           Must be non-negative when calling this function.
 * @param base The base value (x in x^y).
 * @param exp  The exponent value (y in x^y).
 * @return The result of x^y as a BigInt10.
 * @throws std::invalid_argument if `exp` is negative.
 */
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt10 pow(const BigInt10 &base, const T exp) {
  if (exp < 0) {
    throw(std::invalid_argument("BigInt10::pow() : negative exponent"));
  }
  if (exp == 0) { // precedes the next check because 0^0 == 1
    return 1;
  }
  if (base == BigInt10{0}) {
    return 0;
  }

  BigInt10 m_base = base;                     // mutable copy
  auto m_exp = static_cast<std::size_t>(exp); // mutable copy
  BigInt10 res{1};                            // result

  while (m_exp > 0) {
    if (m_exp % 2 == 1) {
      res *= m_base;
    }
    m_base *= m_base;
    m_exp /= 2;
  }
  return res;
}

} // namespace sch

#endif // SCH_INCLUDE_SCH_ALL_HPP_
