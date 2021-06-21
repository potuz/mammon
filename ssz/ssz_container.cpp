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
#include "ssz/ssz.hpp"
#include "common/bytes.hpp" 
#include <algorithm>
#include <stdexcept>
#include <cstdint>

namespace ssz
{
    std::vector<std::byte> Container::serialize_(std::vector<const Container*> parts)
    {
        //Check if we are one of the basic types
        if (parts.size() == 0)
            return {};

        //Compute the total length of the fixed parts including the offsets and the number of offsets
        std::uint32_t fixed_length = 0;
        auto sum_lengths = [&fixed_length](const Container* part)
        {
            if (part->size_ == 0)
            {
                fixed_length += constants::BYTES_PER_LENGTH_OFFSET;
            } else {
                fixed_length += std::uint32_t(part->size_);
            }
        };
        std::for_each(parts.cbegin(), parts.cend(), sum_lengths);
        
        //Insert the fixed parts and the offsets
        std::vector<std::byte> ret, variable_part;
        for(auto * part : parts)
        {
            auto part_ssz = part->serialize();
            if (part->size_ == 0)
            {
                eth::Bytes4 offset (fixed_length);
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

}
