
#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

#include "BigInt.hpp"
#include "helpers.cxx"

namespace BigInt_test {
// todo special cases -- 1's 0's etc.
// todo truncate fractional part of doubles
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
} // namespace BigInt_test
