/*  ssz.hpp 
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
#include <cstddef>
#include <vector>
#include "common/bytes.hpp"

namespace constants
{
    constexpr std::uint32_t BYTES_PER_LENGTH_OFFSET = 4;
}

namespace ssz
{
    template<typename T, bool variable_parts = true>
    std::vector<std::byte> serialize_list(const std::vector<T>& vec)
    {
        std::vector<std::byte> ret;
        std::uint32_t offset = constants::BYTES_PER_LENGTH_OFFSET * vec.size();
        for (std::size_t i = 0; i < vec.size(); ++i)
        {
            auto part_ssz = vec[i].serialize();
            if (variable_parts)
            {
                eth::Bytes4 offset_ssz(offset);
                ret.insert(ret.begin() + i*constants::BYTES_PER_LENGTH_OFFSET,
                        offset_ssz.begin(), offset_ssz.end());
                offset += std::uint32_t(part_ssz.size());
            }
            ret.insert(ret.end(), part_ssz.begin(), part_ssz.end());
        }
        return ret;
    }
}

