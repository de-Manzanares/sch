#include <catch2/catch_all.hpp>
#include <iostream>
#include <random>

#include "BigInt.hpp"

std::ranlux48_base rand_engine{std::random_device{}()};
std::uniform_int_distribution<u_int> dist_digit{0, 9};
std::uniform_int_distribution dist_length{1'000, 10'000};
std::uniform_int_distribution dist_length_sum_check{1, 20};
std::uniform_int_distribution dist_length_product_check{1, 8};

TEST_CASE("constructor") {
  SECTION("positive") {
    for (int i = 0; i < 100; ++i) {
      std::string str;
      for (int j = 0; j < dist_length(rand_engine); ++j) {
        str += static_cast<char>(dist_digit(rand_engine) + '0');
      }
      sch::BigInt b_uint(str);
    }
  }
  SECTION("negative") {
    sch::BigInt bing{"-123456789"};
    for (int i = 0; i < 100; ++i) {
      std::string str;
      for (int j = 0; j < dist_length(rand_engine); ++j) {
        str += static_cast<char>(dist_digit(rand_engine) + '0');
      }
      str.insert(0, 1, '-');
      sch::BigInt b_uint(str);
    }
  }
}

TEST_CASE("comparison operators") {
  for (int i = 0; i < 1000; ++i) {
    std::string str;
    long long n1;
    long long n2;
    sch::BigInt b_uint1;
    sch::BigInt b_uint2;
    for (int k = 0; k < 2; ++k) {
      str.clear();
      for (int j = 0; j < dist_length_sum_check(rand_engine); ++j) {
        str += static_cast<char>(dist_digit(rand_engine) + '0');
      }
      if (dist_digit(rand_engine) % 2 == 0) {
        str.insert(0, 1, '-');
      }
      if (k == 0) {
        b_uint1 = sch::BigInt{str};
        n1 = std::stoll(str);
      }
      if (k == 1) {
        b_uint2 = sch::BigInt{str};
        n2 = std::stoll(str);
      }
    }
    CHECK((n1 == n2) == (b_uint1 == b_uint2));
    CHECK((n1 != n2) == (b_uint1 != b_uint2));
    CHECK((n1 < n2) == (b_uint1 < b_uint2));
    CHECK((n1 > n2) == (b_uint1 > b_uint2));
    CHECK((n1 <= n2) == (b_uint1 <= b_uint2));
    CHECK((n1 >= n2) == (b_uint1 >= b_uint2));
  }
}

TEST_CASE("operator << : stream extraction") {
  for (int i = 0; i < 200; ++i) {
    std::string str;
    for (int j = 0; j < dist_length(rand_engine); ++j) {
      str += static_cast<char>(dist_digit(rand_engine) + '0');
    }
    size_t str_index{}; // to ignore leading zero's in string
    while (str[str_index] == '0') {
      ++str_index;
    }
    if (dist_digit(rand_engine) % 2 == 0) {
      str.insert(0, 1, '-');
      ++str_index;
    }
    std::ostringstream os;
    sch::BigInt b_uint{str};
    os << b_uint;
    // ternary is to maintain the '-' while dodging leading zeros, should
    // the character be present
    CHECK(os.str() == (str[0] == '-' ? '-' + str.substr(str_index)
                                     : str.substr(str_index)));
  }
}

TEST_CASE("operator + : addition") {
  CHECK(sch::BigInt(std::string{'0'}) + sch::BigInt(std::string{'0'}) ==
        sch::BigInt(std::string{'0'}));
  for (int i = 0; i < 500; ++i) {
    std::string str;
    unsigned long long n1;
    unsigned long long n2;
    sch::BigInt b_uint1;
    sch::BigInt b_uint2;
    for (int k = 0; k < 2; ++k) {
      str.clear();
      for (int j = 0; j < dist_length_sum_check(rand_engine); ++j) {
        str += static_cast<char>(dist_digit(rand_engine) + '0');
      }
      if (k == 0) {
        b_uint1 = sch::BigInt{str};
        n1 = std::stoull(str);
      }
      if (k == 1) {
        b_uint2 = sch::BigInt{str};
        n2 = std::stoull(str);
      }
    }
    std::ostringstream os1;
    std::ostringstream os2;
    os1 << n1 + n2;
    os2 << b_uint1 + b_uint2;
    std::cout << n1 << " + " << n2 << " = " << b_uint1 + b_uint2 << '\n';
    CHECK(os1.str() == os2.str());
  }
}

