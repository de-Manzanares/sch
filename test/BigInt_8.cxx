#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <random>

#include "BigInt_8.hpp"

namespace BigInt_8_test {

//------------------------------------------------------------------------------

/// one less than the number of digits in LONG_LONG_MAX
const size_t LL_SUM_DIF_TEST_LENGTH =
    std::log(LONG_LONG_MAX) / std::log(10); // NOLINT

/// half of one less than the number of digits in LONG_LONG_MAX
const size_t LL_PRODUCT_TEST_LENGTH = LL_SUM_DIF_TEST_LENGTH / 2;

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
 * @param[in,out] str The string subject to modification
 * @note 50% chance to prepend '-' to the string
 */
void randomizeSign(std::string &str) {
  if (randomInRange(0, 9) % 2 == 0) {
    str.insert(0, 1, '-');
  }
}

//------------------------------------------------------------------------------

TEST_CASE("constructor") {
  SECTION("positive") {
    for (int i = 0; i < 100; ++i) {
      std::string str = randomString(1, 10'000);
      sch::BigInt_8 bint8{str};
    }
  }
  SECTION("negative") {
    for (int i = 0; i < 100; ++i) {
      std::string str = randomString(1, 10'000);
      str.insert(0, 1, '-');
      sch::BigInt_8 bint8{str};
    }
  }
}

TEST_CASE("comparison operators") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt_8 bint8[2];
    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_SUM_DIF_TEST_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      bint8[k] = n[k];
    }
    // clang-format off
    CHECK((n[0] == n[1]) == (bint8[0] == bint8[1]));
    CHECK((n[0] != n[1]) == (bint8[0] != bint8[1]));
    CHECK((n[0] <  n[1]) == (bint8[0] <  bint8[1]));
    CHECK((n[0] >  n[1]) == (bint8[0] >  bint8[1]));
    CHECK((n[0] <= n[1]) == (bint8[0] <= bint8[1]));
    CHECK((n[0] >= n[1]) == (bint8[0] >= bint8[1]));
    // clang-format on
  }
}

TEST_CASE("operator << : stream extraction") {
  for (int i = 0; i < 100; ++i) {
    std::ostringstream os;
    std::string str = randomString(1, 10'000);
    size_t str_index = str.find_first_not_of('0');
    randomizeSign(str);
    if (str[0] == '-') {
      ++str_index;
    }
    sch::BigInt_8 bint8{str};
    os << bint8;
    // ternary is to maintain the '-' while dodging leading zeros, should
    // the character be present
    CHECK(os.str() == (str[0] == '-' ? '-' + str.substr(str_index)
                                     : str.substr(str_index)));
  }
}

TEST_CASE("operator + : addition") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt_8 bint8[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_SUM_DIF_TEST_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      bint8[k] = n[k];
    }
    os[0] << n[0] + n[1];
    os[1] << bint8[0] + bint8[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("operator - : subtraction") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt_8 bint8[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_SUM_DIF_TEST_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      bint8[k] = n[k];
    }
    os[0] << n[0] - n[1];
    os[1] << bint8[0] - bint8[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("operator: * multiplication") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt_8 bint8[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_PRODUCT_TEST_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      bint8[k] = n[k];
    }
    os[0] << n[0] * n[1];
    os[1] << bint8[0] * bint8[1];
    CHECK(os[0].str() == os[1].str());
  }
}

// TODO consider sign
TEST_CASE("sch::pow()") {
  for (uint i = 0; i < 11; ++i) {
    for (uint j = 0; j < 11; ++j) {
      sch::BigInt_8 a{i};
      std::ostringstream os1;
      std::ostringstream os2;
      os1 << static_cast<unsigned long long>(std::pow(i, j));
      os2 << sch::pow(a, j);
      CHECK(os1.str() == os2.str());
    }
  }
}

TEST_CASE("counting to 1 million") {
  sch::BigInt_8 n{0};
  for (size_t i = 0; i < 1E6; ++i) { // NOLINT
    ++n;
  }
}

TEST_CASE("counting from 1 million") {
  sch::BigInt_8 n{100000};
  for (size_t i = 1E6; i > 0; --i) {
    --n;
  }
}

} // namespace BigInt_8_test
