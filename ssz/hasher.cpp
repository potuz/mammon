/*  hasher.cpp
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

#include "ssz/hasher.hpp"

#include <cpuid.h>


extern "C" void sha256_8_avx2(unsigned char* output, const unsigned char* input, std::size_t blocks);
extern "C" void sha256_shani(unsigned char* output, const unsigned char* input, std::size_t blocks);

namespace {
constexpr auto CPUID_LEAF = 7;
}

namespace ssz::Hasher {

SHA256_hasher best_sha256_implementation() {
    std::uint32_t a, b, c, d;  // NOLINT
    __get_cpuid_count(CPUID_LEAF, 0, &a, &b, &c, &d);
    if (b & bit_SHA) return &sha256_shani;
    return &sha256_8_avx2;
}

}  // namespace ssz::Hasher
