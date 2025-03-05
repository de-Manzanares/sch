#ifndef PROJECT_EULER_CPP17_BIGUINT_HPP_
#define PROJECT_EULER_CPP17_BIGUINT_HPP_

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

class BigUInt {
 public:
  BigUInt() = default;
  explicit BigUInt(const std::string &str);
  ~BigUInt() = default;

  BigUInt(const BigUInt &) = default;
  BigUInt(BigUInt &&) = default;
  BigUInt &operator=(const BigUInt &) = default;
  BigUInt &operator=(BigUInt &&) = default;

  bool operator==(const BigUInt &rhs) const;
  bool operator!=(const BigUInt &rhs) const;
  bool operator<(const BigUInt &rhs) const;
  bool operator>(const BigUInt &rhs) const;
  bool operator<=(const BigUInt &rhs) const;
  bool operator>=(const BigUInt &rhs) const;

  BigUInt operator+(const BigUInt &rhs) const;
  BigUInt operator-(const BigUInt &rhs) const;
  BigUInt operator*(const BigUInt &rhs) const;
  BigUInt operator/(const BigUInt &rhs) const;
  BigUInt operator%(const BigUInt &rhs) const;
  BigUInt &operator+=(const BigUInt &rhs);
  BigUInt &operator-=(const BigUInt &rhs);
  BigUInt &operator*=(const BigUInt &rhs);
  BigUInt &operator/=(const BigUInt &rhs);
  BigUInt &operator%=(const BigUInt &rhs);
  BigUInt &operator<<=(const BigUInt &rhs);
  BigUInt &operator>>=(const BigUInt &rhs);

  BigUInt &operator++();
  BigUInt &operator--();

  friend std::ostream &operator<<(std::ostream &os, const BigUInt &b);

  void normalize();

 private:
  std::vector<uint8_t> _data; // little-endian order
};

inline BigUInt::BigUInt(const std::string &str) {
  if (!std::all_of(str.begin(), str.end(), isdigit)) {
    throw std::invalid_argument(
        "BigUInt::BigUInt(): string contains non-numeric characters");
  }
  _data = std::vector<uint8_t>(str.rbegin(), str.rend());
  for (auto &digit : _data) {
    digit -= '0';
  }
}

inline bool BigUInt::operator==(const BigUInt &rhs) const {
  return _data == rhs._data;
}

inline bool BigUInt::operator!=(const BigUInt &rhs) const {
  return _data != rhs._data;
}

inline bool BigUInt::operator<(const BigUInt &rhs) const {
  // little endian order means we cannot reuse the std::vector operator
  if (_data.size() != rhs._data.size()) {
    return _data.size() < rhs._data.size(); // shorter number is smaller
  }
  return std::lexicographical_compare(_data.rbegin(), _data.rend(),
                                      rhs._data.rbegin(), rhs._data.rend());
}

inline bool BigUInt::operator>(const BigUInt &rhs) const { return rhs < *this; }

inline bool BigUInt::operator<=(const BigUInt &rhs) const {
  return !(*this > rhs);
}

inline bool BigUInt::operator>=(const BigUInt &rhs) const {
  return !(*this < rhs);
}

inline BigUInt BigUInt::operator+(const BigUInt &rhs) const {
  BigUInt sum;
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

inline BigUInt &BigUInt::operator+=(const BigUInt &rhs) {
  *this = *this + rhs;
  return *this;
}

inline BigUInt &BigUInt::operator++() {
  *this += BigUInt("1");
  return *this;
}

inline BigUInt BigUInt::operator*(const BigUInt &rhs) const {
  // naive implementation
  // schoolbook algorithm -- should be faster than repeated addition
  std::vector<BigUInt> sums;
  sums.reserve(_data.size() + rhs._data.size());

  BigUInt product{};
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
  product.normalize(); // idk if this is necessary in this function
  return product;
}

inline BigUInt &BigUInt::operator*=(const BigUInt &rhs) {
  *this = *this * rhs;
  return *this;
}

inline std::ostream &operator<<(std::ostream &os, const BigUInt &b) {
  for (auto it = b._data.rbegin(); it != b._data.rend(); ++it) {
    os << static_cast<char>(*it + '0');
  }
  return os;
}

inline void BigUInt::normalize() {
  while (_data.size() > 1 && _data.back() == 0) {
    _data.pop_back();
  }
}

namespace sch {
inline BigUInt pow(const BigUInt &base, const uint exp) {
  BigUInt _base = base;
  uint _exp = exp;
  if (BigUInt(std::to_string(_exp)) == BigUInt(std::string{'0'})) {
    return BigUInt(std::string{'1'});
  }
  if (base == BigUInt(std::string{'0'})) {
    return BigUInt(std::string{'0'});
  }
  BigUInt res{std::string{'1'}};
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

#endif // PROJECT_EULER_CPP17_BIGUINT_HPP_
