/*  slot.hpp
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

#include <cstdint>

#include "bytes.hpp"
#include "helpers/bytes_to_int.hpp"
#include "ssz/ssz_container.hpp"
#include "yaml-cpp/yaml.h"

namespace eth {
class Slot : public ssz::Container {
   private:
    std::uint64_t value_;

   public:
    constexpr Slot(std::uint64_t s = 0) : value_{s} {};
    constexpr ~Slot(){};
    constexpr Slot(const Slot&) = default;
    constexpr Slot(Slot&&) = default;
    constexpr Slot& operator=(const Slot&) = default;
    constexpr Slot& operator=(Slot&&) = default;

    operator std::uint64_t() const { return value_; };
    operator std::uint64_t&() { return value_; }
    operator Bytes8() const { return Bytes8{value_}; }

    std::vector<std::byte> serialize() const override { return Bytes8(value_).serialize(); }
    bool deserialize(ssz::SSZIterator it, ssz::SSZIterator end) override {
        if (std::distance(it, end) != sizeof(value_)) return false;
        value_ = helpers::to_integer_little_endian<std::uint64_t>(&*it);
        return true;
    }

    bool operator==(const Slot&) const = default;
    static constexpr std::size_t ssz_size = 8;
    std::size_t get_ssz_size() const override { return ssz_size; }

    YAML::Node encode() const override { return YAML::convert<std::uint64_t>::encode(value_); }
    bool decode(const YAML::Node& node) override { return YAML::convert<std::uint64_t>::decode(node, value_); }
};

using Epoch = Slot;
using Counter = Slot;
using UnixTime = Slot;
using CommitteeIndex = Slot;
using ValidatorIndex = Slot;
using DepositIndex = Slot;
using Gwei = Slot;

}  // namespace eth
