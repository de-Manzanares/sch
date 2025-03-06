#ifndef SCH_INCLUDE_BIGINT_HPP_
#define SCH_INCLUDE_BIGINT_HPP_

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace sch {

enum class sign : bool { negative, positive };

class BigInt {
 public:
  BigInt() = default;
  explicit BigInt(const std::string &str);
  ~BigInt() = default;

  BigInt(const BigInt &) = default;
  BigInt(BigInt &&) = default;
  BigInt &operator=(const BigInt &) = default;
  BigInt &operator=(BigInt &&) = default;

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
  BigInt &operator-=(const BigInt &rhs);
  BigInt &operator*=(const BigInt &rhs);
  BigInt &operator/=(const BigInt &rhs);
  BigInt &operator%=(const BigInt &rhs);
  BigInt &operator<<=(const BigInt &rhs);
  BigInt &operator>>=(const BigInt &rhs);

  BigInt &operator-();
  BigInt operator-() const;
  BigInt &operator++();
  BigInt &operator--();

  friend std::ostream &operator<<(std::ostream &os, const BigInt &b);

  void normalize();

 private:
  sign _sign = sign::positive;
  std::vector<uint8_t> _data{}; // little-endian order
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
        "BigUInt::BigUInt(): string contains non-numeric characters");
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

//------------------------------------------------------------------------------
// Binary comparison operators

inline bool BigInt::operator==(const BigInt &rhs) const {
  return (_data == rhs._data && _sign == rhs._sign);
}

inline bool BigInt::operator!=(const BigInt &rhs) const {
  return !(*this == rhs);
}

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

inline bool BigInt::operator>(const BigInt &rhs) const { return rhs < *this; }

inline bool BigInt::operator<=(const BigInt &rhs) const {
  return !(*this > rhs);
}

inline bool BigInt::operator>=(const BigInt &rhs) const {
  return !(*this < rhs);
}

//------------------------------------------------------------------------------
// Binary arithmetic operators

inline BigInt BigInt::operator+(const BigInt &rhs) const {

  if (_sign != rhs._sign) {
    if (_sign == sign::negative) {
      return rhs - (-*this);
    }
    if (rhs._sign == sign::negative) {
      return *this - (-rhs);
    }
  }
  if (_sign == sign::negative && rhs._sign == sign::negative) {
    return -(-*this + (-rhs));
  }

  BigInt sum;
  bool carry = false;
  size_t tmp_lhs{0};
  size_t tmp_rhs{0};

  while (tmp_lhs < _data.size() && tmp_rhs < rhs._data.size()) {
    sum._data.push_back(_data[tmp_lhs] + rhs._data[tmp_rhs] + (carry ? 1 : 0));
    if (sum._data.back() > 9) {
      carry = true;
      sum._data.back() -= 10;
    } else {
      carry = false;
    }
    ++tmp_lhs;
    ++tmp_rhs;
  }
  while (tmp_lhs < _data.size()) {
    sum._data.push_back(_data[tmp_lhs] + (carry ? 1 : 0));
    if (sum._data.back() > 9) {
      carry = true;
      sum._data.back() -= 10;
    } else {
      carry = false;
    }
    ++tmp_lhs;
  }
  while (tmp_rhs < rhs._data.size()) {
    sum._data.push_back(rhs._data[tmp_rhs] + (carry ? 1 : 0));
    if (sum._data.back() > 9) {
      carry = true;
      sum._data.back() -= 10;
    } else {
      carry = false;
    }
    ++tmp_rhs;
  }
  if (carry) {
    sum._data.push_back(1);
  }
  sum.normalize();
  return sum;
}

// is there a way to work around using copies to maintain constness?
inline BigInt BigInt::operator-(const BigInt &rhs) const {
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

  // working draft
  BigInt difference{};
  BigInt _lhs{*this};
  BigInt _rhs{rhs};
  size_t tmp_lhs{0};
  size_t tmp_rhs{0};

  if (_rhs > _lhs) {
    difference._sign = sign::negative;
  }
  if (difference._sign == sign::positive) { // subtract rhs from lhs
    while (tmp_lhs < _lhs._data.size() && tmp_rhs < _rhs._data.size()) {
      if (_lhs._data[tmp_lhs] < _rhs._data[tmp_rhs]) {
        _lhs._data[tmp_lhs] += 10;
        if (_lhs._data[tmp_lhs + 1] != 0) {
          _lhs._data[tmp_lhs + 1] -= 1;
        } else {
          size_t tmp_it{1};
          while ((tmp_lhs + tmp_it) < _lhs._data.size() - 1 &&
                 _lhs._data[tmp_lhs + tmp_it] == 0) {
            _lhs._data[tmp_lhs + tmp_it] = 9;
            ++tmp_it;
          }
          _lhs._data[tmp_lhs + tmp_it] -= 1;
        }
      }
      difference._data.push_back(_lhs._data[tmp_lhs] - _rhs._data[tmp_rhs]);
      ++tmp_lhs;
      ++tmp_rhs;
    }
  } else { // subtract lhs from rhs
    while (tmp_lhs < _lhs._data.size() && tmp_rhs < _rhs._data.size()) {
      if (_rhs._data[tmp_rhs] < _lhs._data[tmp_lhs]) {
        _rhs._data[tmp_rhs] += 10;
        if (_rhs._data[tmp_rhs + 1] != 0) {
          _rhs._data[tmp_rhs + 1] -= 1;
        } else {
          size_t tmp_it{1};
          while ((tmp_rhs + tmp_it) < _rhs._data.size() - 1 &&
                 _rhs._data[tmp_rhs + tmp_it] == 0) {
            _rhs._data[tmp_rhs + tmp_it] = 9;
            ++tmp_it;
          }
          _rhs._data[tmp_rhs + tmp_it] -= 1;
        }
      }
      difference._data.push_back(_rhs._data[tmp_rhs] - _data[tmp_lhs]);
      ++tmp_lhs;
      ++tmp_rhs;
    }
  }
  while (tmp_lhs < _lhs._data.size()) {
    difference._data.push_back(_lhs._data[tmp_lhs]);
    ++tmp_lhs;
  }
  while (tmp_rhs < _rhs._data.size()) {
    difference._data.push_back(_rhs._data[tmp_rhs]);
    ++tmp_rhs;
  }
  difference.normalize();
  return difference;
}

inline BigInt BigInt::operator*(const BigInt &rhs) const {
  // naive implementation
  // schoolbook algorithm -- should be faster than repeated addition
  std::vector<BigInt> sums;
  sums.reserve(_data.size() + rhs._data.size());

  BigInt product{};
  bool carry_bool = false;
  uint8_t carry_val{};
  size_t sum_index{0};

  // multiply digits and carry
  for (const auto &digit0 : _data) {
    sums.emplace_back(); // reserve does not actually initialize any data
    // pad with zeros
    if (sums.size() > 1) {
      for (size_t i = 0; i < sums.size() - 1; ++i) {
        sums[sum_index]._data.push_back(0);
      }
    }
    for (const auto &digit1 : rhs._data) {
      sums[sum_index]._data.push_back(digit0 * digit1 +
                                      (carry_bool ? carry_val : 0));
      if (sums[sum_index]._data.back() > 9) {
        carry_bool = true;
        carry_val = sums[sum_index]._data.back() / 10;
        sums[sum_index]._data.back() %= 10;
      } else {
        carry_bool = false;
      }
    }
    if (carry_bool) {
      sums[sum_index]._data.push_back(carry_val);
    }
    ++sum_index;
    carry_bool = false;
    carry_val = 0;
  }
  for (const auto &sum : sums) {
    product += sum;
  }
  product._sign = (_sign == rhs._sign) ? sign::positive : sign::negative;
  product.normalize();
  return product;
}

inline BigInt &BigInt::operator+=(const BigInt &rhs) {
  *this = *this + rhs;
  return *this;
}

inline BigInt &BigInt::operator-=(const BigInt &rhs) {
  *this = *this - rhs;
  return *this;
}

inline BigInt &BigInt::operator*=(const BigInt &rhs) {
  *this = *this * rhs;
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

inline BigInt &BigInt::operator++() {
  size_t i{0};
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

inline BigInt &BigInt::operator--() {

  if (_sign == sign::negative) {
    // -X - 1 = -(X + 1) → Use `operator++()` on the absolute value
    *this = -(-*this + BigInt{"1"});
    return *this;
  }

  // Normal case: subtract 1 from a positive BigInt
  size_t i = 0;
  while (i < _data.size()) {
    if (_data[i] > 0) {
      --_data[i];  // No borrow needed
      break;
    }
    _data[i++] = 9;  // Borrow needed
  }
  return *this;
}

//------------------------------------------------------------------------------
// Stream operators

inline std::ostream &operator<<(std::ostream &os, const BigInt &b) {
  if (b._data.empty()) {
    os << "0";
    return os;
  }
  if (b._sign == sign::negative) {
    os << '-';
  }
  for (auto it = b._data.rbegin(); it != b._data.rend(); ++it) {
    os << static_cast<char>(*it + '0');
  }
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

//------------------------------------------------------------------------------
// Non-member functions

inline BigInt pow(const BigInt &base, const uint exp) {
  BigInt _base = base;
  uint _exp = exp;
  if (BigInt(std::to_string(_exp)) == BigInt(std::string{'0'})) {
    return BigInt(std::string{'1'});
  }
  if (base == BigInt(std::string{'0'})) {
    return BigInt(std::string{'0'});
  }
  BigInt res{std::string{'1'}};
  while (_exp > 0) {
    if (_exp % 2 == 1) {
      res *= _base;
    }
    _base *= _base;
    _exp /= 2;
  }
  return res;
}

} // namespace sch

#endif // SCH_INCLUDE_BIGINT_HPP_
