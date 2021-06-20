/*  attestation.cpp 
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

#include "attestation.hpp"
#include "ssz/ssz.hpp"

namespace eth
{
    std::vector<std::byte> IndexedAttestation::serialize() const
    {
        auto fixed = Bytes<4>(std::uint32_t(constants::BYTES_PER_LENGTH_OFFSET + 224)) +
                data.serialize() + signature;
        
        std::vector<std::byte> ret(fixed.to_array().cbegin(), fixed.to_array().cend());
        for (auto &idx : attesting_indices)
        {
            Bytes<8> arr (idx);
            ret.insert(ret.end(), arr.to_array().cbegin(), arr.to_array().cend());
        }
       return ret;
    } 

    std::vector<std::byte> PendingAttestation::serialize() const
    {
        auto fixed = Bytes<4>(std::uint32_t(constants::BYTES_PER_LENGTH_OFFSET + 144)) +
                data.serialize() + Bytes<8>(inclusion_delay) + Bytes<8>(proposer_index);
       
        auto aggregation_bytes = aggregation_bits.serialize(); 

        std::vector<std::byte> ret(fixed.to_array().cbegin(), fixed.to_array().cend());
        std::copy(aggregation_bytes.begin(), aggregation_bytes.end(), std::back_inserter(ret));
        return ret;
    }

    std::vector<std::byte> Attestation::serialize() const
    {
        auto fixed = Bytes<4>(std::uint32_t(constants::BYTES_PER_LENGTH_OFFSET + 224)) +
                data.serialize() + signature;
       
        auto aggregation_bytes = aggregation_bits.serialize(); 

        std::vector<std::byte> ret(fixed.to_array().cbegin(), fixed.to_array().cend());
        std::copy(aggregation_bytes.begin(), aggregation_bytes.end(), std::back_inserter(ret));
        return ret;
    }
}
