/*
 * Copyright (c) 2025 Drake Manzanares
 * Distributed under the MIT License.
 */

#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <variant>

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
      std::string str = randomString(10000, 20000);
      randomizeSign(str);
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
      std::string str = randomString(10000, 20000);
      randomizeSign(str);
      bint10[k] = str;
      bint[k] = str;
    }
    os[0] << bint10[0] - bint10[1];
    os[1] << bint[0] - bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("operator: * multiplication") {
  for (int i = 0; i < 50; ++i) {
    sch::BigInt10 n[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = randomString(1000, 2000);
      randomizeSign(str);
      n[k] = str;
      bint[k] = str;
    }
    os[0] << n[0] * n[1];
    os[1] << bint[0] * bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

// todo special cases -- 1's 0's etc.
TEST_CASE("templated operators") {
  SECTION("std::string") {
    for (int i = 0; i < 1000; ++i) {
      std::string str[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        str[k] = randomString(1, 200);
        removeLeadingZeros(str[k]);
        randomizeSign(str[k]);
        bint[k] = str[k];
      }
      // clang-format off
      CHECK((sch::BigInt10{str[0]} == sch::BigInt10{str[1]}) == (bint[0] == str[1]));
      CHECK((sch::BigInt10{str[0]} == sch::BigInt10{str[1]}) == (str[0] == bint[1]));
      CHECK((sch::BigInt10{str[0]} != sch::BigInt10{str[1]}) == (bint[0] != str[1]));
      CHECK((sch::BigInt10{str[0]} != sch::BigInt10{str[1]}) == (str[0] != bint[1]));
      CHECK((sch::BigInt10{str[0]} <  sch::BigInt10{str[1]}) == (bint[0] < str[1]));
      CHECK((sch::BigInt10{str[0]} <  sch::BigInt10{str[1]}) == (str[0] < bint[1]));
      CHECK((sch::BigInt10{str[0]} >  sch::BigInt10{str[1]}) == (bint[0] > str[1]));
      CHECK((sch::BigInt10{str[0]} >  sch::BigInt10{str[1]}) == (str[0] > bint[1]));
      CHECK((sch::BigInt10{str[0]} <= sch::BigInt10{str[1]}) == (bint[0] <= str[1]));
      CHECK((sch::BigInt10{str[0]} <= sch::BigInt10{str[1]}) == (str[0] <= bint[1]));
      CHECK((sch::BigInt10{str[0]} >= sch::BigInt10{str[1]}) == (bint[0] >= str[1]));
      CHECK((sch::BigInt10{str[0]} >= sch::BigInt10{str[1]}) == (str[0] >= bint[1]));
      CHECK((sch::BigInt10{str[0]} +  sch::BigInt10{str[1]}).to_string() == bint[0] + str[1]);
      CHECK((sch::BigInt10{str[0]} +  sch::BigInt10{str[1]}).to_string() == str[0] + bint[1]);
      CHECK((sch::BigInt10{str[0]} -  sch::BigInt10{str[1]}).to_string() == bint[0] - str[1]);
      CHECK((sch::BigInt10{str[0]} -  sch::BigInt10{str[1]}).to_string() == str[0] - bint[1]);
      CHECK((sch::BigInt10{str[0]} *  sch::BigInt10{str[1]}).to_string() == bint[0] * str[1]);
      CHECK((sch::BigInt10{str[0]} *  sch::BigInt10{str[1]}).to_string() == str[0] * bint[1]);
      // clang-format on
    }
  }
  SECTION("c-string") {
    for (int i = 0; i < 1; ++i) {
      sch::BigInt bint[2];
      bint[0] = "123456789";
      bint[1] = "987654321";
      // clang-format off
      CHECK((sch::BigInt10{"123456789"} == sch::BigInt10{"987654321"}) == (bint[0] == "987654321"));
      CHECK((sch::BigInt10{"123456789"} == sch::BigInt10{"987654321"}) == ("123456789" == bint[1]));
      CHECK((sch::BigInt10{"123456789"} != sch::BigInt10{"987654321"}) == (bint[0] != "987654321"));
      CHECK((sch::BigInt10{"123456789"} != sch::BigInt10{"987654321"}) == ("123456789" != bint[1]));
      CHECK((sch::BigInt10{"123456789"} <  sch::BigInt10{"987654321"}) == (bint[0] < "987654321"));
      CHECK((sch::BigInt10{"123456789"} <  sch::BigInt10{"987654321"}) == ("123456789" < bint[1]));
      CHECK((sch::BigInt10{"123456789"} >  sch::BigInt10{"987654321"}) == (bint[0] > "987654321"));
      CHECK((sch::BigInt10{"123456789"} >  sch::BigInt10{"987654321"}) == ("123456789" > bint[1]));
      CHECK((sch::BigInt10{"123456789"} <= sch::BigInt10{"987654321"}) == (bint[0] <= "987654321"));
      CHECK((sch::BigInt10{"123456789"} <= sch::BigInt10{"987654321"}) == ("123456789" <= bint[1]));
      CHECK((sch::BigInt10{"123456789"} >= sch::BigInt10{"987654321"}) == (bint[0] >= "987654321"));
      CHECK((sch::BigInt10{"123456789"} >= sch::BigInt10{"987654321"}) == ("123456789" >= bint[1]));
      CHECK((sch::BigInt10{"123456789"} +  sch::BigInt10{"987654321"}).to_string() == bint[0] + "987654321");
      CHECK((sch::BigInt10{"123456789"} +  sch::BigInt10{"987654321"}).to_string() == "123456789" + bint[1]);
      CHECK((sch::BigInt10{"123456789"} -  sch::BigInt10{"987654321"}).to_string() == bint[0] - "987654321");
      CHECK((sch::BigInt10{"123456789"} -  sch::BigInt10{"987654321"}).to_string() == "123456789" - bint[1]);
      CHECK((sch::BigInt10{"123456789"} *  sch::BigInt10{"987654321"}).to_string() == bint[0] * "987654321");
      CHECK((sch::BigInt10{"123456789"} *  sch::BigInt10{"987654321"}).to_string() == "123456789" * bint[1]);
      // clang-format on
    }
  }
  SECTION("std::string_view") {
    for (int i = 0; i < 1000; ++i) {
      std::string str[2];
      std::string_view strv[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        str[k] = randomString(1, 200);
        removeLeadingZeros(str[k]);
        randomizeSign(str[k]);
        strv[k] = str[k];
        bint[k] = str[k];
      }
      // clang-format off
      CHECK((sch::BigInt10{str[0]} == sch::BigInt10{str[1]}) == (bint[0] == strv[1]));
      CHECK((sch::BigInt10{str[0]} == sch::BigInt10{str[1]}) == (strv[0] == bint[1]));
      CHECK((sch::BigInt10{str[0]} != sch::BigInt10{str[1]}) == (bint[0] != str[1]));
      CHECK((sch::BigInt10{str[0]} != sch::BigInt10{str[1]}) == (strv[0] != bint[1]));
      CHECK((sch::BigInt10{str[0]} <  sch::BigInt10{str[1]}) == (bint[0] < str[1]));
      CHECK((sch::BigInt10{str[0]} <  sch::BigInt10{str[1]}) == (strv[0] < bint[1]));
      CHECK((sch::BigInt10{str[0]} >  sch::BigInt10{str[1]}) == (bint[0] > str[1]));
      CHECK((sch::BigInt10{str[0]} >  sch::BigInt10{str[1]}) == (strv[0] > bint[1]));
      CHECK((sch::BigInt10{str[0]} <= sch::BigInt10{str[1]}) == (bint[0] <= str[1]));
      CHECK((sch::BigInt10{str[0]} <= sch::BigInt10{str[1]}) == (strv[0] <= bint[1]));
      CHECK((sch::BigInt10{str[0]} >= sch::BigInt10{str[1]}) == (bint[0] >= str[1]));
      CHECK((sch::BigInt10{str[0]} >= sch::BigInt10{str[1]}) == (strv[0] >= bint[1]));
      CHECK((sch::BigInt10{str[0]} +  sch::BigInt10{str[1]}).to_string() == bint[0] + strv[1]);
      CHECK((sch::BigInt10{str[0]} +  sch::BigInt10{str[1]}).to_string() == strv[0] + bint[1]);
      CHECK((sch::BigInt10{str[0]} -  sch::BigInt10{str[1]}).to_string() == bint[0] - strv[1]);
      CHECK((sch::BigInt10{str[0]} -  sch::BigInt10{str[1]}).to_string() == strv[0] - bint[1]);
      CHECK((sch::BigInt10{str[0]} *  sch::BigInt10{str[1]}).to_string() == bint[0] * strv[1]);
      CHECK((sch::BigInt10{str[0]} *  sch::BigInt10{str[1]}).to_string() == strv[0] * bint[1]);
      // clang-format on
    }
  }
  SECTION("short int") {
    for (int i = 0; i < 5000; ++i) {
      short int shint[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        shint[k] = randomInRange(std::numeric_limits<short int>::min(),
                                 std::numeric_limits<short int>::max());
        bint[k] = shint[k];
      }
      // clang-format off
      CHECK((shint[0] == shint[1]) == (bint[0] == shint[1]));
      CHECK((shint[0] == shint[1]) == (shint[0] == bint[1]));
      CHECK((shint[0] != shint[1]) == (bint[0] != shint[1]));
      CHECK((shint[0] != shint[1]) == (shint[0] != bint[1]));
      CHECK((shint[0] <  shint[1]) == (bint[0] < shint[1]));
      CHECK((shint[0] <  shint[1]) == (shint[0] < bint[1]));
      CHECK((shint[0] >  shint[1]) == (bint[0] > shint[1]));
      CHECK((shint[0] >  shint[1]) == (shint[0] > bint[1]));
      CHECK((shint[0] <= shint[1]) == (bint[0] <= shint[1]));
      CHECK((shint[0] <= shint[1]) == (shint[0] <= bint[1]));
      CHECK((shint[0] >= shint[1]) == (bint[0] >= shint[1]));
      CHECK((shint[0] >= shint[1]) == (shint[0] >= bint[1]));
      CHECK((sch::BigInt10{shint[0]} + sch::BigInt10{shint[1]}).to_string() == bint[0] + shint[1]);
      CHECK((sch::BigInt10{shint[0]} + sch::BigInt10{shint[1]}).to_string() == shint[0] + bint[1]);
      CHECK((sch::BigInt10{shint[0]} - sch::BigInt10{shint[1]}).to_string() == bint[0] - shint[1]);
      CHECK((sch::BigInt10{shint[0]} - sch::BigInt10{shint[1]}).to_string() == shint[0] - bint[1]);
      CHECK((sch::BigInt10{shint[0]} * sch::BigInt10{shint[1]}).to_string() == bint[0] * shint[1]);
      CHECK((sch::BigInt10{shint[0]} * sch::BigInt10{shint[1]}).to_string() == shint[0] * bint[1]);
      // clang-format on
    }
  }
  SECTION("unsigned short int") {
    for (int i = 0; i < 2000; ++i) {
      unsigned short int ushint[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        ushint[k] =
            randomInRange(std::numeric_limits<unsigned short int>::min(),
                          std::numeric_limits<unsigned short int>::max());
        bint[k] = ushint[k];
      }
      // clang-format off
      CHECK((ushint[0] == ushint[1]) == (bint[0] == ushint[1]));
      CHECK((ushint[0] == ushint[1]) == (ushint[0] == bint[1]));
      CHECK((ushint[0] != ushint[1]) == (bint[0] != ushint[1]));
      CHECK((ushint[0] != ushint[1]) == (ushint[0] != bint[1]));
      CHECK((ushint[0] <  ushint[1]) == (bint[0] < ushint[1]));
      CHECK((ushint[0] <  ushint[1]) == (ushint[0] < bint[1]));
      CHECK((ushint[0] >  ushint[1]) == (bint[0] > ushint[1]));
      CHECK((ushint[0] >  ushint[1]) == (ushint[0] > bint[1]));
      CHECK((ushint[0] <= ushint[1]) == (bint[0] <= ushint[1]));
      CHECK((ushint[0] <= ushint[1]) == (ushint[0] <= bint[1]));
      CHECK((ushint[0] >= ushint[1]) == (bint[0] >= ushint[1]));
      CHECK((ushint[0] >= ushint[1]) == (ushint[0] >= bint[1]));
      CHECK((sch::BigInt10{ushint[0]} + sch::BigInt10{ushint[1]}).to_string() == bint[0] + ushint[1]);
      CHECK((sch::BigInt10{ushint[0]} + sch::BigInt10{ushint[1]}).to_string() == ushint[0] + bint[1]);
      CHECK((sch::BigInt10{ushint[0]} - sch::BigInt10{ushint[1]}).to_string() == bint[0] - ushint[1]);
      CHECK((sch::BigInt10{ushint[0]} - sch::BigInt10{ushint[1]}).to_string() == ushint[0] - bint[1]);
      CHECK((sch::BigInt10{ushint[0]} * sch::BigInt10{ushint[1]}).to_string() == bint[0] * ushint[1]);
      CHECK((sch::BigInt10{ushint[0]} * sch::BigInt10{ushint[1]}).to_string() == ushint[0] * bint[1]);
      // clang-format on
    }
  }
  SECTION("int") {
    for (int i = 0; i < 2000; ++i) {
      int nint[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        nint[k] = randomInRange(std::numeric_limits<int>::min(),
                                std::numeric_limits<int>::max());
        bint[k] = nint[k];
      }
      // clang-format off
      CHECK((nint[0] == nint[1]) == (bint[0] == nint[1]));
      CHECK((nint[0] == nint[1]) == (nint[0] == bint[1]));
      CHECK((nint[0] != nint[1]) == (bint[0] != nint[1]));
      CHECK((nint[0] != nint[1]) == (nint[0] != bint[1]));
      CHECK((nint[0] <  nint[1]) == (bint[0] < nint[1]));
      CHECK((nint[0] <  nint[1]) == (nint[0] < bint[1]));
      CHECK((nint[0] >  nint[1]) == (bint[0] > nint[1]));
      CHECK((nint[0] >  nint[1]) == (nint[0] > bint[1]));
      CHECK((nint[0] <= nint[1]) == (bint[0] <= nint[1]));
      CHECK((nint[0] <= nint[1]) == (nint[0] <= bint[1]));
      CHECK((nint[0] >= nint[1]) == (bint[0] >= nint[1]));
      CHECK((nint[0] >= nint[1]) == (nint[0] >= bint[1]));
      CHECK((sch::BigInt10{nint[0]} + sch::BigInt10{nint[1]}).to_string() == bint[0] + nint[1]);
      CHECK((sch::BigInt10{nint[0]} + sch::BigInt10{nint[1]}).to_string() == nint[0] + bint[1]);
      CHECK((sch::BigInt10{nint[0]} - sch::BigInt10{nint[1]}).to_string() == bint[0] - nint[1]);
      CHECK((sch::BigInt10{nint[0]} - sch::BigInt10{nint[1]}).to_string() == nint[0] - bint[1]);
      CHECK((sch::BigInt10{nint[0]} * sch::BigInt10{nint[1]}).to_string() == bint[0] * nint[1]);
      CHECK((sch::BigInt10{nint[0]} * sch::BigInt10{nint[1]}).to_string() == nint[0] * bint[1]);
      // clang-format on
    }
  }
  SECTION("unsigned int") {
    for (int i = 0; i < 2000; ++i) {
      unsigned int uint[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        uint[k] = randomInRange(std::numeric_limits<unsigned int>::min(),
                                std::numeric_limits<unsigned int>::max());
        bint[k] = uint[k];
      }
      // clang-format off
      CHECK((uint[0] == uint[1]) == (bint[0] == uint[1]));
      CHECK((uint[0] == uint[1]) == (uint[0] == bint[1]));
      CHECK((uint[0] != uint[1]) == (bint[0] != uint[1]));
      CHECK((uint[0] != uint[1]) == (uint[0] != bint[1]));
      CHECK((uint[0] <  uint[1]) == (bint[0] < uint[1]));
      CHECK((uint[0] <  uint[1]) == (uint[0] < bint[1]));
      CHECK((uint[0] >  uint[1]) == (bint[0] > uint[1]));
      CHECK((uint[0] >  uint[1]) == (uint[0] > bint[1]));
      CHECK((uint[0] <= uint[1]) == (bint[0] <= uint[1]));
      CHECK((uint[0] <= uint[1]) == (uint[0] <= bint[1]));
      CHECK((uint[0] >= uint[1]) == (bint[0] >= uint[1]));
      CHECK((uint[0] >= uint[1]) == (uint[0] >= bint[1]));
      CHECK((sch::BigInt10{uint[0]} + sch::BigInt10{uint[1]}).to_string() == bint[0] + uint[1]);
      CHECK((sch::BigInt10{uint[0]} + sch::BigInt10{uint[1]}).to_string() == uint[0] + bint[1]);
      CHECK((sch::BigInt10{uint[0]} - sch::BigInt10{uint[1]}).to_string() == bint[0] - uint[1]);
      CHECK((sch::BigInt10{uint[0]} - sch::BigInt10{uint[1]}).to_string() == uint[0] - bint[1]);
      CHECK((sch::BigInt10{uint[0]} * sch::BigInt10{uint[1]}).to_string() == bint[0] * uint[1]);
      CHECK((sch::BigInt10{uint[0]} * sch::BigInt10{uint[1]}).to_string() == uint[0] * bint[1]);
      // clang-format on
    }
  }
  SECTION("long int") {
    for (int i = 0; i < 2000; ++i) {
      long int lint[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        lint[k] = randomInRange(std::numeric_limits<long int>::min(),
                                std::numeric_limits<long int>::max());
        bint[k] = lint[k];
      }
      // clang-format off
      CHECK((lint[0] == lint[1]) == (bint[0] == lint[1]));
      CHECK((lint[0] == lint[1]) == (lint[0] == bint[1]));
      CHECK((lint[0] != lint[1]) == (bint[0] != lint[1]));
      CHECK((lint[0] != lint[1]) == (lint[0] != bint[1]));
      CHECK((lint[0] <  lint[1]) == (bint[0] < lint[1]));
      CHECK((lint[0] <  lint[1]) == (lint[0] < bint[1]));
      CHECK((lint[0] >  lint[1]) == (bint[0] > lint[1]));
      CHECK((lint[0] >  lint[1]) == (lint[0] > bint[1]));
      CHECK((lint[0] <= lint[1]) == (bint[0] <= lint[1]));
      CHECK((lint[0] <= lint[1]) == (lint[0] <= bint[1]));
      CHECK((lint[0] >= lint[1]) == (bint[0] >= lint[1]));
      CHECK((lint[0] >= lint[1]) == (lint[0] >= bint[1]));
      CHECK((sch::BigInt10{lint[0]} + sch::BigInt10{lint[1]}).to_string() == bint[0] + lint[1]);
      CHECK((sch::BigInt10{lint[0]} + sch::BigInt10{lint[1]}).to_string() == lint[0] + bint[1]);
      CHECK((sch::BigInt10{lint[0]} - sch::BigInt10{lint[1]}).to_string() == bint[0] - lint[1]);
      CHECK((sch::BigInt10{lint[0]} - sch::BigInt10{lint[1]}).to_string() == lint[0] - bint[1]);
      CHECK((sch::BigInt10{lint[0]} * sch::BigInt10{lint[1]}).to_string() == bint[0] * lint[1]);
      CHECK((sch::BigInt10{lint[0]} * sch::BigInt10{lint[1]}).to_string() == lint[0] * bint[1]);
      // clang-format on
    }
  }
  SECTION("unsigned long int") {
    for (int i = 0; i < 2000; ++i) {
      unsigned long int ulint[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        ulint[k] = randomInRange(std::numeric_limits<unsigned long int>::min(),
                                 std::numeric_limits<unsigned long int>::max());
        bint[k] = ulint[k];
      }
      // clang-format off
      CHECK((ulint[0] == ulint[1]) == (bint[0] == ulint[1]));
      CHECK((ulint[0] == ulint[1]) == (ulint[0] == bint[1]));
      CHECK((ulint[0] != ulint[1]) == (bint[0] != ulint[1]));
      CHECK((ulint[0] != ulint[1]) == (ulint[0] != bint[1]));
      CHECK((ulint[0] <  ulint[1]) == (bint[0] < ulint[1]));
      CHECK((ulint[0] <  ulint[1]) == (ulint[0] < bint[1]));
      CHECK((ulint[0] >  ulint[1]) == (bint[0] > ulint[1]));
      CHECK((ulint[0] >  ulint[1]) == (ulint[0] > bint[1]));
      CHECK((ulint[0] <= ulint[1]) == (bint[0] <= ulint[1]));
      CHECK((ulint[0] <= ulint[1]) == (ulint[0] <= bint[1]));
      CHECK((ulint[0] >= ulint[1]) == (bint[0] >= ulint[1]));
      CHECK((ulint[0] >= ulint[1]) == (ulint[0] >= bint[1]));
      CHECK((sch::BigInt10{ulint[0]} + sch::BigInt10{ulint[1]}).to_string() == bint[0] + ulint[1]);
      CHECK((sch::BigInt10{ulint[0]} + sch::BigInt10{ulint[1]}).to_string() == ulint[0] + bint[1]);
      CHECK((sch::BigInt10{ulint[0]} - sch::BigInt10{ulint[1]}).to_string() == bint[0] - ulint[1]);
      CHECK((sch::BigInt10{ulint[0]} - sch::BigInt10{ulint[1]}).to_string() == ulint[0] - bint[1]);
      CHECK((sch::BigInt10{ulint[0]} * sch::BigInt10{ulint[1]}).to_string() == bint[0] * ulint[1]);
      CHECK((sch::BigInt10{ulint[0]} * sch::BigInt10{ulint[1]}).to_string() == ulint[0] * bint[1]);
      // clang-format on
    }
  }
  SECTION("long long int") {
    for (int i = 0; i < 2000; ++i) {
      long long int llint[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        llint[k] = randomInRange(std::numeric_limits<long long int>::min(),
                                 std::numeric_limits<long long int>::max());
        bint[k] = llint[k];
      }
      // clang-format off
      CHECK((llint[0] == llint[1]) == (bint[0] == llint[1]));
      CHECK((llint[0] == llint[1]) == (llint[0] == bint[1]));
      CHECK((llint[0] != llint[1]) == (bint[0] != llint[1]));
      CHECK((llint[0] != llint[1]) == (llint[0] != bint[1]));
      CHECK((llint[0] <  llint[1]) == (bint[0] < llint[1]));
      CHECK((llint[0] <  llint[1]) == (llint[0] < bint[1]));
      CHECK((llint[0] >  llint[1]) == (bint[0] > llint[1]));
      CHECK((llint[0] >  llint[1]) == (llint[0] > bint[1]));
      CHECK((llint[0] <= llint[1]) == (bint[0] <= llint[1]));
      CHECK((llint[0] <= llint[1]) == (llint[0] <= bint[1]));
      CHECK((llint[0] >= llint[1]) == (bint[0] >= llint[1]));
      CHECK((llint[0] >= llint[1]) == (llint[0] >= bint[1]));
      CHECK((sch::BigInt10{llint[0]} + sch::BigInt10{llint[1]}).to_string() == bint[0] + llint[1]);
      CHECK((sch::BigInt10{llint[0]} + sch::BigInt10{llint[1]}).to_string() == llint[0] + bint[1]);
      CHECK((sch::BigInt10{llint[0]} - sch::BigInt10{llint[1]}).to_string() == bint[0] - llint[1]);
      CHECK((sch::BigInt10{llint[0]} - sch::BigInt10{llint[1]}).to_string() == llint[0] - bint[1]);
      CHECK((sch::BigInt10{llint[0]} * sch::BigInt10{llint[1]}).to_string() == bint[0] * llint[1]);
      CHECK((sch::BigInt10{llint[0]} * sch::BigInt10{llint[1]}).to_string() == llint[0] * bint[1]);
      // clang-format on
    }
  }
  SECTION("unsigned long long int") {
    for (int i = 0; i < 2000; ++i) {
      unsigned long long int ullint[2];
      sch::BigInt bint[2];
      for (int k = 0; k < 2; ++k) {
        ullint[k] =
            randomInRange(std::numeric_limits<unsigned long long int>::min(),
                          std::numeric_limits<unsigned long long int>::max());
        bint[k] = ullint[k];
      }
      // clang-format off
      CHECK((ullint[0] == ullint[1]) == (bint[0] == ullint[1]));
      CHECK((ullint[0] == ullint[1]) == (ullint[0] == bint[1]));
      CHECK((ullint[0] != ullint[1]) == (bint[0] != ullint[1]));
      CHECK((ullint[0] != ullint[1]) == (ullint[0] != bint[1]));
      CHECK((ullint[0] <  ullint[1]) == (bint[0] < ullint[1]));
      CHECK((ullint[0] <  ullint[1]) == (ullint[0] < bint[1]));
      CHECK((ullint[0] >  ullint[1]) == (bint[0] > ullint[1]));
      CHECK((ullint[0] >  ullint[1]) == (ullint[0] > bint[1]));
      CHECK((ullint[0] <= ullint[1]) == (bint[0] <= ullint[1]));
      CHECK((ullint[0] <= ullint[1]) == (ullint[0] <= bint[1]));
      CHECK((ullint[0] >= ullint[1]) == (bint[0] >= ullint[1]));
      CHECK((ullint[0] >= ullint[1]) == (ullint[0] >= bint[1]));
      CHECK((sch::BigInt10{ullint[0]} + sch::BigInt10{ullint[1]}).to_string() == bint[0] + ullint[1]);
      CHECK((sch::BigInt10{ullint[0]} + sch::BigInt10{ullint[1]}).to_string() == ullint[0] + bint[1]);
      CHECK((sch::BigInt10{ullint[0]} - sch::BigInt10{ullint[1]}).to_string() == bint[0] - ullint[1]);
      CHECK((sch::BigInt10{ullint[0]} - sch::BigInt10{ullint[1]}).to_string() == ullint[0] - bint[1]);
      CHECK((sch::BigInt10{ullint[0]} * sch::BigInt10{ullint[1]}).to_string() == bint[0] * ullint[1]);
      CHECK((sch::BigInt10{ullint[0]} * sch::BigInt10{ullint[1]}).to_string() == ullint[0] * bint[1]);
      // clang-format on
    }
  }
}

/*

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
