#include "BigInt.hpp"
#include "BigInt_8.hpp"

#include <catch2/catch_all.hpp>
#include <iostream>
#include <random>
#include <string>

std::ranlux48_base rand_engine{std::random_device{}()};
std::uniform_int_distribution<u_int> dist_digit{0, 9};
std::uniform_int_distribution dist_length{1, 1'000};

TEST_CASE("constructor") {
  SECTION("positive") {
    for (int i = 0; i < 500; ++i) {
      std::string str;
      const size_t length = dist_length(rand_engine);
      for (int j = 0; j < length; ++j) {
        str += static_cast<char>(dist_digit(rand_engine) + '0');
      }
      std::cout << str.size() << '\n';
      sch::BigInt bint(str);
    }
  }
}
