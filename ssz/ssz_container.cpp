/*  ssz_container.cpp
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

#include "ssz_container.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>

#include "common/bytes.hpp"
#include "helpers/bytes_to_int.hpp"
#include "ssz/ssz.hpp"

template <typename T>
std::uint32_t compute_fixed_length(const std::vector<T *> &parts) {
    std::uint32_t ret = 0;
    auto sum_lengths = [&ret](const T *part) {
        if (part->get_ssz_size() == 0) {
            ret += constants::BYTES_PER_LENGTH_OFFSET;
        } else {
            ret += std::uint32_t(part->get_ssz_size());
        }
    };
    std::for_each(parts.cbegin(), parts.cend(), sum_lengths);
    return ret;
}

namespace ssz {
std::vector<std::byte> Container::serialize_(const std::vector<const Container *> &parts) {
    // Check if we are one of the basic types
    if (parts.size() == 0) return {};

    // Compute the length of the fixed sized parts;
    auto fixed_length = compute_fixed_length(parts);

    // Insert the fixed parts and the offsets
    std::vector<std::byte> ret, variable_part;
    for (auto *part : parts) {
        auto part_ssz = part->serialize();
        if (part->get_ssz_size() == 0) {
            eth::Bytes4 offset(fixed_length);
            ret.insert(ret.end(), offset.begin(), offset.end());

            fixed_length += std::uint32_t(part_ssz.size());
            variable_part.insert(variable_part.end(), part_ssz.begin(), part_ssz.end());
        } else {
            ret.insert(ret.end(), part_ssz.begin(), part_ssz.end());
        }
    }
    ret.insert(ret.end(), variable_part.begin(), variable_part.end());
    return ret;
}

bool Container::deserialize_(SSZIterator it, SSZIterator end, const std::vector<Container *> &parts) {
    auto fixed_length = compute_fixed_length(parts);
    SSZIterator begin = it;
    // We are hardcoding BYTES_PER_LENGTH_OFFSET = 4 here
    std::uint32_t last_offset = 0;
    Container *last_variable_part = nullptr;

    for (auto *part : parts) {
        auto part_size = part->get_ssz_size();
        if (part_size) {
            if (std::distance(it, end) < part_size) return false;
            if (!part->deserialize(it, it + part_size))  // NOLINT
                return false;
            it += part_size;  // NOLINT
        } else {
            if (std::distance(it, end) < constants::BYTES_PER_LENGTH_OFFSET) return false;
            auto current_offset = helpers::to_integer_little_endian<std::uint32_t>(&*it);
            if (std::distance(begin, end) < current_offset) return false;

            if (last_offset) {
                if (current_offset < last_offset) return false;
                if (!last_variable_part->deserialize(begin + last_offset, begin + current_offset)) return false;
            } else if (current_offset != fixed_length)
                return false;

            last_offset = current_offset;
            last_variable_part = part;
            it += constants::BYTES_PER_LENGTH_OFFSET;
        }
    }
    if (last_offset)
        if (!last_variable_part->deserialize(begin + last_offset, end)) return false;
    return true;
}

bool Container::decode_(const YAML::Node &node, std::vector<Part> parts) {
    return std::all_of(parts.begin(), parts.end(),
                       [&node](Part part) { return part.second->decode(node[part.first]); });
}

YAML::Node Container::encode_(const std::vector<ConstPart> &parts) {
    YAML::Node node;
    for (const auto &part : parts) node[part.first] = part.second->encode();
    return node;
}
}  // namespace ssz
