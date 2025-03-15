#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

#include "BigInt10.hpp"

namespace BigInt10_test {

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

//------------------------------------------------------------------------------

TEST_CASE("constructor") {
  SECTION("positive") {
    for (int i = 0; i < 100; ++i) {
      std::string str = randomString(1, 10'000);
      sch::BigInt10 bint{str};
    }
  }
  SECTION("negative") {
    for (int i = 0; i < 100; ++i) {
      std::string str = randomString(1, 10'000);
      str.insert(0, 1, '-');
      sch::BigInt10 bint{str};
    }
  }
}

TEST_CASE("comparison operators") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt10 bint[2];
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

TEST_CASE("operator << : stream extraction") {
  for (int i = 0; i < 100; ++i) {
    std::ostringstream os;
    std::string str = randomString(1, 10'000);
    size_t str_index = str.find_first_not_of('0');
    randomizeSign(str);
    if (str[0] == '-') {
      ++str_index;
    }
    sch::BigInt10 bint{str};
    os << bint;
    // ternary is to maintain the '-' while dodging leading zeros, should
    // the character be present
    CHECK(os.str() == (str[0] == '-' ? '-' + str.substr(str_index)
                                     : str.substr(str_index)));
  }
}

TEST_CASE("operator + : addition") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt10 bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_FULL_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      bint[k] = n[k];
    }
    os[0] << n[0] + n[1];
    os[1] << bint[0] + bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("operator - : subtraction") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt10 bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1, LL_FULL_LENGTH);
      randomizeSign(str);
      n[k] = std::stoll(str);
      bint[k] = n[k];
    }
    os[0] << n[0] - n[1];
    os[1] << bint[0] - bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("operator: * multiplication") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt10 bint[2];
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
    sch::BigInt10 bint[2];
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
    sch::BigInt10 bint[2];
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
  for (int i = 0; i < 11; ++i) {
    for (int j = 0; j < 11; ++j) {
      sch::BigInt10 a{i};
      std::ostringstream os1;
      std::ostringstream os2;
      os1 << static_cast<unsigned long long>(std::pow(i, j));
      os2 << sch::pow(a, j);
      CHECK(os1.str() == os2.str());
    }
  }
}

TEST_CASE("counting to 1 million") {
  // todo fix sign change on increment
  sch::BigInt10 n{0};
  for (size_t i = 0; i < 1E6; ++i) { // NOLINT
    ++n;
  }
  CHECK(n == 1000000);
  n = -1000001;
  for (size_t i = 0; i < 1E6; ++i) { // NOLINT
    ++n;
  }
  CHECK(n == sch::BigInt10{-1});
}

TEST_CASE("counting from 1 million") {
  // todo fix sign change on decrement
  sch::BigInt10 n{1000000};
  for (size_t i = 1E6; i > 0; --i) {
    --n;
  }
  CHECK(n == sch::BigInt10{0});
  n = -1;
  for (size_t i = 1E6; i > 0; --i) {
    --n;
  }
  CHECK(n == -1000001);
}

} // namespace BigInt10_test