TEST_CASE("operator - : subtraction") {
  CHECK(sch::BigInt(std::string{'0'}) - sch::BigInt(std::string{'0'}) ==
        sch::BigInt(std::string{'0'}));
  for (int i = 0; i < 10000; ++i) {
    std::string str;
    long long n1;
    long long n2;
    sch::BigInt b_uint1;
    sch::BigInt b_uint2;
    for (int k = 0; k < 2; ++k) {
      str.clear();
      for (int j = 0; j < dist_length_sum_check(rand_engine); ++j) {
        str += static_cast<char>(dist_digit(rand_engine) + '0');
      }
      if (k == 0) {
        b_uint1 = sch::BigInt{str};
        n1 = std::stoll(str);
      }
      if (k == 1) {
        b_uint2 = sch::BigInt{str};
        n2 = std::stoll(str);
      }
    }
    std::ostringstream os1;
    std::ostringstream os2;
    os1 << n1 - n2;
    os2 << b_uint1 - b_uint2;
    std::cout << n1 << " - " << n2 << " = " << b_uint1 - b_uint2 << '\n';
    CHECK(os1.str() == os2.str());
  }
}

TEST_CASE("operator ++ : pre-increment") {
  sch::BigInt b_uint("1");
  CHECK(++b_uint == sch::BigInt("2"));
  b_uint = sch::BigInt("999999999");
  CHECK(++b_uint == sch::BigInt("1000000000"));
}

TEST_CASE("simple multiplication") {
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      sch::BigInt a(std::to_string(i));
      sch::BigInt b(std::to_string(j));
      std::ostringstream os1;
      std::ostringstream os2;
      std::cout << i << " * " << j << " = " << a * b << '\n';
      os1 << i * j;
      os2 << a * b;
      CHECK(os1.str() == os2.str());
    }
  }
}

TEST_CASE("operator: * multiplication") {
  for (int i = 0; i < 1000; ++i) {
    std::string str;
    unsigned long long n1;
    unsigned long long n2;
    sch::BigInt b_uint1;
    sch::BigInt b_uint2;
    for (int k = 0; k < 2; ++k) {
      str.clear();
      for (int j = 0; j < dist_length_product_check(rand_engine); ++j) {
        str += static_cast<char>(dist_digit(rand_engine) + '0');
      }
      if (k == 0) {
        b_uint1 = sch::BigInt{str};
        n1 = std::stoull(str);
      }
      if (k == 1) {
        b_uint2 = sch::BigInt{str};
        n2 = std::stoull(str);
      }
    }
    std::ostringstream os1;
    std::ostringstream os2;
    os1 << n1 * n2;
    os2 << b_uint1 * b_uint2;
    std::cout << n1 << " * " << n2 << " = " << b_uint1 * b_uint2 << '\n';
    CHECK(os1.str() == os2.str());
  }
}

TEST_CASE("sch::pow()") {
  for (uint i = 0; i < 11; ++i) {
    for (uint j = 0; j < 11; ++j) {
      sch::BigInt a(std::to_string(i));
      std::ostringstream os1;
      std::ostringstream os2;
      std::cout << i << " ^ " << j << " = " << sch::pow(a, j) << '\n';
      os1 << static_cast<unsigned long long>(std::pow(i, j));
      os2 << sch::pow(a, j);
      CHECK(os1.str() == os2.str());
    }
  }
}
