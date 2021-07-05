/*  bitvector.hpp
 *
 *  This file is part of sszpp.
 *  sszpp is C++ implementation of the "Simple Serialize" specification.
 *  https://github.com/ethereum/eth2.0-specs/tree/dev/ssz
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
#include "common/bytes.hpp"
#include "ssz/ssz.hpp"
#include "ssz/ssz_container.hpp"
#include "yaml-cpp/yaml.h"
#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>

namespace eth {
template <unsigned N> class Bitvector : public ssz::Container {
private:
  std::array<bool, N> m_arr;

public:
  static constexpr std::size_t ssz_size =
      (N + constants::BITS_PER_BYTE - 1) / constants::BITS_PER_BYTE;
  std::size_t get_ssz_size() const override { return ssz_size; }

  Bitvector() = default;
  explicit constexpr Bitvector(std::array<bool, N> vec) : m_arr{vec} {};

  void from_hexstring(std::string str) {
    if (!str.starts_with("0x"))
      throw std::invalid_argument("string not prepended with 0x");
    str.erase(0, 2);

    if (4 * str.length() > N + 4)
      throw std::invalid_argument("hexstring larger than bitvector length");

    std::stringstream ss;
    unsigned int buffer = 0;
    std::vector<unsigned char> hex;
    for (int offset = 0; offset < str.length(); offset += 2) {
      ss.clear();
      ss << std::hex << str.substr(offset, 2);
      ss >> buffer;
      hex.push_back(static_cast<unsigned char>(buffer));
    }

    m_arr.fill(0);
    for (int i = 0; i < hex.size(); ++i)
      for (int j = 0;
           j < constants::BITS_PER_BYTE && constants::BITS_PER_BYTE * i + j < N;
           ++j)
        m_arr[constants::BITS_PER_BYTE * i + j] = ((hex[i] >> j) & 1);
  }

  std::string to_string() const {
    std::stringstream os;
    std::ios_base::fmtflags save = std::cout.flags();
    auto serial = this->serialize();
    os << "0x";
    for (auto i = serial.cbegin(); i != serial.cend(); ++i)
      os << std::setfill('0') << std::setw(2) << std::hex
         << std::to_integer<int>(*i);
    std::cout.flags(save);
    return os.str();
  };

  friend std::ostream &operator<<(std::ostream &os,
                                  const Bitvector<N> &m_bits) {
    for (auto const &b : m_bits.m_arr)
      os << b;
    return os;
  };

  std::vector<std::byte> serialize() const override {
    Bytes<(N + constants::BITS_PER_BYTE - 1) / constants::BITS_PER_BYTE> ret{};
    for (int i = 0; i < N; ++i)
      ret[i / constants::BITS_PER_BYTE] |=
          (std::byte(m_arr[i] << (i % constants::BITS_PER_BYTE)));
    return ret;
  }

  bool deserialize(ssz::SSZIterator it, ssz::SSZIterator end) override {
    if (std::distance(it, end) !=
        (N + constants::BITS_PER_BYTE - 1) / constants::BITS_PER_BYTE)
      return false;
    for (auto i = it; i != end; ++i)
      for (int j = 0; j < constants::BITS_PER_BYTE &&
                      constants::BITS_PER_BYTE * std::distance(it, i) + j < N;
           ++j)
        m_arr[constants::BITS_PER_BYTE * std::distance(it, i) + j] =
            std::to_integer<unsigned char>(*i & (std::byte(1) << j));
    return true;
  }

  bool operator==(const Bitvector &) const = default;

  YAML::Node encode() const override {
    auto str = this->to_string();
    return YAML::convert<std::string>::encode(str);
  }

  bool decode(const YAML::Node &node) override {
    std::string str;
    if (!YAML::convert<std::string>::decode(node, str))
      return false;
    this->from_hexstring(str);
    return true;
  }
};
} // namespace eth
