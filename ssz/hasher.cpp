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

#include <cpuid.h>

#include "ssz/hasher.hpp"
#include "ssz/ssz.hpp"

extern "C" void sha256_1_avx(unsigned char* output, const unsigned char* input);

namespace {
constexpr auto CPUID_LEAF = 7;
}

namespace ssz {
void Hasher::sha256_sse(unsigned char* output, const unsigned char* input, std::size_t blocks) {
    while (blocks) {
        sha256_1_avx(output, input);
        input += 2*constants::BYTES_PER_CHUNK;
        output += constants::BYTES_PER_CHUNK;
        blocks--;
    }
}

const Hasher::IMPL Hasher::implemented() {
    IMPL ret = IMPL::NONE; 
    std::uint32_t a, b, c, d;  // NOLINT
    __get_cpuid_count(CPUID_LEAF, 0, &a, &b, &c, &d);
    if (b & bit_SHA) ret = ret | IMPL::SHA;
    if (b & bit_AVX2) ret = ret | IMPL::AVX2;

    __get_cpuid(1, &a, &b, &c, &d);
    if (c & bit_AVX) ret = ret |  IMPL::AVX;
    if (c & bit_SSE3) ret = ret | IMPL::SSE;

    return ret;
}

Hasher::SHA256_hasher Hasher::best_sha256_implementation() {
    auto impl = implemented(); 
    if (!!(impl & IMPL::SHA)) return &::sha256_shani;
    if (!!(impl & IMPL::AVX2)) return &::sha256_8_avx2;
    if (!!(impl & IMPL::AVX)) return &::sha256_4_avx;
    return &sha256_sse;
}

Hasher::Hasher(Hasher::IMPL impl) {
    switch (impl) {
        case IMPL::SHA:
            _hash_64b_blocks = sha256_shani;
            break;
        case IMPL::AVX2:
            _hash_64b_blocks = sha256_8_avx2;
            break;
        case IMPL::AVX: 
            _hash_64b_blocks = sha256_4_avx;
            break;
        case IMPL::SSE:
            _hash_64b_blocks = &sha256_sse;
        default:
            _hash_64b_blocks = best_sha256_implementation();
    }
}
}  // namespace ssz
