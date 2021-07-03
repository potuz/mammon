/*  eth1data.hpp
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
#include "common/basic_types.hpp"
#include "ssz/ssz_container.hpp"
#include "yaml-cpp/yaml.h"

namespace eth {
struct Eth1Data : public ssz::Container {
  Root deposit_root;
  Counter deposit_count;
  Hash32 block_hash;

  static constexpr std::size_t ssz_size = 72;
  std::size_t get_ssz_size() const { return ssz_size; }
  BytesVector serialize() const {
    return serialize_({&deposit_root, &deposit_count, &block_hash});
  }
  bool deserialize(ssz::SSZIterator it, ssz::SSZIterator end) {
    return deserialize_(it, end, {&deposit_root, &deposit_count, &block_hash});
  }
  YAML::Node encode() const {
    return encode_({{"deposit_root", &deposit_root},
                    {"deposit_count", &deposit_count},
                    {"block_hash", &block_hash}});
  }

  bool decode(const YAML::Node &node) {
    return decode_(node, {{"deposit_root", &deposit_root},
                          {"deposit_count", &deposit_count},
                          {"block_hash", &block_hash}});
  }
};
} // namespace eth
