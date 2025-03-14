#ifndef SCH_INCLUDE_BigInt_HPP_
#define SCH_INCLUDE_BigInt_HPP_

#include <algorithm>
#include <cstdint>
#include <execution>
#include <numeric>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace sch {

enum class sign : bool { negative, positive };

class BigInt {
 public:
  BigInt() = default;
  explicit BigInt(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt(const T val) : BigInt(std::to_string(val)) {} // NOLINT
  ~BigInt() = default;

  BigInt(const BigInt &) = default;       // copy constructor
  BigInt(BigInt &&) = default;            // move constructor
  BigInt &operator=(BigInt &&) = default; // move assignment

  // Copy assignment
  BigInt &operator=(const BigInt &) = default;
  BigInt &operator=(const char *str);
  BigInt &operator=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator=(T val);

  bool operator==(const BigInt &rhs) const;
  bool operator!=(const BigInt &rhs) const;
  bool operator<(const BigInt &rhs) const;
  bool operator>(const BigInt &rhs) const;
  bool operator<=(const BigInt &rhs) const;
  bool operator>=(const BigInt &rhs) const;

  BigInt operator+(const BigInt &rhs) const;
  BigInt operator-(const BigInt &rhs) const;
  BigInt operator*(const BigInt &rhs) const;
  BigInt operator/(const BigInt &rhs) const;
  BigInt operator%(const BigInt &rhs) const;

  BigInt &operator+=(const BigInt &rhs);
  BigInt &operator+=(const char *str);
  BigInt &operator+=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator+=(T val);

  BigInt &operator-=(const BigInt &rhs);
  BigInt &operator-=(const char *str);
  BigInt &operator-=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator-=(T val);

  BigInt &operator*=(const BigInt &rhs);
  BigInt &operator*=(const char *str);
  BigInt &operator*=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator*=(T val);

  BigInt &operator/=(const BigInt &rhs);
  BigInt &operator/=(const char *str);
  BigInt &operator/=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator/=(T val);

  BigInt &operator%=(const BigInt &rhs);
  BigInt &operator%=(const char *str);
  BigInt &operator%=(const std::string &str);
  template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
  BigInt &operator%=(T val);

  BigInt &operator-();
  BigInt operator-() const;
  BigInt &operator++();
  BigInt &operator--();

  friend std::ostream &operator<<(std::ostream &os, const BigInt &b);

  void normalize();
  [[nodiscard]] std::string to_string() const;

 private:
  // constants
  static constexpr uint64_t BASE = 10; ///< data is stored in base 10

  // private variables
  sign _sign = sign::positive;  ///< sign of the number
  std::vector<uint8_t> _data{}; ///< @note little endian order

  // Addition operator helpers ---------------------------------
  static void add(std::size_t &it_lhs, const BigInt &lhs, std::size_t &it_rhs,
                  const BigInt &rhs, bool &carry, BigInt &sum);
  static void a_carryDown(std::size_t &it, const BigInt &bint_8, bool &carry,
                          BigInt &sum);

  // Subtraction operator helpers ------------------------------
  static void subtract(std::size_t &it_lhs, BigInt &lhs, std::size_t &it_rhs,
                       const BigInt &rhs, BigInt &difference);
  static void s_carryDown(std::size_t &it, const BigInt &bint_8, BigInt &difference);

  // Multiplication operator -----------------------------------
  static BigInt longMultiplication(const BigInt &bottom, const BigInt &top);

  // Division operator -----------------------------------------
  static std::pair<BigInt, BigInt> longDivision(const BigInt &dividend,
                                                const BigInt &divisor);
};

//------------------------------------------------------------------------------
// Constructors

// String constructor
inline BigInt::BigInt(const std::string &str) {
  int offset = 0;           // to ignore negative sign, if it exists
  if (str.front() == '-') { // check for sign
    offset = 1;
    _sign = sign::negative;
  }
  // ensure there are no other non-numeric characters
  if (!std::all_of(str.begin() + offset, str.end(), isdigit)) {
    throw std::invalid_argument(
        "BigInt::BigUInt_8() : string contains non-numeric characters");
  }
  _data = std::vector<uint8_t>(str.rbegin(), str.rend() - offset);
  for (auto &digit : _data) {
    digit -= '0';
  }
  // the BigInt objects can sometimes be initialized with leading zeros
  // having leading zeros will cause incorrect comparisons between BigInts
  // best to get rid of them now
  this->normalize();
}

// COPY ASSIGNMENT -------------------------------------------------------------

inline BigInt &BigInt::operator=(const char *str) {
  *this = BigInt{std::string(str)};
  return *this;
}

inline BigInt &BigInt::operator=(const std::string &str) {
  *this = BigInt{str};
  return *this;
}

template <typename T, typename> BigInt &BigInt::operator=(const T val) {
  *this = BigInt{val};
  return *this;
}

// EQUALITY OPERATORS-----------------------------------------------------------

inline bool BigInt::operator==(const BigInt &rhs) const {
  return (_data == rhs._data && _sign == rhs._sign); // NOLINT
}

// for comparison to zero

inline bool operator==(const BigInt &lhs, const int rhs) {
  return lhs == BigInt{rhs};
}

// for comparison to zero

inline bool operator==(const int lhs, const BigInt &rhs) {
  return BigInt{lhs} == rhs;
}

inline bool operator==(const BigInt &lhs, const char *str) {
  return lhs == BigInt{std::string{str}};
}

inline bool operator==(const char *str, const BigInt &rhs) {
  return rhs == BigInt{std::string{str}};
}

inline bool operator==(const BigInt &lhs, const std::string &str) {
  return lhs == BigInt{str};
}

inline bool operator==(const std::string &str, const BigInt &rhs) {
  return rhs == BigInt{str};
}

template <typename T, typename>
bool operator==(const BigInt &lhs, const T &rhs) {
  return lhs == BigInt{rhs};
}

template <typename T, typename>
bool operator==(const T &lhs, const BigInt &rhs) {
  return BigInt{lhs} == rhs;
}

inline bool BigInt::operator!=(const BigInt &rhs) const {
  return !(*this == rhs);
}

inline bool operator!=(const BigInt &lhs, const char *str) {
  return lhs != BigInt{std::string{str}};
}

inline bool operator!=(const char *str, const BigInt &rhs) {
  return rhs != BigInt{std::string{str}};
}

inline bool operator!=(const BigInt &lhs, const std::string &str) {
  return lhs != BigInt{str};
}

inline bool operator!=(const std::string &str, const BigInt &rhs) {
  return rhs != BigInt{str};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator!=(const BigInt &lhs, const T val) {
  return lhs != BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator!=(const T val, const BigInt &rhs) {
  return rhs != BigInt{val};
}

// RELATIONAL OPERATORS --------------------------------------------------------

// LESS THAN -------------------------------------------------------------------

inline bool BigInt::operator<(const BigInt &rhs) const {
  // opposite sign considerations ---------------------
  if (_sign == sign::negative && rhs._sign == sign::positive) {
    return true;
  }
  if (_sign == sign::positive && rhs._sign == sign::negative) {
    return false;
  }
  // same sign considerations -------------------------
  // different lengths ------------------
  // if positive, shorter is smaller
  // if negative, longer is smaller
  if (_data.size() != rhs._data.size()) {
    return (_sign == sign::positive ? _data.size() < rhs._data.size()
                                    : rhs._data.size() < _data.size());
  }
  // same length ------------------------
  // if positive, lexicographically "smaller" is smaller
  // if negative, lexicographically "larger" is smaller
  return (
      _sign == sign::positive
          ? std::lexicographical_compare(_data.rbegin(), _data.rend(),
                                         rhs._data.rbegin(), rhs._data.rend())
          : std::lexicographical_compare(rhs._data.rbegin(), rhs._data.rend(),
                                         _data.rbegin(), _data.rend()));
  // --------------------------------------------------
}

inline bool operator<(const BigInt &lhs, const char *str) {
  return lhs < BigInt{std::string{str}};
}

inline bool operator<(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} < rhs;
}

inline bool operator<(const BigInt &lhs, const std::string &str) {
  return lhs < BigInt{std::string{str}};
}

inline bool operator<(const std::string &str, const BigInt &rhs) {
  return BigInt{std::string{str}} < rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<(const BigInt &lhs, const T val) {
  return lhs < BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<(const T val, const BigInt &rhs) {
  return BigInt{val} < rhs;
}

inline bool BigInt::operator>(const BigInt &rhs) const { return rhs < *this; }

// GREATER THAN ----------------------------------------------------------------

inline bool operator>(const BigInt &lhs, const char *str) {
  return lhs > BigInt{std::string{str}};
}

inline bool operator>(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} > rhs;
}

inline bool operator>(const BigInt &lhs, const std::string &str) {
  return lhs > BigInt{str};
}

inline bool operator>(const std::string &str, const BigInt &rhs) {
  return BigInt{str} > rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>(const BigInt &lhs, const T val) {
  return lhs > BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>(const T val, const BigInt &rhs) {
  return BigInt{val} > rhs;
}

// LESS THAN OR EQUAL TO -------------------------------------------------------

inline bool BigInt::operator<=(const BigInt &rhs) const {
  return !(*this > rhs);
}

inline bool operator<=(const BigInt &lhs, const char *str) {
  return lhs <= BigInt{std::string{str}};
}

inline bool operator<=(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} <= rhs;
}

inline bool operator<=(const BigInt &lhs, const std::string &str) {
  return lhs <= BigInt{str};
}

inline bool operator<=(const std::string &str, const BigInt &rhs) {
  return BigInt{str} <= rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<=(const BigInt &lhs, const T val) {
  return lhs <= BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator<=(const T val, const BigInt &rhs) {
  return BigInt{val} <= rhs;
}

// GREATER THAN OR EQUAL TO ----------------------------------------------------

inline bool BigInt::operator>=(const BigInt &rhs) const {
  return !(*this < rhs);
}

inline bool operator>=(const BigInt &lhs, const char *str) {
  return lhs >= BigInt{std::string{str}};
}

inline bool operator>=(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} >= rhs;
}

inline bool operator>=(const BigInt &lhs, const std::string &str) {
  return lhs >= BigInt{str};
}

inline bool operator>=(const std::string &str, const BigInt &rhs) {
  return BigInt{str} >= rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>=(const BigInt &lhs, const T val) {
  return lhs >= BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool operator>=(const T val, const BigInt &rhs) {
  return BigInt{val} >= rhs;
}

// ADDITION OPERATOR -----------------------------------------------------------

inline BigInt BigInt::operator+(const BigInt &rhs) const { // NOLINT
  // todo optimizations for adding to 0 or 1 and so on
  // Initially, addition and subtraction were implemented assuming two
  // non-negative integers. Sign handling was introduced afterward; the most
  // straightforward approach to implementation was(is?,were?) the
  // conditional statements below. This allows us to reuse the subtraction
  // (addition) logic.
  if (_sign != rhs._sign) {
    if (_sign == sign::negative) {
      return rhs - -*this;
    }
    if (rhs._sign == sign::negative) {
      return *this - -rhs;
    }
  }
  if (_sign == sign::negative && rhs._sign == sign::negative) {
    return -(-*this + -rhs);
  }

  BigInt sum;
  bool carry = false;
  std::size_t it_lhs{0}; // iterate through the digits of the lhs
  std::size_t it_rhs{0}; // iterate through the digits of the rhs

  sum._data.reserve(_data.size() > rhs._data.size() ? _data.size()
                                                    : rhs._data.size());

  add(it_lhs, *this, it_rhs, rhs, carry, sum);
  a_carryDown(it_lhs, *this, carry, sum);
  a_carryDown(it_rhs, rhs, carry, sum);

  if (carry) { // final carry
    sum._data.push_back(1);
  }
  sum.normalize();
  return sum;
}

/**
 * @brief School-book addition.
 * @param[in,out] it_lhs iterate through the lhs digits
 * @param[in] lhs the left-hand-side addend
 * @param[in,out] it_rhs iterate through the rhs digits
 * @param[in] rhs the right-hand-side addend
 * @param[in,out] carry carry 1?
 * @param[in,out] sum the sum
 */
inline void BigInt::add(std::size_t &it_lhs, const BigInt &lhs, std::size_t &it_rhs,
                        const BigInt &rhs, bool &carry, BigInt &sum) {
  while (it_lhs < lhs._data.size() && it_rhs < rhs._data.size()) {
    sum._data.push_back(lhs._data[it_lhs] + rhs._data[it_rhs] +
                        (carry ? 1 : 0));
    if (sum._data.back() > BASE - 1) {
      carry = true;
      sum._data.back() -= BASE;
    } else {
      carry = false;
    }
    ++it_lhs;
    ++it_rhs;
  }
}

/**
 * @brief School-book addition -- performs all the x + 0 columns
 * @param[in,out] it iterate through the digits
 * @param bint_8 the number we are iterating through
 * @param[in,out] carry carry 1?
 * @param[in,out] sum the sum
 */
inline void BigInt::a_carryDown(std::size_t &it, const BigInt &bint_8, bool &carry,
                                BigInt &sum) {
  while (it < bint_8._data.size()) {
    sum._data.push_back(bint_8._data[it] + (carry ? 1 : 0));
    if (sum._data.back() > BASE - 1) {
      carry = true;
      sum._data.back() -= BASE;
    } else {
      carry = false;
    }
    ++it;
  }
}

inline BigInt operator+(const BigInt &lhs, const char *str) {
  return lhs + BigInt{std::string{str}};
}

inline BigInt operator+(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} + rhs;
}

inline BigInt operator+(const BigInt &lhs, const std::string &str) {
  return lhs + BigInt{str};
}

inline BigInt operator+(const std::string &str, const BigInt &rhs) {
  return BigInt{str} + rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator+(const BigInt &lhs, const T val) {
  return lhs + BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator+(const T val, const BigInt &rhs) {
  return BigInt{val} + rhs;
}

// SUBTRACTION OPERATOR --------------------------------------------------------

// is there a way to work around using copies to maintain constness?

inline BigInt BigInt::operator-(const BigInt &rhs) const { // NOLINT
  // todo optimizations for subtracting to and from 0 or 1 and so on
  // Initially, addition and subtraction were implemented assuming two
  // non-negative integers. Sign handling was introduced afterward; the most
  // straightforward approach to implementation was(is?,were?) the conditional
  // statements below. This allows us to reuse the subtraction (addition) logic.
  if (*this == rhs) {
    return BigInt{"0"};
  }
  if (_sign != rhs._sign) {
    if (_sign == sign::negative) {
      return -(-(*this) + rhs);
    }
    if (rhs._sign == sign::negative) {
      return *this + (-(rhs));
    }
  }
  if (_sign == sign::negative && rhs._sign == sign::negative) {
    return -(rhs) - (-(*this));
  }

  BigInt difference{};
  BigInt _lhs{*this}; // mutable copy
  BigInt _rhs{rhs};   // mutable copy
  std::size_t it_lhs{0};   // iterate through the digits of the lhs
  std::size_t it_rhs{0};   // iterate through the digits of the rhs

  difference._data.reserve(_data.size() > rhs._data.size() ? _data.size()
                                                           : rhs._data.size());

  if (_rhs > _lhs) {
    difference._sign = sign::negative; // otherwise dif sign is pos. by default
  }
  if (difference._sign == sign::positive) { // subtract rhs from lhs
    subtract(it_lhs, _lhs, it_rhs, _rhs, difference);
  } else { // subtract lhs from rhs
    subtract(it_rhs, _rhs, it_lhs, _lhs, difference);
  }
  s_carryDown(it_lhs, _lhs, difference);
  s_carryDown(it_rhs, _rhs, difference);

  difference.normalize();
  return difference;
}

/**
 * @brief School-book subtraction
 * @param[in,out] it_lhs iterate through the digits of the lhs
 * @param[in,out] lhs the minuend
 * @param[in,out] it_rhs iterate through the digits of the rhs
 * @param rhs the subtrahend
 * @param[in,out] difference the difference
 */
inline void BigInt::subtract(std::size_t &it_lhs, BigInt &lhs, std::size_t &it_rhs,
                             const BigInt &rhs, BigInt &difference) {
  while (it_lhs < lhs._data.size() && it_rhs < rhs._data.size()) {
    if (lhs._data[it_lhs] < rhs._data[it_rhs]) {
      lhs._data[it_lhs] += 10;
      if (lhs._data[it_lhs + 1] != 0) {
        lhs._data[it_lhs + 1] -= 1;
      } else {
        std::size_t tmp_it{1};
        while ((it_lhs + tmp_it) < lhs._data.size() - 1 &&
               lhs._data[it_lhs + tmp_it] == 0) {
          lhs._data[it_lhs + tmp_it] = 9;
          ++tmp_it;
        }
        lhs._data[it_lhs + tmp_it] -= 1;
      }
    }
    difference._data.push_back(lhs._data[it_lhs] - rhs._data[it_rhs]);
    ++it_lhs;
    ++it_rhs;
  }
}

/**
 * School-book subtraction -- performs all the x - 0 columns
 * @param[in,out] it iterate through the digits
 * @param bint_8 the number we are iterating through
 * @param[in,out] difference the difference
 */
inline void BigInt::s_carryDown(std::size_t &it, const BigInt &bint_8,
                                BigInt &difference) {
  while (it < bint_8._data.size()) {
    difference._data.push_back(bint_8._data[it]);
    ++it;
  }
}

inline BigInt operator-(const BigInt &lhs, const char *str) {
  return lhs - BigInt{std::string{str}};
}

inline BigInt operator-(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} - rhs;
}

inline BigInt operator-(const BigInt &lhs, const std::string &str) {
  return lhs - BigInt{str};
}

inline BigInt operator-(const std::string &str, const BigInt &rhs) {
  return BigInt{str} - rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator-(const BigInt &lhs, const T val) {
  return lhs - BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator-(const T val, const BigInt &rhs) {
  return BigInt{val} - rhs;
}

// MULTIPLICATION OPERATOR -----------------------------------------------------

inline BigInt BigInt::operator*(const BigInt &rhs) const {
  // minimize number of operations by putting the shorter number on the "bottom"
  return _data.size() < rhs._data.size() ? longMultiplication(*this, rhs)
                                         : longMultiplication(rhs, *this);
}

/**
 * @brief School-book multiplication
 * @param bottom the number on the "bottom" of the long multiplication setup
 * @param top the number on the "top" of the long multiplication setup
 * @return the product of the two factors
 */
inline BigInt BigInt::longMultiplication(const BigInt &bottom,
                                         const BigInt &top) {
  BigInt final_product{};       // the sum of the intermediate products
  std::vector<BigInt> products; // the intermediate products
  uint8_t carry{0};             // what value is being carried?
  std::size_t latest_prod{0};        // latest intermediate product

  products.reserve(bottom._data.size());

  for (const auto &n_b : bottom._data) {
    // reserve does not initialize the objects
    products.emplace_back();
    products[latest_prod]._data.reserve(products.size() + top._data.size());

    // the intermediate products must be raised to the correct power of 10
    products[latest_prod]._data.resize(products.size() - 1, 0);

    // multiply the bottom digit by each of the top digits
    for (const auto &n_t : top._data) {
      products[latest_prod]._data.push_back(n_b * n_t + (carry ? carry : 0));
      if (products[latest_prod]._data.back() > BASE - 1) {
        carry = products[latest_prod]._data.back() / BASE;
        products[latest_prod]._data.back() %= BASE;
      } else {
        carry = 0;
      }
    }
    if (carry) { // final carry of the iteration
      products[latest_prod]._data.push_back(carry);
    }
    ++latest_prod; // moving on
    carry = 0;     // reset for the next intermediate product
  }
  final_product = std::reduce(std::execution::par, products.begin(),
                              products.end(), BigInt{});
  final_product._sign =
      bottom._sign == top._sign ? sign::positive : sign::negative;
  final_product.normalize();
  return final_product;
}

inline BigInt operator*(const BigInt &lhs, const char *str) {
  return lhs * BigInt{std::string{str}};
}

inline BigInt operator*(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} * rhs;
}

inline BigInt operator*(const BigInt &lhs, const std::string &str) {
  return lhs * BigInt{str};
}

inline BigInt operator*(const std::string &str, const BigInt &rhs) {
  return BigInt{str} * rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator*(const BigInt &lhs, const T val) {
  return lhs * BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator*(const T val, const BigInt &rhs) {
  return BigInt{val} * rhs;
}

// DIVISION and MODULO OPERATORS -----------------------------------------------

inline BigInt BigInt::operator/(const BigInt &rhs) const {
  return longDivision(*this, rhs).first;
}

inline BigInt BigInt::operator%(const BigInt &rhs) const {
  return longDivision(*this, rhs).second;
}

/**
 * @brief School-book division
 * @param dividend the number to be divided
 * @param divisor the number to divide by
 * @return {quotient,remainder}
 */
inline std::pair<BigInt, BigInt> BigInt::longDivision(const BigInt &dividend,
                                                      const BigInt &divisor) {
  if (divisor == "0") {
    throw std::runtime_error(
        "BigInt::operator/() : Division by zero is undefined");
  }

  BigInt m_dividend{dividend}; // mutable copy
  BigInt m_divisor{divisor};   // mutable copy
  BigInt quotient{};
  BigInt remainder{};

  m_dividend._sign = sign::positive;
  m_divisor._sign = sign::positive;

  auto chooseQuotientSign = [&dividend, &divisor]() {
    return dividend._sign == divisor._sign ? sign::positive : sign::negative;
  };
  auto chooseRemainderSign = [&dividend, &remainder]() {
    if (remainder == "0") {
      return sign::positive;
    }
    return dividend._sign == sign::positive ? sign::positive : sign::negative;
  };

  if (m_divisor == m_dividend) {
    quotient = 1;
    quotient._sign = chooseQuotientSign();
    return {quotient, 0};
  }
  if (m_divisor > m_dividend) {
    return {0, dividend};
  }
  if (m_divisor == 1) {
    quotient = m_dividend;
    quotient._sign = chooseQuotientSign();
    return {quotient, 0};
  }

  std::vector<BigInt> products(10);
  products[0] = 0;
  std::generate(std::next(products.begin()), products.end(),
                [acc = BigInt{0}, &m_divisor]() mutable {
                  acc += m_divisor;
                  return acc;
                });

  // find the first place of the quotient
  auto it = m_dividend._data.rbegin();
  while (it != m_dividend._data.rend()) {
    // todo maybe little endian order is not the best? lol
    remainder._data.insert(remainder._data.begin(), *it);
    ++it;

    uint8_t multiple{};
    remainder.normalize();
    multiple = std::distance(products.begin(),
                             std::prev(std::upper_bound(
                                 products.begin(), products.end(), remainder)));
    quotient._data.emplace_back(multiple);
    remainder -= m_divisor * multiple;
  }

  std::reverse(quotient._data.begin(), quotient._data.end());
  quotient.normalize();
  quotient._sign = chooseQuotientSign();
  remainder._sign = chooseRemainderSign();
  return {quotient, remainder};
}

inline BigInt operator/(const BigInt &lhs, const char *str) {
  return lhs / BigInt{std::string{str}};
}

inline BigInt operator/(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} / rhs;
}

inline BigInt operator/(const BigInt &lhs, const std::string &str) {
  return lhs / BigInt{str};
}

inline BigInt operator/(const std::string &str, const BigInt &rhs) {
  return BigInt{str} / rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator/(const BigInt &lhs, const T val) {
  return lhs / BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator/(const T val, const BigInt &rhs) {
  return BigInt{val} / rhs;
}

inline BigInt operator%(const BigInt &lhs, const char *str) {
  return lhs % BigInt{std::string{str}};
}

inline BigInt operator%(const char *str, const BigInt &rhs) {
  return BigInt{std::string{str}} % rhs;
}

inline BigInt operator%(const BigInt &lhs, const std::string &str) {
  return lhs % BigInt{str};
}

inline BigInt operator%(const std::string &str, const BigInt &rhs) {
  return BigInt{str} % rhs;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator%(const BigInt &lhs, const T val) {
  return lhs % BigInt{val};
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt operator%(const T val, const BigInt &rhs) {
  return BigInt{val} % rhs;
}

// SELF ASSIGNMENT OPERATORS ---------------------------------------------------

inline BigInt &BigInt::operator+=(const BigInt &rhs) {
  *this = *this + rhs;
  return *this;
}

inline BigInt &BigInt::operator+=(const char *str) {
  *this = *this + BigInt{std::string{str}};
  return *this;
}

inline BigInt &BigInt::operator+=(const std::string &str) {
  *this = *this + BigInt{str};
  return *this;
}

template <typename T, typename> BigInt &BigInt::operator+=(const T val) {
  *this = *this + BigInt{val};
  return *this;
}

inline BigInt &BigInt::operator-=(const BigInt &rhs) {
  *this = *this - rhs;
  return *this;
}

inline BigInt &BigInt::operator-=(const char *str) {
  *this = *this - BigInt{std::string{str}};
  return *this;
}

inline BigInt &BigInt::operator-=(const std::string &str) {
  *this = *this - BigInt{str};
  return *this;
}

template <typename T, typename> BigInt &BigInt::operator-=(const T val) {
  *this = *this - BigInt{val};
  return *this;
}

inline BigInt &BigInt::operator*=(const BigInt &rhs) {
  *this = *this * rhs;
  return *this;
}

inline BigInt &BigInt::operator*=(const char *str) {
  *this = *this * BigInt{std::string{str}};
  return *this;
}

inline BigInt &BigInt::operator*=(const std::string &str) {
  *this = *this * BigInt{str};
  return *this;
}

template <typename T, typename> BigInt &BigInt::operator*=(const T val) {
  *this = *this * BigInt{val};
  return *this;
}

inline BigInt &BigInt::operator/=(const BigInt &rhs) {
  *this = *this / rhs;
  return *this;
}

inline BigInt &BigInt::operator/=(const char *str) {
  *this = *this / BigInt{std::string{str}};
  return *this;
}

inline BigInt &BigInt::operator/=(const std::string &str) {
  *this = *this / BigInt{str};
  return *this;
}

template <typename T, typename> BigInt &BigInt::operator/=(const T val) {
  *this = *this / BigInt{val};
  return *this;
}

inline BigInt &BigInt::operator%=(const BigInt &rhs) {
  *this = *this % rhs;
  return *this;
}

inline BigInt &BigInt::operator%=(const char *str) {
  *this = *this % BigInt{std::string{str}};
  return *this;
}

inline BigInt &BigInt::operator%=(const std::string &str) {
  *this = *this % BigInt{str};
  return *this;
}

template <typename T, typename> BigInt &BigInt::operator%=(const T val) {
  *this = *this % BigInt{val};
  return *this;
}

// ------------------------------------------------------------------------------
// Unary operators

inline BigInt &BigInt::operator-() {
  _sign = (_sign == sign::positive ? sign::negative : sign::positive);
  return *this;
}

inline BigInt BigInt::operator-() const {
  BigInt tmp = *this;
  tmp._sign = (tmp._sign == sign::positive ? sign::negative : sign::positive);
  return tmp;
}

//------------------------------------------------------------------------------
// Increment decrement operators

inline BigInt &BigInt::operator++() { // NOLINT(*-no-recursion)
  if (_sign == sign::negative) {
    // -X + 1 = -(X - 1) Use `operator--()` on the absolute value
    *this = -(--(-*this));
    return *this;
  }
  std::size_t i{0};
  // while there is carry propagation
  while (i < _data.size() && ++_data[i] == 10) {
    // propagate
    _data[i++] = 0;
  }
  // if we reach the end, execute the final carry
  if (i == _data.size()) {
    _data.push_back(1);
  }
  return *this;
}

inline BigInt &BigInt::operator--() { // NOLINT(*-no-recursion)
  if (_sign == sign::negative) {
    // -X - 1 = -(X + 1) Use `operator++()` on the absolute value
    *this = -(++(-*this));
    return *this;
  }

  // Normal case: subtract 1 from a positive BigInt
  std::size_t i = 0;
  while (i < _data.size()) {
    if (_data[i] > 0) {
      --_data[i]; // No borrow needed
      break;
    }
    _data[i++] = 9; // Borrow needed
  }
  if (_data.back() == 0 && _data.size() != 1) {
    _data.pop_back();
  }
  return *this;
}

//------------------------------------------------------------------------------
// Stream operators

inline std::ostream &operator<<(std::ostream &os, const BigInt &b) {
  os << b.to_string();
  return os;
}

//------------------------------------------------------------------------------
// Member functions

inline void BigInt::normalize() {
  while (_data.size() > 1 && _data.back() == 0) {
    _data.pop_back();
  }
  if (_data.empty() || _data == std::vector{uint8_t{0}}) {
    _sign = sign::positive;
  }
}

/**
 * @return Base-10 string representation
 */
inline std::string BigInt::to_string() const {
  if (_data.empty()) {
    return std::string{"0"};
  }
  std::string str;
  str.reserve(_data.size());
  if (_sign == sign::negative) {
    str.push_back('-');
  }
  for (auto it = _data.rbegin(); it != _data.rend(); ++it) {
    str.push_back(static_cast<char>(*it + '0'));
  }
  return str;
}

//------------------------------------------------------------------------------
// Non-member functions

/**
 * @tparam T A built-in integral type (signed or unsigned).
 *           Must be non-negative when calling this function.
 * @param base The base value (x in x^y).
 * @param exp  The exponent value (y in x^y).
 * @return The result of x^y as a BigInt.
 * @throws std::invalid_argument if `exp` is negative.
 */
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
BigInt pow(const BigInt &base, const T exp) {
  if (exp < 0) {
    throw(std::invalid_argument("BigInt::pow() : negative exponent"));
  }
  if (exp == 0) { // precedes the next check because 0^0 == 1
    return 1;
  }
  if (base == BigInt{0}) {
    return 0;
  }

  BigInt m_base = base;                       // mutable copy
  auto m_exp = static_cast<std::size_t>(exp); // mutable copy
  BigInt res{1};                              // result

  while (m_exp > 0) {
    if (m_exp % 2 == 1) {
      res *= m_base;
    }
    m_base *= m_base;
    m_exp /= 2;
  }
  return res;
}

} // namespace sch

#endif // SCH_INCLUDE_BigInt_HPP_
