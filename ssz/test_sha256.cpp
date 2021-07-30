/*  test_sha256.cpp
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

#include <array>
#include <cstdint>
#include <vector>

#include "include/acutest.h"
#include "ssz/hasher.hpp"
#include "ssz/ssz.hpp"

using namespace ssz;
using namespace constants;


extern "C" void sha256_1_avx(unsigned char* output, const unsigned char* input);
extern "C" void sha256_4_avx(unsigned char* output, const unsigned char* input, size_t blocks);
extern "C" void sha256_oct_avx2(unsigned char* output, const unsigned char* input, size_t blocks);

namespace {

constexpr auto AVX_MAX_LANES = 4;
constexpr auto AVX2_MAX_LANES = 8;
constexpr auto SHA256_BLOCK_SIZE = 2 * BYTES_PER_CHUNK; 

constexpr std::array<std::uint8_t, AVX2_MAX_LANES * SHA256_BLOCK_SIZE> test_8_block {
    0x00, 0xb9, 0x2f, 0xfe, 0x84, 0x85, 0x5d, 0xde, 0x50, 0xe2, 0x43, 0x1e, 0x32, 0x09, 0x58, 0x90,
    0x90, 0x2e, 0xf9, 0xe7, 0x0e, 0xba, 0x4d, 0xe3, 0x40, 0x65, 0xec, 0x61, 0xd7, 0x8a, 0x09, 0x49,
    0x0b, 0x4d, 0x0b, 0x5c, 0xa3, 0x6a, 0x2b, 0x5f, 0xe7, 0x2e, 0x68, 0xde, 0x54, 0x97, 0x0d, 0x33,
    0x4c, 0x35, 0x65, 0x96, 0x29, 0x71, 0xcb, 0x65, 0x07, 0xa8, 0x62, 0x2b, 0x8d, 0x73, 0x1a, 0x3d,
    0x28, 0x02, 0x5f, 0x8d, 0x47, 0xcc, 0x3c, 0xf9, 0x6d, 0x50, 0x35, 0x7b, 0xb3, 0x5d, 0xd3, 0xa7,
    0x17, 0x73, 0x5c, 0x5e, 0x99, 0x28, 0x39, 0x71, 0xba, 0x83, 0x1d, 0x54, 0xa5, 0x4f, 0xe5, 0xde,
    0x51, 0x20, 0x5e, 0x01, 0x00, 0xb6, 0x12, 0xf1, 0x4f, 0x3f, 0x3f, 0x64, 0x8d, 0x4f, 0xa7, 0xca,
    0x8c, 0x06, 0x63, 0x3d, 0xdd, 0x91, 0x44, 0xbb, 0x5b, 0x0b, 0xb0, 0xa6, 0x81, 0x87, 0x22, 0xbd,
    0xe2, 0x3f, 0x20, 0xb3, 0x02, 0x64, 0x8b, 0x5b, 0x14, 0x53, 0xa8, 0x2a, 0x7a, 0x3e, 0x9d, 0x29,
    0x51, 0xf8, 0xfc, 0x94, 0x6d, 0xc6, 0x0e, 0x63, 0x1e, 0xc5, 0xf6, 0x69, 0x1a, 0x3f, 0x94, 0xaa,
    0xa9, 0x0f, 0x8f, 0x58, 0x4f, 0x35, 0x21, 0xf1, 0x52, 0x33, 0xd3, 0x07, 0x13, 0x52, 0xd4, 0x7b,
    0x30, 0x74, 0x64, 0x12, 0x53, 0x8c, 0x9e, 0xa8, 0x9c, 0x61, 0xb2, 0x91, 0x2c, 0x35, 0x6d, 0xa9,
    0x66, 0x47, 0xc9, 0x60, 0xa1, 0x30, 0x6d, 0xf4, 0x0f, 0x7d, 0xe4, 0x36, 0x2f, 0xbc, 0xbd, 0x4f,
    0xe4, 0xf6, 0x6d, 0x05, 0xd8, 0x0f, 0x25, 0x6c, 0x35, 0x11, 0x4d, 0x08, 0x14, 0x5d, 0xe9, 0xae,
    0x32, 0x99, 0xd0, 0xb4, 0x87, 0x8e, 0x49, 0xc3, 0x70, 0xf1, 0xd8, 0x79, 0x38, 0x77, 0xa7, 0xea,
    0x12, 0xb9, 0xe5, 0x50, 0xac, 0xd4, 0x16, 0xd4, 0x93, 0x38, 0x10, 0xef, 0xf8, 0x9c, 0x2b, 0x5b,
    0xbc, 0xde, 0x47, 0x45, 0xa2, 0x6b, 0xac, 0x7a, 0xab, 0x25, 0x68, 0x60, 0xf2, 0x37, 0x12, 0x81,
    0x30, 0xd1, 0x85, 0x60, 0x0e, 0xef, 0x91, 0x5f, 0xe4, 0x52, 0x23, 0xbe, 0x00, 0x8a, 0x12, 0xf8,
    0xe8, 0x6f, 0x70, 0xb5, 0x1d, 0xee, 0x47, 0xec, 0x80, 0x5f, 0x9d, 0xa3, 0x10, 0x03, 0x56, 0x61,
    0xdb, 0xb9, 0x56, 0x60, 0x4e, 0x15, 0x99, 0x2a, 0x57, 0x94, 0xc9, 0x9c, 0x73, 0x67, 0x16, 0x4f,
    0xa5, 0x2e, 0xe9, 0x21, 0xe0, 0x50, 0x49, 0x5d, 0x43, 0x06, 0xb1, 0x4f, 0x81, 0x36, 0xd3, 0x24,
    0xb5, 0x38, 0x86, 0xf4, 0x35, 0x51, 0x83, 0xfd, 0xa4, 0xc8, 0xba, 0x03, 0xd0, 0x38, 0xdb, 0x19,
    0x21, 0x1f, 0x89, 0x20, 0x44, 0x50, 0x14, 0x25, 0x02, 0xa4, 0xe9, 0x77, 0x65, 0x08, 0x87, 0x66,
    0x1e, 0x4f, 0x0d, 0xef, 0xf7, 0xda, 0x9b, 0x6d, 0xeb, 0xf2, 0x23, 0x1f, 0xee, 0xa7, 0xec, 0xc7,
    0xe9, 0xe1, 0x68, 0x6c, 0x3e, 0x0f, 0x3b, 0xf4, 0x1e, 0x12, 0x05, 0xf8, 0x6f, 0x3d, 0xdc, 0x0e,
    0x7f, 0xce, 0x45, 0x5a, 0xd1, 0xc1, 0x50, 0x9b, 0xe6, 0xff, 0x09, 0x16, 0x54, 0x93, 0x78, 0xbf,
    0x71, 0x28, 0x9c, 0x64, 0x03, 0xb3, 0x74, 0xf7, 0xe9, 0x2d, 0x91, 0x80, 0xac, 0x56, 0x39, 0x42,
    0x9a, 0xe4, 0x1d, 0x13, 0xf2, 0xa1, 0x70, 0xb1, 0x23, 0x76, 0xb0, 0x13, 0x3b, 0xc1, 0x1c, 0xb3,
    0xa6, 0xa7, 0x2e, 0x2c, 0x5a, 0xad, 0x57, 0x0f, 0x7b, 0x79, 0xf7, 0x45, 0xf2, 0xff, 0x88, 0x84,
    0x2a, 0x39, 0x83, 0x4b, 0xba, 0xa5, 0x8f, 0x77, 0x08, 0x67, 0x3a, 0x15, 0x2d, 0xbc, 0x2e, 0x24,
    0xb0, 0x58, 0x2c, 0xf9, 0x55, 0xe3, 0xac, 0x3a, 0xc5, 0x54, 0x74, 0xbb, 0x1f, 0xe7, 0x1e, 0xef,
    0x06, 0x46, 0xea, 0xec, 0x5e, 0x46, 0x91, 0xcb, 0xbc, 0x75, 0xb1, 0xbe, 0xec, 0xd0, 0x2b, 0xf4
};

constexpr Chunk test_1_digest {
    0xd3, 0x12, 0x8b, 0xe4, 0x5f, 0xb5, 0x78, 0xd0, 0x7f, 0x94, 0x9d, 0x3e, 0x77, 0x77, 0x84, 0x93, 
    0xe8, 0x41, 0x78, 0xdb, 0x0d, 0x6d, 0xfb, 0x91, 0x8e, 0x30, 0x00, 0xef, 0xec, 0x7b, 0xe4, 0x0a
};

constexpr std::array<std::uint8_t, AVX_MAX_LANES * BYTES_PER_CHUNK> test_4_digests {
    0xd3, 0x12, 0x8b, 0xe4, 0x5f, 0xb5, 0x78, 0xd0, 0x7f, 0x94, 0x9d, 0x3e, 0x77, 0x77, 0x84, 0x93, 
    0xe8, 0x41, 0x78, 0xdb, 0x0d, 0x6d, 0xfb, 0x91, 0x8e, 0x30, 0x00, 0xef, 0xec, 0x7b, 0xe4, 0x0a,
    0xe7, 0x76, 0xb2, 0xe7, 0x8c, 0x8f, 0xba, 0x6c, 0xad, 0xa3, 0xf2, 0x0b, 0xb4, 0x92, 0x2d, 0xc2,
    0x42, 0x83, 0xf2, 0xd5, 0x67, 0xd6, 0x02, 0x65, 0xc1, 0x6a, 0x78, 0x9a, 0x7c, 0x87, 0x91, 0xca,
    0xf8, 0x13, 0x99, 0x16, 0x2e, 0x62, 0x20, 0xa9, 0x25, 0xc0, 0x84, 0xc1, 0xe6, 0x1f, 0x35, 0xdc,
    0x79, 0x37, 0x49, 0x32, 0xf7, 0xf5, 0xfb, 0x78, 0x49, 0xa5, 0xf4, 0x34, 0x8e, 0xfa, 0xc8, 0x9f,
    0xae, 0x27, 0x9b, 0x28, 0x57, 0x14, 0x12, 0x21, 0x7f, 0x9d, 0xf6, 0x56, 0xbd, 0x03, 0x28, 0x0d,
    0x9e, 0x11, 0xd0, 0x05, 0x1c, 0x8e, 0x98, 0xf1, 0xdb, 0x83, 0xc1, 0xbe, 0x0d, 0x08, 0x65, 0x1a
};

constexpr std::array<std::uint8_t, AVX2_MAX_LANES * BYTES_PER_CHUNK> test_8_digests {
    0xd3, 0x12, 0x8b, 0xe4, 0x5f, 0xb5, 0x78, 0xd0, 0x7f, 0x94, 0x9d, 0x3e, 0x77, 0x77, 0x84, 0x93, 
    0xe8, 0x41, 0x78, 0xdb, 0x0d, 0x6d, 0xfb, 0x91, 0x8e, 0x30, 0x00, 0xef, 0xec, 0x7b, 0xe4, 0x0a,
    0xe7, 0x76, 0xb2, 0xe7, 0x8c, 0x8f, 0xba, 0x6c, 0xad, 0xa3, 0xf2, 0x0b, 0xb4, 0x92, 0x2d, 0xc2,
    0x42, 0x83, 0xf2, 0xd5, 0x67, 0xd6, 0x02, 0x65, 0xc1, 0x6a, 0x78, 0x9a, 0x7c, 0x87, 0x91, 0xca,
    0xf8, 0x13, 0x99, 0x16, 0x2e, 0x62, 0x20, 0xa9, 0x25, 0xc0, 0x84, 0xc1, 0xe6, 0x1f, 0x35, 0xdc,
    0x79, 0x37, 0x49, 0x32, 0xf7, 0xf5, 0xfb, 0x78, 0x49, 0xa5, 0xf4, 0x34, 0x8e, 0xfa, 0xc8, 0x9f,
    0xae, 0x27, 0x9b, 0x28, 0x57, 0x14, 0x12, 0x21, 0x7f, 0x9d, 0xf6, 0x56, 0xbd, 0x03, 0x28, 0x0d,
    0x9e, 0x11, 0xd0, 0x05, 0x1c, 0x8e, 0x98, 0xf1, 0xdb, 0x83, 0xc1, 0xbe, 0x0d, 0x08, 0x65, 0x1a,
    0x0b, 0x3a, 0x42, 0xef, 0xfe, 0xdf, 0xea, 0x2f, 0x9a, 0x72, 0xd9, 0xeb, 0x19, 0xac, 0x5a, 0xcf,
    0x64, 0x25, 0x75, 0x09, 0xcd, 0xf5, 0x19, 0x61, 0x4a, 0xcf, 0x9d, 0xdb, 0x2c, 0x48, 0xf5, 0xa9,
    0x80, 0xaa, 0x69, 0x47, 0x67, 0x02, 0xd3, 0x3f, 0xa8, 0x82, 0x8b, 0xe9, 0x60, 0xcf, 0xe8, 0x02,
    0x05, 0xcf, 0xaa, 0x66, 0x3d, 0xeb, 0x85, 0x0a, 0xe2, 0xcc, 0x21, 0x87, 0x54, 0x42, 0x6c, 0xdd,
    0x4f, 0x39, 0xb4, 0xd8, 0xad, 0xd8, 0xce, 0x78, 0x18, 0xda, 0xbe, 0x57, 0x41, 0x9b, 0x4a, 0x8f,
    0xa1, 0x9d, 0x58, 0xfe, 0x2f, 0x6e, 0x71, 0xd4, 0xa0, 0x37, 0xaf, 0x04, 0x9d, 0x6f, 0x60, 0x29,
    0x0a, 0x39, 0x4a, 0x84, 0x2e, 0x81, 0x24, 0x5e, 0xde, 0xb6, 0xc9, 0x63, 0x7b, 0x82, 0xf1, 0xc3,
    0xde, 0x07, 0x14, 0x82, 0x27, 0x25, 0x03, 0x46, 0x7e, 0x6b, 0x09, 0x4d, 0x41, 0x98, 0x82, 0xb9
};

} // namespace

void test_hash_avx_1() {
    std::array<std::uint8_t, BYTES_PER_CHUNK> digest {};
    sha256_1_avx(digest.begin(), test_8_block.begin());

    TEST_CHECK(digest == test_1_digest);
    
}

void test_hash_avx_4() {    
    std::array<std::uint8_t, AVX_MAX_LANES * BYTES_PER_CHUNK> digest {};
    sha256_4_avx(digest.begin(), test_8_block.begin(), AVX_MAX_LANES);

    TEST_CHECK(digest == test_4_digests);
}


void test_hash_avx2_8() {    
    std::array<std::uint8_t, AVX2_MAX_LANES * BYTES_PER_CHUNK> digest {};
    sha256_oct_avx2(digest.begin(), test_8_block.begin(), AVX2_MAX_LANES);

    TEST_CHECK(digest == test_8_digests);
}

void test_hasher() {
    std::array<std::uint8_t, AVX2_MAX_LANES * BYTES_PER_CHUNK> digest {};
    ssz::Hasher::hash_64b_blocks(digest.begin(), test_8_block.begin(), AVX2_MAX_LANES);

    TEST_CHECK(digest == test_8_digests);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)

TEST_LIST = {{"hash_avx_1", test_hash_avx_1},
             {"hash_avx_4", test_hash_avx_4},
             {"hash_avx2_8", test_hash_avx2_8},
             {"hasher", test_hasher},
             {NULL, NULL}};
