/*  bytes.hpp
 *
 *  This file is part of Mammon.
 *  mammon is a greedy and selfish ETH consensus client.
 *
 *  Copyright (c) 2021 - Reimundo Heluani (potuz) potuz@potuz.net
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ssz/ssz_container.hpp"
#include "yaml-cpp/yaml.h"
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace eth {
template <std::size_t N> class Bytes : public ssz::Container {
private:
  std::array<std::byte, N> m_arr;

  // The caller must check that the integral value fits in the array.
  constexpr static auto bytes_from_int(std::integral auto value)
      -> std::array<std::byte, N> {
    std::array<std::byte, N> ret_arr{};
    if constexpr (std::endian::native == std::endian::big) {
      std::memcpy(ret_arr.data() + N - 1 - sizeof(value), &value,
                  sizeof(value));
      std::reverse(ret_arr.begin(), ret_arr.end());
    }
    // We assume if it's not big endian we are little endian
    else {
      std::memcpy(ret_arr.data(), &value, sizeof(value));
    }
    return ret_arr;
  }
  static auto chars_to_byte(std::string const &c) -> std::byte {
    unsigned int x = 0;
    std::stringstream ss;
    ss << std::hex << c;
    ss >> x;
    return std::byte(x);
  }

  template <typename T>
  static auto bytes_from_str(T &&str) -> std::array<std::byte, N> {
    if (!str.starts_with("0x"))
      throw std::invalid_argument("string not prepended with 0x");

    str.erase(0, 2);

    if (str.size() > 2 * N)
      throw std::out_of_range("integer larger than bytes size");

    if (str.size() % 2 == 1)
      str.insert(0, "0");

    std::array<std::byte, N> ret_arr{};
    for (int i = 0; i < str.size(); i += 2)
      ret_arr[i / 2] = chars_to_byte(str.substr(i, 2));
    return ret_arr;
  }

public:
  Bytes() : m_arr{} {};
  explicit Bytes(std::string &hex) : m_arr{bytes_from_str(hex)} {};
  explicit Bytes(std::string &&hex)
      : m_arr{bytes_from_str(std::forward<std::string>(hex))} {};
  explicit Bytes(std::integral auto value) requires(sizeof(value) <= N)
      : m_arr{bytes_from_int(value)} {};
  explicit Bytes(std::array<std::byte, N> arr) : m_arr{arr} {};
  explicit Bytes(std::vector<std::byte> arr) {
    if (arr.size() != N)
      throw std::out_of_range("vector of different size than bytes");
    std::copy_n(arr.begin(), N, m_arr.begin());
  }
  template <typename... T> // is_convertible_v<..,std::byte> fails.
  explicit Bytes(T &&...l)
      : m_arr{{std::forward<std::byte>(std::byte(l))...}} {};

  std::vector<std::byte> serialize() const override {
    std::vector<std::byte> ret(m_arr.cbegin(), m_arr.cend());
    return ret;
  }

  bool deserialize(ssz::SSZIterator it, ssz::SSZIterator end) override {
    if (std::distance(it, end) != N)
      return false;
    std::copy(it, end, m_arr.begin());
    return true;
  }

  operator std::vector<std::byte>() {
    std::vector<std::byte> ret(m_arr.begin(), m_arr.end());
    return ret;
  }

  constexpr std::byte *data() noexcept { return m_arr.data(); }

  const std::array<std::byte, N> &to_array() const { return m_arr; }

  std::string to_string() const {
    std::stringstream os;
    std::ios_base::fmtflags save = std::cout.flags();
    os << "0x";
    for (auto i = m_arr.cbegin(); i != m_arr.cend(); ++i)
      os << std::setfill('0') << std::setw(2) << std::hex
         << std::to_integer<int>(*i);
    std::cout.flags(save);
    return os.str();
  };

  void from_string(std::string &hex) { m_arr = bytes_from_str(hex); }

  // clang-format off
  template <typename T> requires(std::unsigned_integral<T> && sizeof(T) == N)
  T to_integer_little_endian() const {
    auto ptr = reinterpret_cast<const T *>(m_arr.data()); // NOLINT
    return *ptr;
  }
  // clang-format on
  std::byte &operator[](std::size_t index) { return m_arr[index]; }

  std::byte operator[](std::size_t index) const { return m_arr[index]; }

  void operator^=(Bytes<N> const &b) {
    for (int i = 0; i < N; i++)
      m_arr[i] ^= b[i];
  }

  template <std::size_t M> constexpr bool operator==(Bytes<M> const &r) const {
    if (N != M)
      return false;
    return m_arr == r.to_array();
  }

  constexpr bool operator==(std::vector<std::byte> const &r) const {
    if (N != r.size())
      return false;
    for (int i = 0; i < N; ++i)
      if (m_arr[i] != r[i])
        return false;
    return true;
  }

  template <std::size_t M> Bytes<N + M> operator+(Bytes<M> const &r) const {
    std::array<std::byte, N + M> ret;
    std::copy(m_arr.cbegin(), m_arr.cend(), ret.begin());
    std::copy(r.to_array().cbegin(), r.to_array().cend(), ret.begin() + N);
    return Bytes<N + M>(ret);
  }

  static constexpr std::size_t ssz_size = N;
  static constexpr std::size_t size() { return N; }
  std::size_t get_ssz_size() const override { return N; }

  constexpr typename std::array<std::byte, N>::iterator begin() noexcept {
    return m_arr.begin();
  }

  constexpr typename std::array<std::byte, N>::const_iterator
  cbegin() const noexcept {
    return m_arr.cbegin();
  }

  constexpr typename std::array<std::byte, N>::iterator end() noexcept {
    return m_arr.end();
  }

  constexpr typename std::array<std::byte, N>::const_iterator
  cend() const noexcept {
    return m_arr.cend();
  }

  YAML::Node encode() const override {
    auto str = this->to_string();
    return YAML::convert<std::string>::encode(str);
  }
  bool decode(const YAML::Node &node) override {
    std::string str;
    if (!YAML::convert<std::string>::decode(node, str))
      return false;
    this->from_string(str);
    return true;
  }
};

using Bytes1 = Bytes<1>;
using Bytes4 = Bytes<4>;   // NOLINT
using Bytes8 = Bytes<8>;   // NOLINT
using Bytes20 = Bytes<20>; // NOLINT
using Bytes32 = Bytes<32>; // NOLINT
using Bytes48 = Bytes<48>; // NOLINT
using Bytes96 = Bytes<96>; // NOLINT

using BytesVector = std::vector<std::byte>;
} // namespace eth
