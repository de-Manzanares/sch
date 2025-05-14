# sch

[![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/de-Manzanares/sch/.github%2Fworkflows%2Fcmake-multi-platform.yml?logo=githubactions&label=Test)](https://github.com/de-Manzanares/sch/tree/master/test)
[![codecov](https://codecov.io/gh/de-Manzanares/sch/graph/badge.svg?token=Y9345DJGVF)](https://codecov.io/gh/de-Manzanares/sch)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue)

# BigInt &mdash; Multiple-Precision Integer

- Fast multiplication ([Karatsuba](https://en.wikipedia.org/wiki/Karatsuba_algorithm))
- Naive division (but not terribly slow)
- Overloads arithmetic, comparison, and stream insertion operators
    - `+ - * / % += -= *= /= %=`
    - `== != < > <= >=`
    - `>>`

### Example application

A solution to [Project Euler](https://projecteuler.net/about) [Problem 16](https://projecteuler.net/problem=16):

```c++
#include <BigInt.hpp>
#include <iostream>

// the sum of the digits of the number 2^1000
int main() {
  sch::BigInt sum{};
  const sch::BigInt n = sch::pow(2, 1000);
  for (const char ch : n.to_string()) {
    sum += ch - '0';
  }
  std::cout << sum << '\n';
}
```

## How to Use

[BigInt Class Reference](https://de-manzanares.github.io/sch/classsch_1_1BigInt.html)

### Getting BigInt

If you are using CMake, you can use
[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html):

```cmake
include(FetchContent)
FetchContent_Declare(
        sch
        GIT_REPOSITORY https://github.com/de-Manzanares/sch.git
        GIT_TAG v0.1.0 # or a later version
)
FetchContent_MakeAvailable(sch)
target_link_libraries(<your-target> PRIVATE sch)
```

## Resources

The construction of this software was informed by the following:

- R. P. Brent and P. Zimmermann, “Modern Computer Arithmetic”.

## License

This software is made available under the [MIT License](LICENSE.md).
