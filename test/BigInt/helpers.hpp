#ifndef SCH_TEST_BIGINT_HELPERS_HPP_
#define SCH_TEST_BIGINT_HELPERS_HPP_

#include <catch2/catch_all.hpp>
#include <cmath>
#include <limits>
#include <random>

namespace big_int_test {

/// one less than the number of digits in LONG_LONG_MAX
const std::size_t LL_FULL_LENGTH =
    std::log(std::numeric_limits<long long>::max()) / std::log(10); // NOLINT

/// half of one less than the number of digits in LONG_LONG_MAX
[[maybe_unused]] const std::size_t LL_HALF_LENGTH = LL_FULL_LENGTH / 2;

/**
 * @param low_b lower bound
 * @param up_b upper bound
 * @return random integer in range [low_b, up_b]
 * @note rand_engine is thread_local to avoid constructing a new engine each
 * time this function is called
 */
template <typename T = std::size_t,
          typename = std::enable_if_t<std::is_integral_v<T>>>
auto random_in_range(const std::size_t low_b = 0, const std::size_t up_b = 9)
    -> T {
  thread_local std::ranlux48_base rand_engine{std::random_device{}()};
  auto dist = std::uniform_int_distribution<T>{static_cast<T>(low_b),
                                               static_cast<T>(up_b)};
  return dist(rand_engine);
}

/// @return random digit in range [0,9]
inline auto random_base_10_digit() -> char {
  return random_in_range<char>('0', '9');
}

/**
 * @param low_b string length lower bound
 * @param up_b string length upper bound
 * @return A string of numbers of length L, such that low_b <= L <=up_b
 */
inline auto random_string(const std::size_t low_b, const std::size_t up_b)
    -> std::string {

  std::string str;
  const std::size_t length = random_in_range(low_b, up_b);
  for (std::size_t i = 0; i < length; ++i) {
    str += random_base_10_digit();
  }
  return str;
}

/**
 * @param[in,out] str The string subject to modification
 * @note 50% chance to prepend '-' to the string
 */
inline void randomize_sign(std::string &str) {
  if (random_in_range() % 2 == 0) {
    str.insert(0, 1, '-');
  }
}

inline void remove_leading_zeros(std::string &str) {
  str.erase(0, str.find_first_not_of('0'));
  if (str.empty()) {
    str = random_in_range<char>('1', '9');
  }
}

/**
 * @param low_b c-string length lower bound
 * @param up_b c-string length upper bound
 * @return A c-string of numbers of length L, such that low_b <= L <=up_b
 */
[[maybe_unused]] inline auto random_c_string(const std::size_t low_b,
                                             const std::size_t up_b) -> char * {

  const auto length = random_in_range<std::ptrdiff_t>(low_b, up_b);
  auto *c_str = new char[length + 1];

  for (std::ptrdiff_t i = 0; i < length; ++i) {
    *std::next(c_str, i) = random_base_10_digit();
  }
  *std::next(c_str, length) = '\0';
  return c_str;
}

/**
 * @param[in,out] c_str The c-string subject to modification
 * @note 50% chance to replace the first character with '-'
 */
[[maybe_unused]] inline void randomize_sign(char *c_str) {
  if (random_in_range() % 2 == 0) {
    *c_str = '-';
  }
}

[[maybe_unused]] inline void remove_leading_zeros(char *c_str) {
  std::ptrdiff_t leading_zeros{};
  while (*std::next(c_str, leading_zeros) == '0') {
    ++leading_zeros;
  }
  for (std::ptrdiff_t j = 0; j < leading_zeros; ++j) {
    *std::next(c_str, j) = random_in_range<char>('1', '9');
  }
}

} // namespace big_int_test

#endif // SCH_TEST_BIGINT_HELPERS_HPP_
