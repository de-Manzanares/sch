/*
 * Copyright (c) 2025 Drake Manzanares
 * Distributed under the MIT License.
 */

#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

#include "BigInt.hpp"

namespace BigInt_test {

//------------------------------------------------------------------------------

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
 * @param[in,out] str The string subject to modification
 * @note 50% chance to prepend '-' to the string
 */
void randomizeSign(std::string &str) {
  if (randomInRange(0, 9) % 2 == 0) {
    str.insert(0, 1, '-');
  }
}

void removeLeadingZeros(std::string &str) {
  str.erase(0, str.find_first_not_of('0'));
  if (str == "") {
    str = std::to_string(randomInRange(1, 9));
  }
}

//------------------------------------------------------------------------------

TEST_CASE("constructor and stream insertion") {
  SECTION("positive") {
    for (int i = 0; i < 1000; ++i) {
      std::ostringstream oss[2];
      std::string str = randomString(10000, 20000);
      removeLeadingZeros(str);
      sch::BigInt bint{str};
      oss[0] << str;
      oss[1] << bint;
      CHECK(oss[0].str() == oss[1].str());
    }
  }
  SECTION("negative") {
    for (int i = 0; i < 1000; ++i) {
      std::ostringstream oss[2];
      std::string str = randomString(10000, 20000);
      removeLeadingZeros(str);
      str.insert(0, 1, '-');
      sch::BigInt bint{str};
      oss[0] << str;
      oss[1] << bint;
      CHECK(oss[0].str() == oss[1].str());
    }
  }
  SECTION("0") {
    std::string strP = "0";
    std::string strN = "-0";
    sch::BigInt bintP{strP};
    sch::BigInt bintN{strN};
    CHECK(strP == bintP.to_string());
    CHECK(strN.substr(1) == bintN.to_string());
  }
}

TEST_CASE("comparison operators") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt bint[2];
    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_FULL_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      bint[k] = n[k];
    }
    // clang-format off
    CHECK((n[0] == n[1]) == (bint[0] == bint[1]));
    CHECK((n[0] != n[1]) == (bint[0] != bint[1]));
    CHECK((n[0] <  n[1]) == (bint[0] <  bint[1]));
    CHECK((n[0] >  n[1]) == (bint[0] >  bint[1]));
    CHECK((n[0] <= n[1]) == (bint[0] <= bint[1]));
    CHECK((n[0] >= n[1]) == (bint[0] >= bint[1]));
    // clang-format on
  }
}

TEST_CASE("operator + : addition") {
  for (int i = 0; i < 1000; ++i) {
    sch::BigInt10 bint10[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      const std::string str = randomString(1000, 2000);
      // randomizeSign(str);
      bint10[k] = str;
      bint[k] = str;
    }
    os[0] << bint10[0] + bint10[1];
    os[1] << bint[0] + bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("operator - : subtraction") {
  for (int i = 0; i < 1000; ++i) {
    sch::BigInt10 bint10[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      const std::string str = randomString(1000, 2000);
      // randomizeSign(str);
      bint10[k] = str;
      bint[k] = str;
    }
    os[0] << bint10[0] - bint10[1];
    os[1] << bint[0] - bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

/*

TEST_CASE("operator: * multiplication") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_HALF_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      bint[k] = n[k];
    }
    os[0] << n[0] * n[1];
    os[1] << bint[0] * bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("operator: / division") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_FULL_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      if (n[k] == 0) {
        n[k] = randomInRange(1, 9); // NOLINT
      }
      bint[k] = n[k];
    }
    os[0] << n[0] / n[1];
    os[1] << bint[0] / bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("operator: % modulo") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_FULL_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      if (n[k] == 0) {
        n[k] = randomInRange(1, 9); // NOLINT
      }
      bint[k] = n[k];
    }
    os[0] << n[0] % n[1];
    os[1] << bint[0] % bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

// TODO consider sign
TEST_CASE("sch::pow()") {
  SKIP();
  for (int i = 0; i < 11; ++i) {
    for (int j = 0; j < 11; ++j) {
      sch::BigInt a{i};
      std::ostringstream os1;
      std::ostringstream os2;
      os1 << static_cast<unsigned long long>(std::pow(i, j));
      // os2 << sch::pow(a, j);
      CHECK(os1.str() == os2.str());
    }
  }
}

TEST_CASE("counting to 1 million") {
  // todo fix sign change on increment
  sch::BigInt n{0};
  for (size_t i = 0; i < 1E6; ++i) { // NOLINT
    ++n;
  }
  CHECK(n == 1000000);
  n = -1000001;
  for (size_t i = 0; i < 1E6; ++i) { // NOLINT
    ++n;
  }
  CHECK(n == sch::BigInt{-1});
}

TEST_CASE("counting from 1 million") {
  // todo fix sign change on decrement
  sch::BigInt n{1000000};
  for (size_t i = 1E6; i > 0; --i) {
    --n;
  }
  CHECK(n == sch::BigInt{0});
  n = -1;
  for (size_t i = 1E6; i > 0; --i) {
    --n;
  }
  CHECK(n == -1000001);
}

*/

} // namespace BigInt_test
