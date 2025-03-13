#include <catch2/catch_all.hpp>
#include <iostream>

#include "BigInt.hpp"

namespace BigInt_perf_test {

// PERFORMANCE TESTS -----------------------------------------------------------

sch::BigInt s003(const sch::BigInt &n) {
  sch::BigInt m_bint{n}; // mutable copy
  for (sch::BigInt i = 2; i < n; ++i) {
    while (m_bint % i == 0) {
      if (m_bint / i == 1) {
        return i;
      }
      m_bint /= i;
    }
  }
  return 0;
}

TEST_CASE("largest prime factor") { CHECK(s003(600851475143) == 0x1ac9); }

sch::BigInt s005(const sch::BigInt &n) {
  for (sch::BigInt i = n * (n - 1);; i += 2) {
    for (sch::BigInt j = n; j > 1; --j) {
      if (i % j != 0) {
        break;
      }
      if (j == 2) {
        return i;
      }
    }
  }
}

TEST_CASE("smallest multiple") {
  SKIP("division and modulo are too inefficient");
  std::cout << s005(20);
}

TEST_CASE("large exponentiation") { sch::pow(1954, 2004); }

sch::BigInt s014(const sch::BigInt &n) {
  sch::BigInt max_seed{};
  sch::BigInt max_count{};
  // store sequence length of previously visited integers
  std::map<sch::BigInt, sch::BigInt> cache;

  for (sch::BigInt i = 2; i < n; ++i) {
    sch::BigInt _n = i;
    sch::BigInt count{};
    while (_n != 1 && _n >= i) {
      if (_n % 2 == 0) {
        _n /= 2;
      } else {
        _n = 3 * _n + 1;
      }
      ++count;
    }
    cache[i] = count;
    if (_n < i) {
      cache[i] += cache[_n];
    }
    if (cache[i] > max_count) {
      max_count = cache[i];
      max_seed = i;
    }
  }
  return max_seed;
}

TEST_CASE("longest Collatz sequence") {
  SKIP("division and modulo are too inefficient");
  CHECK(s014(1'000'000) == 0xcc8a7);
}

} // namespace BigInt_perf_test
