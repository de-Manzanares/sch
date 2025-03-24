#ifndef SCH_TEST_BIGINT_HELPERS_CXX_
#define SCH_TEST_BIGINT_HELPERS_CXX_

#include <catch2/catch_all.hpp>
#include <cmath>
#include <limits>
#include <random>

namespace BigInt_test {

/// one less than the number of digits in LONG_LONG_MAX
const size_t LL_FULL_LENGTH =
    std::log(std::numeric_limits<long long>::max()) / std::log(10); // NOLINT

/// half of one less than the number of digits in LONG_LONG_MAX
const size_t LL_HALF_LENGTH = LL_FULL_LENGTH / 2;

std::ranlux48_base rand_engine{std::random_device{}()};

/**
 * @param lb lower bound
 * @param ub upper bound
 * @return random integer in range [lb,ub]
 */
size_t randomInRange(const size_t lb, const size_t ub) {
  return std::uniform_int_distribution{lb, ub}(rand_engine);
}

/**
 * @return random digit in range [0,9]
 */
char randomBase10digit() { return randomInRange(0, 9) + '0'; } // NOLINT

/**
 * @param lb string length lower bound
 * @param ub string length upper bound
 * @return A string of numbers of length L, such that lb <= L <=ub
 */
std::string randomString(const size_t lb, const size_t ub) {
  std::string str;
  const size_t length = randomInRange(lb, ub);
  for (size_t i = 0; i < length; ++i) {
    str += randomBase10digit();
  }
  return str;
}

/**
 * @param lb cstring length lower bound
 * @param ub cstring length upper bound
 * @return A cstring of numbers of length L, such that lb <= L <=ub
 */
char *randomCString(const size_t lb, const size_t ub) {
  const size_t length = randomInRange(lb, ub);
  char *str = new char[length + 1];
  for (size_t i = 0; i < length; ++i) {
    str[i] = randomBase10digit();
  }
  str[length] = '\0';
  return str;
}

/**
 * @param[in,out] str The string subject to modification
 * @note 50% chance to prepend '-' to the string
 */
void randomizeSign(std::string &str) {
  if (randomInRange(0, 9) % 2 == 0) {
    str.insert(0, 1, '-');
  }
}

/**
 * @param[in,out] cstr The cstring subject to modification
 * @note 50% chance to replace the first character with '-'
 */
void randomizeSign(char *cstr) {
  if (randomInRange(0, 9) % 2 == 0) {
    cstr[0] = '-';
  }
}

void removeLeadingZeros(std::string &str) {
  str.erase(0, str.find_first_not_of('0'));
  if (str == "") {
    str = std::to_string(randomInRange(1, 9));
  }
}

void removeLeadingZeros(char *cstr) {
  std::size_t i{};
  while (cstr[i] == '0') {
    ++i;
  }
  for (std::size_t j = 0; j < i; ++j) {
    cstr[j] = randomInRange(1, 9) + '0';
  }
}

} // namespace BigInt_test

#endif // SCH_TEST_BIGINT_HELPERS_CXX_
