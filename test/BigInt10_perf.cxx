#include <catch2/catch_all.hpp>
#include <iostream>

#include "BigInt10.hpp"

namespace BigInt10_perf_test {

// PERFORMANCE TESTS -----------------------------------------------------------

sch::BigInt10 s003(const sch::BigInt10 &n) {
  sch::BigInt10 m_bint{n}; // mutable copy
  for (sch::BigInt10 i = 2; i < n; ++i) {
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

sch::BigInt10 s005(const sch::BigInt10 &n) {
  for (sch::BigInt10 i = n * (n - 1);; i += 2) {
    for (sch::BigInt10 j = n; j > 1; --j) {
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

sch::BigInt10 s014(const sch::BigInt10 &n) {
  sch::BigInt10 max_seed{};
  sch::BigInt10 max_count{};
  // store sequence length of previously visited integers
  std::map<sch::BigInt10, sch::BigInt10> cache;

  for (sch::BigInt10 i = 2; i < n; ++i) {
    sch::BigInt10 _n = i;
    sch::BigInt10 count{};
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

} // namespace BigInt10_perf_test
