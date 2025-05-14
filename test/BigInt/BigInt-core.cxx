#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <random>

#include "BigInt.hpp"
#include "BigInt10.hpp"
#include "helpers.hpp"

namespace big_int_test {

TEST_CASE("constructor and stream insertion") {
  SECTION("positive") {
    for (int i = 0; i < 1000; ++i) {
      std::ostringstream oss[2];
      std::string str = random_string(10000, 20000);
      remove_leading_zeros(str);
      sch::BigInt bint{str};
      oss[0] << str;
      oss[1] << bint;
      CHECK(oss[0].str() == oss[1].str());
    }
  }
  SECTION("negative") {
    for (int i = 0; i < 1000; ++i) {
      std::ostringstream oss[2];
      std::string str = random_string(10000, 20000);
      remove_leading_zeros(str);
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
      std::string str = random_string(1, LL_FULL_LENGTH);
      randomize_sign(str);
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

TEST_CASE("addition") {
  for (int i = 0; i < 1000; ++i) {
    sch::BigInt10 bint10[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = random_string(10000, 20000);
      randomize_sign(str);
      bint10[k] = str;
      bint[k] = str;
    }
    os[0] << bint10[0] + bint10[1];
    os[1] << bint[0] + bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("subtraction") {
  for (int i = 0; i < 1000; ++i) {
    sch::BigInt10 bint10[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = random_string(10000, 20000);
      randomize_sign(str);
      bint10[k] = str;
      bint[k] = str;
    }
    os[0] << bint10[0] - bint10[1];
    os[1] << bint[0] - bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("multiplication") {
  for (int i = 0; i < 50; ++i) {
    sch::BigInt10 n[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = random_string(1, 2000);
      randomize_sign(str);
      n[k] = str;
      bint[k] = str;
    }
    os[0] << n[0] * n[1];
    os[1] << bint[0] * bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

TEST_CASE("division") {
  for (int i = 0; i < 50; ++i) {
    sch::BigInt bint[2];
    sch::BigInt10 n[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = random_string(1, 2000);
      randomize_sign(str);
      n[k] = str;
      bint[k] = str;
    }
    if (n[1] == 0) {
      n[1] = random_in_range(1, 9);
      bint[1] = n[1].to_string();
    }

    os[0] << n[0] / n[1];
    os[1] << bint[0] / bint[1];
    CHECK(os[0].str() == os[1].str());
    if (os[0].str() != os[1].str()) {
      std::cout << n[0] << " / " << n[1] << " ..... " << bint[0] << " / "
                << bint[1] << '\n';
    }
  }
}

TEST_CASE("modulo") {
  for (int i = 0; i < 1000; ++i) {
    long long n[2];
    sch::BigInt bint[2];
    std::ostringstream os[2];

    for (int k = 0; k < 2; ++k) {
      std::string str = random_string(1, LL_FULL_LENGTH);
      randomize_sign(str);
      n[k] = std::stoll(str);
      if (n[k] == 0) {
        n[k] = random_in_range(1, 9); // NOLINT
      }
      bint[k] = n[k];
    }
    os[0] << n[0] % n[1];
    os[1] << bint[0] % bint[1];
    CHECK(os[0].str() == os[1].str());
  }
}

/*

// TODO consider Sign
TEST_CASE("exponentiation") {
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
  // todo fix Sign change on increment
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
  // todo fix Sign change on decrement
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

} // namespace big_int_test
