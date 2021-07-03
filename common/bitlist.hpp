/*  bitlist.hpp
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
#include <ostream>
#include <vector>

namespace eth {
class Bitlist : public ssz::Container {
private:
  std::vector<bool> m_arr;

public:
  template <typename... T>
  Bitlist(T &&...l) : m_arr{{std::forward<T>(l)...}} {};

  friend std::ostream &operator<<(std::ostream &os, const Bitlist &m_bits) {
    for (auto const &b : m_bits.m_arr)
      os << b;
    return os;
  };

  void from_hexstring(std::string str);
  std::string to_string() const;
  std::size_t size() const { return m_arr.size(); }

  std::vector<std::byte> serialize() const;
  bool deserialize(ssz::SSZIterator it, ssz::SSZIterator end);
  bool operator==(const Bitlist &) const = default;
  YAML::Node encode() const;
  bool decode(const YAML::Node &node);
};
} // namespace eth
