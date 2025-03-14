# BigInt - Arbitrary Precision Integer

The BigInt class is an arbitrary precision integer that offers natural
arithmetic syntax and semantics.

## Features

* Arbitrary Precision: The sky (RAM) is the limit.
* Natural expression: Seamlessly interoperate with built-in integral types, strings, and character arrays.
    * built-in integral types being (signed or unsigned) `char`, `short`,
      `int`, `long`, `long long`, etc.

## Example Usage

From [Project Euler](https://projecteuler.net/about) [Problem 16](https://projecteuler.net/problem=16).

```cpp
  // the sum of the digits of the number 2^1000
  
  const sch::BigInt n = sch::pow(2, 1000);
  const std::string str = n.to_string();
  sch::BigInt sum{};
  for (const auto &ch : str) {
    sum += ch - '0';
  }
  std::cout << sum << '\n';
```

## How to Use

Simply include the BigInt.hpp header in your project and compile with a C++17 (or later) compliant compiler.

```cpp
#include "sch/BigInt.hpp"
```

## BigInt API Reference

### Constructors

```cpp
std::string str0{"2147483647"};
std::string str1{"-2147483647"};

sch::BigInt a{str0};         // construct with string
sch::BigInt b{"2147483647"}; // construct with c-string
sch::BigInt c{2147483647};   // construct with built-in integral type
sch::BigInt d{a};            // construct with BigInt

// negative numbers are welcome too

sch::BigInt e{str1};          // construct with string
sch::BigInt f{"-2147483647"}; // construct with c-string
sch::BigInt g{-2147483647};   // construct with built-in integral type
sch::BigInt h{a * -1};        // construct with BigInt
```

### Assignment Operator

Assign from other BigInts, C‑strings, standard strings, and built-in integral types:

```cpp
sch::BigInt a;
sch::BigInt b{314159};
std::string str0{"2147483647"};
std::string str1{"-2147483647"};

a = str0;         // assign from string
a = "2147483647"; // assign from c-string
a = 2147483647;   // assign from built-in integral type
a = b;            // assign from BigInt

// negative numbers are welcome too

a = str1;          // assign from string
a = "-2147483647"; // assign from c-string
a = -2147483647;   // assign from built-in integral type
a = b * -1;        // assign from BigInt
```

### To String; Stream Insertion

`BigInt::to_string()` returns a base-10 string representation of the value.

`sch::operator<<(std::ostream &, const BigInt)` inserts a base-10 string representation of the value to a stream.

```cpp
  sch::BigInt a{-123456};
  std::string str = a.to_string();
  

  std::cout << a << '\n';   // -123456
  std::cout << str << '\n'; // -123456
```

### Arithmetic Operators

The class overloads the standard arithmetic operators:
``+ - * / % += -= *= /= %=``

These can be used to interoperate with c-strings, standard strings, built-in
integral types, and other BigInts. Excluding the compound assignment operators,
the operators are symmetric, i.e., the BigInt can be on either side (or both sides)
of the operand.

```cpp
  sch::BigInt a{2147483647};
  sch::BigInt b{-1 * a};
  std::string str{"250"};
  int num = 25;

  // Symmetric arithmetic operations:
  sch::BigInt c = ((("125" * a) + (b % str)) / num) * (a - b);
  std::cout << c << '\n'; // 46116860115554402326

  // Compound assignment
  c += c;
  c -= a * b;
  c *= "-31415926535897932";
  c /= 123456789;
  c %= -987654321;
  std::cout << c << '\n'; // -211191818
```

### Comparison Operators

The class overloads the standard comparison operators:
``== != < > <= >=``

These can be used for comparison with c-strings, standard strings, built-in
integral types, and other BigInts. The operators are symmetric, i.e., the
BigInt can be on either side (or both sides) of the operand.

```cpp
  sch::BigInt a{18446744073709551615UL};
  sch::BigInt b{"113811381138113811381138"};
  std::string str{"2147483647"};
  int num = -2147483648;

  // Symmetric comparison operations:
  bool cmp1 = (a < b);                      // BigInt < BigInt
  bool cmp2 = (a <= str);                   // BigInt <= std::string
  bool cmp3 = ("9223372036854775807" == a); // c-string == BigInt
  bool cmp4 = (b > 184467440737095516UL);   // BigInt > built-in integral type
  bool cmp5 = (num >= a);                   // built-in integral type >= BigInt
  bool cmp6 = (a != b);                     // BigInt != BigInt
```

### Exponentiation

The base can be provided as a c-string, standard string, built-in integral type, or BigInt.
The exponent must be a non-negative (signed or unsigned) built-in integral type.
The function throws std::invalid_argument if the exponent is negative.

```cpp
  const sch::BigInt a = sch::pow(1965, 2004);
  const sch::BigInt b = sch::pow(2004, 1965);
  if (a > b) {
    std::cout << 1965 << '^' << 2004 << "=\n" << a << '\n'; 
  } else {
    std::cout << 2004 << '^' << 1965 << "=\n" << b << '\n';
  }
```

### Unary Negation:

Be careful with this ...

If used on a const BigInt, it will return an opposite-sign copy of the object;
if used on a non-const BigInt, it will modify the object.

```cpp
  int a{2147483647};
  sch::BigInt ma{a};       // mutable BigInt
  const sch::BigInt ca{a}; // const BigInt

  // currently
  // a == ma == ca

  sch::BigInt b0{-ma}; // ma = -ma, b0 = ma;
  sch::BigInt b1{-ca}; // b1 = -ca

  // after the use of unary operator-()
  // a == ca != ma
```

### Increment/Decrement:

The class defines the prefix increment/decrement operators, but not the postfix.

WARNING! These operators do not handle sign change.

They work just fine so long as we avoid crossing zero.
Thus far, attempts to handle sign change within the increment/decrement operators
have drastically impacted performance.
How often do we loop from -100 to 100 anyway?

# License

This project is provided under the [MIT License](LICENSE.md).
