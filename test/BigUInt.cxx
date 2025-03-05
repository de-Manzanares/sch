#include <catch2/catch_all.hpp>
#include <iostream>
#include <random>

#include "BigUInt.hpp"

std::ranlux48_base rand_engine{std::random_device{}()};
std::uniform_int_distribution<u_int> dist_digit{0, 9};
std::uniform_int_distribution dist_length{1'000, 10'000};
std::uniform_int_distribution dist_length_sum_check{1, 20};
std::uniform_int_distribution dist_length_product_check{1, 8};

TEST_CASE("constructor") {
  for (int i = 0; i < 100; ++i) {
    std::string str;
    for (int j = 0; j < dist_length(rand_engine); ++j) {
      str += static_cast<char>(dist_digit(rand_engine) + '0');
    }
    BigUInt b_uint(str);
  }
}

TEST_CASE("operator: stream extraction") {
  for (int i = 0; i < 100; ++i) {
    std::string str;
    for (int j = 0; j < dist_length(rand_engine); ++j) {
      str += static_cast<char>(dist_digit(rand_engine) + '0');
    }
    std::ostringstream os;
    BigUInt b_uint(str);
    os << b_uint;
    CHECK(os.str() == str);
  }
}

TEST_CASE("operator: + addition") {
  CHECK(BigUInt(std::string{'0'}) + BigUInt(std::string{'0'}) ==
        BigUInt(std::string{'0'}));
  for (int i = 0; i < 500; ++i) {
    std::string str;
    unsigned long long n1;
    unsigned long long n2;
    BigUInt b_uint1;
    BigUInt b_uint2;
    for (int k = 0; k < 2; ++k) {
      str.clear();
      for (int j = 0; j < dist_length_sum_check(rand_engine); ++j) {
        str += static_cast<char>(dist_digit(rand_engine) + '0');
      }
      if (k == 0) {
        b_uint1 = BigUInt{str};
        n1 = std::stoull(str);
      }
      if (k == 1) {
        b_uint2 = BigUInt{str};
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

TEST_CASE("operator: ++x pre-increment") {
  BigUInt b_uint("1");
  CHECK(++b_uint == BigUInt("2"));
  b_uint = BigUInt("999999999");
  CHECK(++b_uint == BigUInt("1000000000"));
}

TEST_CASE("simple multiplication") {
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      BigUInt a(std::to_string(i));
      BigUInt b(std::to_string(j));
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
  for (int i = 0; i < 500; ++i) {
    std::string str;
    unsigned long long n1;
    unsigned long long n2;
    BigUInt b_uint1;
    BigUInt b_uint2;
    for (int k = 0; k < 2; ++k) {
      str.clear();
      for (int j = 0; j < dist_length_product_check(rand_engine); ++j) {
        str += static_cast<char>(dist_digit(rand_engine) + '0');
      }
      if (k == 0) {
        b_uint1 = BigUInt{str};
        n1 = std::stoull(str);
      }
      if (k == 1) {
        b_uint2 = BigUInt{str};
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
      BigUInt a(std::to_string(i));
      std::ostringstream os1;
      std::ostringstream os2;
      std::cout << i << " ^ " << j << " = " << sch::pow(a, j) << '\n';
      os1 << static_cast<unsigned long long>(std::pow(i, j));
      os2 << sch::pow(a, j);
      CHECK(os1.str() == os2.str());
    }
  }
}
