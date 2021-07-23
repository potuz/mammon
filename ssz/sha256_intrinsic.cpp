/*  sha256_intrinsic.cpp
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

#include <algorithm>
#include <cstdint>
#include <immintrin.h>
#include "ssz/ssz.hpp"

namespace {

alignas(__m128i) const std::uint32_t digest[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

alignas(__m128i) const unsigned char padding0 [16] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
alignas(__m128i) const unsigned char padding1 [16] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
alignas(__m128i) const unsigned char padding2 [16] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0};

} // namespace


void hash_64b_blocks(unsigned char* output, const unsigned char* input, size_t blocks) {
   __m128i statea0, statea1, stateb0, stateb1;
   __m128i msga, msgb;
   __m128i msgtmpa0, msgtmpa1, msgtmpa2, msgtmpa3;
   __m128i msgtmpb0, msgtmpb1, msgtmpb2, msgtmpb3;
   __m128i tmpa, tmpb;
   __m128i shuf_mask;
   __m128i abef_savea, cdgh_savea;
   __m128i abef_saveb, cdgh_saveb;

   shuf_mask = _mm_set_epi64x(0x0c0d0e0f08090a0bull, 0x0405060700010203ull);
/*
   while (blocks > 1) {
      
   // Load initial hash values
   // Need to reorder these appropriately
   // DCBA, HGFE -> ABEF, CDGH
   tmpa = _mm_load_si128((__m128i*) digest);
   statea1 = _mm_load_si128((__m128i*) (digest + 4));

   tmpa = _mm_shuffle_epi32(tmpa, 0xB1);       // CDAB
   statea1 = _mm_shuffle_epi32(statea1, 0x1B);    // EFGH
   statea0 = _mm_alignr_epi8(tmpa, statea1, 8);    // ABEF
   statea1 = _mm_blend_epi16(statea1, tmpa, 0xF0); // CDGH
   stateb0 = statea0;
   stateb1 = statea1;


      // Save hash values for addition after rounds
      abef_savea = statea0;
      abef_saveb = stateb0;
      cdgh_savea = statea1;
      cdgh_saveb = stateb1;

      // Rounds 0-3
      msga     = _mm_loadu_si128((__m128i*) input);
      msgb     = _mm_loadu_si128((__m128i*) input+64);
      msgtmpa0 = _mm_shuffle_epi8(msga, shuf_mask);
      msgtmpb0 = _mm_shuffle_epi8(msgb, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0xE9B5DBA5B5C0FBCFull, 0x71374491428A2F98ull));
         msgb    = _mm_add_epi32(msgtmpb0, 
                  _mm_set_epi64x(0xE9B5DBA5B5C0FBCFull, 0x71374491428A2F98ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);

      // Rounds 4-7
      msgtmpa1 = _mm_loadu_si128((__m128i*) (input+16));
      msgtmpb1 = _mm_loadu_si128((__m128i*) (input+80));
      msgtmpa1 = _mm_shuffle_epi8(msgtmpa1, shuf_mask);
      msgtmpb1 = _mm_shuffle_epi8(msgtmpb1, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0xAB1C5ED5923F82A4ull, 0x59F111F13956C25Bull));
         msgb    = _mm_add_epi32(msgtmpb1, 
                  _mm_set_epi64x(0xAB1C5ED5923F82A4ull, 0x59F111F13956C25Bull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
         msga    = _mm_shuffle_epi32(msga, 0xE);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);
      msgtmpb0 = _mm_sha256msg1_epu32(msgtmpb0, msgtmpb1);

      // Rounds 8-11
      msgtmpa2 = _mm_loadu_si128((__m128i*) (input+32));
      msgtmpb2 = _mm_loadu_si128((__m128i*) (input+96));
      msgtmpa2 = _mm_shuffle_epi8(msgtmpa2, shuf_mask);
      msgtmpb2 = _mm_shuffle_epi8(msgtmpb2, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0x550C7DC3243185BEull, 0x12835B01D807AA98ull));
         msgb    = _mm_add_epi32(msgtmpb2, 
                  _mm_set_epi64x(0x550C7DC3243185BEull, 0x12835B01D807AA98ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);
      msgtmpb1 = _mm_sha256msg1_epu32(msgtmpb1, msgtmpb2);

      // Rounds 12-15
      msgtmpa3 = _mm_loadu_si128((__m128i*) (input+48));
      msgtmpb3 = _mm_loadu_si128((__m128i*) (input+112));
      msgtmpa3 = _mm_shuffle_epi8(msgtmpa3, shuf_mask);
      msgtmpb3 = _mm_shuffle_epi8(msgtmpb3, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0xC19BF1749BDC06A7ull, 0x80DEB1FE72BE5D74ull));
         msgb    = _mm_add_epi32(msgtmpb3, 
                  _mm_set_epi64x(0xC19BF1749BDC06A7ull, 0x80DEB1FE72BE5D74ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb3, msgtmpb2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpb0 = _mm_add_epi32(msgtmpb0, tmpb);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
      msgtmpb0 = _mm_sha256msg2_epu32(msgtmpb0, msgtmpb3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);
      msgtmpb2 = _mm_sha256msg1_epu32(msgtmpb2, msgtmpb3);

      // Rounds 16-19
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x240CA1CC0FC19DC6ull, 0xEFBE4786E49B69C1ull));
         msgb    = _mm_add_epi32(msgtmpb0, 
                  _mm_set_epi64x(0x240CA1CC0FC19DC6ull, 0xEFBE4786E49B69C1ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb0, msgtmpb3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpb1 = _mm_add_epi32(msgtmpb1, tmpb);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
      msgtmpb1 = _mm_sha256msg2_epu32(msgtmpb1, msgtmpb0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);
      msgtmpb3 = _mm_sha256msg1_epu32(msgtmpb3, msgtmpb0);

      // Rounds 20-23
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x76F988DA5CB0A9DCull, 0x4A7484AA2DE92C6Full));
         msgb    = _mm_add_epi32(msgtmpb1, 
                  _mm_set_epi64x(0x76F988DA5CB0A9DCull, 0x4A7484AA2DE92C6Full));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb1, msgtmpb0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpb2 = _mm_add_epi32(msgtmpb2, tmpb);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
      msgtmpb2 = _mm_sha256msg2_epu32(msgtmpb2, msgtmpb1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);
      msgtmpb0 = _mm_sha256msg1_epu32(msgtmpb0, msgtmpb1);

      // Rounds 24-27
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0xBF597FC7B00327C8ull, 0xA831C66D983E5152ull));
         msgb    = _mm_add_epi32(msgtmpb2, 
                  _mm_set_epi64x(0xBF597FC7B00327C8ull, 0xA831C66D983E5152ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb2, msgtmpb1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpb3 = _mm_add_epi32(msgtmpb3, tmpb);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
      msgtmpb3 = _mm_sha256msg2_epu32(msgtmpb3, msgtmpb2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);
      msgtmpb1 = _mm_sha256msg1_epu32(msgtmpb1, msgtmpb2);

      // Rounds 28-31
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0x1429296706CA6351ull, 0xD5A79147C6E00BF3ull));
         msgb    = _mm_add_epi32(msgtmpb3, 
                  _mm_set_epi64x(0x1429296706CA6351ull, 0xD5A79147C6E00BF3ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb3, msgtmpb2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpb0 = _mm_add_epi32(msgtmpb0, tmpb);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
      msgtmpb0 = _mm_sha256msg2_epu32(msgtmpb0, msgtmpb3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);
      msgtmpb2 = _mm_sha256msg1_epu32(msgtmpb2, msgtmpb3);

      // Rounds 32-35
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x53380D134D2C6DFCull, 0x2E1B213827B70A85ull));
         msgb    = _mm_add_epi32(msgtmpb0, 
                  _mm_set_epi64x(0x53380D134D2C6DFCull, 0x2E1B213827B70A85ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb0, msgtmpb3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpb1 = _mm_add_epi32(msgtmpb1, tmpb);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
      msgtmpb1 = _mm_sha256msg2_epu32(msgtmpb1, msgtmpb0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);
      msgtmpb3 = _mm_sha256msg1_epu32(msgtmpb3, msgtmpb0);

      // Rounds 36-39
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x92722C8581C2C92Eull, 0x766A0ABB650A7354ull));
         msgb    = _mm_add_epi32(msgtmpb1, 
                  _mm_set_epi64x(0x92722C8581C2C92Eull, 0x766A0ABB650A7354ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb1, msgtmpb0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpb2 = _mm_add_epi32(msgtmpb2, tmpb);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
      msgtmpb2 = _mm_sha256msg2_epu32(msgtmpb2, msgtmpb1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);
      msgtmpb0 = _mm_sha256msg1_epu32(msgtmpb0, msgtmpb1);

      // Rounds 40-43
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0xC76C51A3C24B8B70ull, 0xA81A664BA2BFE8A1ull));
         msgb    = _mm_add_epi32(msgtmpb2, 
                  _mm_set_epi64x(0xC76C51A3C24B8B70ull, 0xA81A664BA2BFE8A1ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb2, msgtmpb1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpb3 = _mm_add_epi32(msgtmpb3, tmpb);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
      msgtmpb3 = _mm_sha256msg2_epu32(msgtmpb3, msgtmpb2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);
      msgtmpb1 = _mm_sha256msg1_epu32(msgtmpb1, msgtmpb2);

      // Rounds 44-47
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0x106AA070F40E3585ull, 0xD6990624D192E819ull));
         msgb    = _mm_add_epi32(msgtmpb3, 
                  _mm_set_epi64x(0x106AA070F40E3585ull, 0xD6990624D192E819ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb3, msgtmpb2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpb0 = _mm_add_epi32(msgtmpb0, tmpb);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
      msgtmpb0 = _mm_sha256msg2_epu32(msgtmpb0, msgtmpb3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);
      msgtmpb2 = _mm_sha256msg1_epu32(msgtmpb2, msgtmpb3);

      // Rounds 48-51
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x34B0BCB52748774Cull, 0x1E376C0819A4C116ull));
         msgb    = _mm_add_epi32(msgtmpb0, 
                  _mm_set_epi64x(0x34B0BCB52748774Cull, 0x1E376C0819A4C116ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb0, msgtmpb3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpb1 = _mm_add_epi32(msgtmpb1, tmpb);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
      msgtmpb1 = _mm_sha256msg2_epu32(msgtmpb1, msgtmpb0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);
      msgtmpb3 = _mm_sha256msg1_epu32(msgtmpb3, msgtmpb0);

      // Rounds 52-55
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x682E6FF35B9CCA4Full, 0x4ED8AA4A391C0CB3ull));
         msgb    = _mm_add_epi32(msgtmpb1, 
                  _mm_set_epi64x(0x682E6FF35B9CCA4Full, 0x4ED8AA4A391C0CB3ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb1, msgtmpb0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpb2 = _mm_add_epi32(msgtmpb2, tmpb);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
      msgtmpb2 = _mm_sha256msg2_epu32(msgtmpb2, msgtmpb1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);

      // Rounds 56-59
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0x8CC7020884C87814ull, 0x78A5636F748F82EEull));
         msgb    = _mm_add_epi32(msgtmpb2, 
                  _mm_set_epi64x(0x8CC7020884C87814ull, 0x78A5636F748F82EEull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb2, msgtmpb1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpb3 = _mm_add_epi32(msgtmpb3, tmpb);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
      msgtmpb3 = _mm_sha256msg2_epu32(msgtmpb3, msgtmpb2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);

      // Rounds 60-63
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0xC67178F2BEF9A3F7ull, 0xA4506CEB90BEFFFAull));
         msgb    = _mm_add_epi32(msgtmpb3, 
                  _mm_set_epi64x(0xC67178F2BEF9A3F7ull, 0xA4506CEB90BEFFFAull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);

      // Add current hash values with previously saved
      statea0 = _mm_add_epi32(statea0, abef_savea);
      stateb0 = _mm_add_epi32(stateb0, abef_saveb);
      statea1 = _mm_add_epi32(statea1, cdgh_savea);
      stateb1 = _mm_add_epi32(stateb1, cdgh_saveb);

      // Round with padding
      //
      // Save hash values for addition after rounds
      abef_savea = statea0;
      abef_saveb = stateb0;
      cdgh_savea = statea1;
      cdgh_saveb = stateb1;

      // Rounds 0-3
      msga     = _mm_loadu_si128((__m128i*) padding0);
      msgb     = _mm_loadu_si128((__m128i*) padding0);
      msgtmpa0 = _mm_shuffle_epi8(msga, shuf_mask);
      msgtmpb0 = _mm_shuffle_epi8(msgb, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0xE9B5DBA5B5C0FBCFull, 0x71374491428A2F98ull));
         msgb    = _mm_add_epi32(msgtmpb0, 
                  _mm_set_epi64x(0xE9B5DBA5B5C0FBCFull, 0x71374491428A2F98ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);

      // Rounds 4-7
      msgtmpa1 = _mm_loadu_si128((__m128i*) padding1);
      msgtmpb1 = _mm_loadu_si128((__m128i*) padding1);
      msgtmpa1 = _mm_shuffle_epi8(msgtmpa1, shuf_mask);
      msgtmpb1 = _mm_shuffle_epi8(msgtmpb1, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0xAB1C5ED5923F82A4ull, 0x59F111F13956C25Bull));
         msgb    = _mm_add_epi32(msgtmpb1, 
                  _mm_set_epi64x(0xAB1C5ED5923F82A4ull, 0x59F111F13956C25Bull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
         msga    = _mm_shuffle_epi32(msga, 0xE);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);
      msgtmpb0 = _mm_sha256msg1_epu32(msgtmpb0, msgtmpb1);

      // Rounds 8-11
      msgtmpa2 = _mm_loadu_si128((__m128i*) padding1);
      msgtmpb2 = _mm_loadu_si128((__m128i*) padding1);
      msgtmpa2 = _mm_shuffle_epi8(msgtmpa2, shuf_mask);
      msgtmpb2 = _mm_shuffle_epi8(msgtmpb2, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0x550C7DC3243185BEull, 0x12835B01D807AA98ull));
         msgb    = _mm_add_epi32(msgtmpb2, 
                  _mm_set_epi64x(0x550C7DC3243185BEull, 0x12835B01D807AA98ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);
      msgtmpb1 = _mm_sha256msg1_epu32(msgtmpb1, msgtmpb2);

      // Rounds 12-15
      msgtmpa3 = _mm_loadu_si128((__m128i*) padding2);
      msgtmpb3 = _mm_loadu_si128((__m128i*) padding2);
      msgtmpa3 = _mm_shuffle_epi8(msgtmpa3, shuf_mask);
      msgtmpb3 = _mm_shuffle_epi8(msgtmpb3, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0xC19BF1749BDC06A7ull, 0x80DEB1FE72BE5D74ull));
         msgb    = _mm_add_epi32(msgtmpb3, 
                  _mm_set_epi64x(0xC19BF1749BDC06A7ull, 0x80DEB1FE72BE5D74ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb3, msgtmpb2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpb0 = _mm_add_epi32(msgtmpb0, tmpb);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
      msgtmpb0 = _mm_sha256msg2_epu32(msgtmpb0, msgtmpb3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);
      msgtmpb2 = _mm_sha256msg1_epu32(msgtmpb2, msgtmpb3);

      // Rounds 16-19
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x240CA1CC0FC19DC6ull, 0xEFBE4786E49B69C1ull));
         msgb    = _mm_add_epi32(msgtmpb0, 
                  _mm_set_epi64x(0x240CA1CC0FC19DC6ull, 0xEFBE4786E49B69C1ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb0, msgtmpb3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpb1 = _mm_add_epi32(msgtmpb1, tmpb);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
      msgtmpb1 = _mm_sha256msg2_epu32(msgtmpb1, msgtmpb0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);
      msgtmpb3 = _mm_sha256msg1_epu32(msgtmpb3, msgtmpb0);

      // Rounds 20-23
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x76F988DA5CB0A9DCull, 0x4A7484AA2DE92C6Full));
         msgb    = _mm_add_epi32(msgtmpb1, 
                  _mm_set_epi64x(0x76F988DA5CB0A9DCull, 0x4A7484AA2DE92C6Full));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb1, msgtmpb0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpb2 = _mm_add_epi32(msgtmpb2, tmpb);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
      msgtmpb2 = _mm_sha256msg2_epu32(msgtmpb2, msgtmpb1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);
      msgtmpb0 = _mm_sha256msg1_epu32(msgtmpb0, msgtmpb1);

      // Rounds 24-27
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0xBF597FC7B00327C8ull, 0xA831C66D983E5152ull));
         msgb    = _mm_add_epi32(msgtmpb2, 
                  _mm_set_epi64x(0xBF597FC7B00327C8ull, 0xA831C66D983E5152ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb2, msgtmpb1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpb3 = _mm_add_epi32(msgtmpb3, tmpb);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
      msgtmpb3 = _mm_sha256msg2_epu32(msgtmpb3, msgtmpb2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);
      msgtmpb1 = _mm_sha256msg1_epu32(msgtmpb1, msgtmpb2);

      // Rounds 28-31
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0x1429296706CA6351ull, 0xD5A79147C6E00BF3ull));
         msgb    = _mm_add_epi32(msgtmpb3, 
                  _mm_set_epi64x(0x1429296706CA6351ull, 0xD5A79147C6E00BF3ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb3, msgtmpb2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpb0 = _mm_add_epi32(msgtmpb0, tmpb);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
      msgtmpb0 = _mm_sha256msg2_epu32(msgtmpb0, msgtmpb3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);
      msgtmpb2 = _mm_sha256msg1_epu32(msgtmpb2, msgtmpb3);

      // Rounds 32-35
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x53380D134D2C6DFCull, 0x2E1B213827B70A85ull));
         msgb    = _mm_add_epi32(msgtmpb0, 
                  _mm_set_epi64x(0x53380D134D2C6DFCull, 0x2E1B213827B70A85ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb0, msgtmpb3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpb1 = _mm_add_epi32(msgtmpb1, tmpb);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
      msgtmpb1 = _mm_sha256msg2_epu32(msgtmpb1, msgtmpb0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);
      msgtmpb3 = _mm_sha256msg1_epu32(msgtmpb3, msgtmpb0);

      // Rounds 36-39
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x92722C8581C2C92Eull, 0x766A0ABB650A7354ull));
         msgb    = _mm_add_epi32(msgtmpb1, 
                  _mm_set_epi64x(0x92722C8581C2C92Eull, 0x766A0ABB650A7354ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb1, msgtmpb0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpb2 = _mm_add_epi32(msgtmpb2, tmpb);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
      msgtmpb2 = _mm_sha256msg2_epu32(msgtmpb2, msgtmpb1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);
      msgtmpb0 = _mm_sha256msg1_epu32(msgtmpb0, msgtmpb1);

      // Rounds 40-43
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0xC76C51A3C24B8B70ull, 0xA81A664BA2BFE8A1ull));
         msgb    = _mm_add_epi32(msgtmpb2, 
                  _mm_set_epi64x(0xC76C51A3C24B8B70ull, 0xA81A664BA2BFE8A1ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb2, msgtmpb1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpb3 = _mm_add_epi32(msgtmpb3, tmpb);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
      msgtmpb3 = _mm_sha256msg2_epu32(msgtmpb3, msgtmpb2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);
      msgtmpb1 = _mm_sha256msg1_epu32(msgtmpb1, msgtmpb2);

      // Rounds 44-47
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0x106AA070F40E3585ull, 0xD6990624D192E819ull));
         msgb    = _mm_add_epi32(msgtmpb3, 
                  _mm_set_epi64x(0x106AA070F40E3585ull, 0xD6990624D192E819ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb3, msgtmpb2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpb0 = _mm_add_epi32(msgtmpb0, tmpb);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
      msgtmpb0 = _mm_sha256msg2_epu32(msgtmpb0, msgtmpb3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);
      msgtmpb2 = _mm_sha256msg1_epu32(msgtmpb2, msgtmpb3);

      // Rounds 48-51
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x34B0BCB52748774Cull, 0x1E376C0819A4C116ull));
         msgb    = _mm_add_epi32(msgtmpb0, 
                  _mm_set_epi64x(0x34B0BCB52748774Cull, 0x1E376C0819A4C116ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb0, msgtmpb3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpb1 = _mm_add_epi32(msgtmpb1, tmpb);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
      msgtmpb1 = _mm_sha256msg2_epu32(msgtmpb1, msgtmpb0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);
      msgtmpb3 = _mm_sha256msg1_epu32(msgtmpb3, msgtmpb0);

      // Rounds 52-55
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x682E6FF35B9CCA4Full, 0x4ED8AA4A391C0CB3ull));
         msgb    = _mm_add_epi32(msgtmpb1, 
                  _mm_set_epi64x(0x682E6FF35B9CCA4Full, 0x4ED8AA4A391C0CB3ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb1, msgtmpb0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpb2 = _mm_add_epi32(msgtmpb2, tmpb);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
      msgtmpb2 = _mm_sha256msg2_epu32(msgtmpb2, msgtmpb1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);

      // Rounds 56-59
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0x8CC7020884C87814ull, 0x78A5636F748F82EEull));
         msgb    = _mm_add_epi32(msgtmpb2, 
                  _mm_set_epi64x(0x8CC7020884C87814ull, 0x78A5636F748F82EEull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      tmpb     = _mm_alignr_epi8(msgtmpb2, msgtmpb1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpb3 = _mm_add_epi32(msgtmpb3, tmpb);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
      msgtmpb3 = _mm_sha256msg2_epu32(msgtmpb3, msgtmpb2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);

      // Rounds 60-63
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0xC67178F2BEF9A3F7ull, 0xA4506CEB90BEFFFAull));
         msgb    = _mm_add_epi32(msgtmpb3, 
                  _mm_set_epi64x(0xC67178F2BEF9A3F7ull, 0xA4506CEB90BEFFFAull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         stateb1 = _mm_sha256rnds2_epu32(stateb1, stateb0, msgb);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         msgb    = _mm_shuffle_epi32(msgb, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
         stateb0 = _mm_sha256rnds2_epu32(stateb0, stateb1, msgb);

      // Add current hash values with previously saved
      statea0 = _mm_add_epi32(statea0, abef_savea);
      stateb0 = _mm_add_epi32(stateb0, abef_saveb);
      statea1 = _mm_add_epi32(statea1, cdgh_savea);
      stateb1 = _mm_add_epi32(stateb1, cdgh_saveb);

       // Write hash values back in the correct order
       tmpa    = _mm_shuffle_epi32(statea0, 0x1B);    // FEBA
       tmpb    = _mm_shuffle_epi32(stateb0, 0x1B);    // FEBA
       statea1 = _mm_shuffle_epi32(statea1, 0xB1);    // DCHG
       stateb1 = _mm_shuffle_epi32(stateb1, 0xB1);    // DCHG
       statea0 = _mm_blend_epi16(tmpa, statea1, 0xF0); // DCBA
       stateb0 = _mm_blend_epi16(tmpb, stateb1, 0xF0); // DCBA
       statea1 = _mm_alignr_epi8(statea1, tmpa, 8);    // ABEF
       stateb1 = _mm_alignr_epi8(stateb1, tmpb, 8);    // ABEF

       _mm_storeu_si128((__m128i*) output, statea0);
       _mm_storeu_si128((__m128i*) (output+16), statea1);
       _mm_storeu_si128((__m128i*) (output+32), stateb0);
       _mm_storeu_si128((__m128i*) (output+48), stateb1);

       for (int i = 0; i < 4; i++) {
           std::reverse(output + 4*i, output + 4*i + 4);
           std::reverse(output + 4*i + 16, output + 4*i + 20);
           std::reverse(output + 4*i + 32, output + 4*i + 36);
           std::reverse(output + 4*i + 48, output + 4*i + 52);
       }
       output += 64;
      input += 128;
      blocks -= 2;
   }
*/
   while (blocks) {
   // Load initial hash values
   // Need to reorder these appropriately
   // DCBA, HGFE -> ABEF, CDGH
   tmpa = _mm_load_si128((__m128i*) &digest[0]);
   statea1 = _mm_load_si128((__m128i*) &digest[4]);

   tmpa  = _mm_shuffle_epi32(tmpa, 0xB1);       // CDAB
   statea1 = _mm_shuffle_epi32(statea1, 0x1B);    // EFGH
   statea0 = _mm_alignr_epi8(tmpa, statea1, 8);    // ABEF
   statea1 = _mm_blend_epi16(statea1, tmpa, 0xF0); // CDGH

      // Save hash values for addition after rounds
      abef_savea = statea0;
      cdgh_savea = statea1;

      // Rounds 0-3
      msga     = _mm_loadu_si128((__m128i*) input);
      msgtmpa0 = _mm_shuffle_epi8(msga, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0xE9B5DBA5B5C0FBCFull, 0x71374491428A2F98ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);

      // Rounds 4-7
      msgtmpa1 = _mm_loadu_si128((__m128i*) (input+16));
      msgtmpa1 = _mm_shuffle_epi8(msgtmpa1, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0xAB1C5ED5923F82A4ull, 0x59F111F13956C25Bull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);

      // Rounds 8-11
      msgtmpa2 = _mm_loadu_si128((__m128i*) (input+32));
      msgtmpa2 = _mm_shuffle_epi8(msgtmpa2, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0x550C7DC3243185BEull, 0x12835B01D807AA98ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);

      // Rounds 12-15
      msgtmpa3 = _mm_loadu_si128((__m128i*) (input+48));
      msgtmpa3 = _mm_shuffle_epi8(msgtmpa3, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0xC19BF1749BDC06A7ull, 0x80DEB1FE72BE5D74ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);

      // Rounds 16-19
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x240CA1CC0FC19DC6ull, 0xEFBE4786E49B69C1ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);

      // Rounds 20-23
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x76F988DA5CB0A9DCull, 0x4A7484AA2DE92C6Full));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);

      // Rounds 24-27
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0xBF597FC7B00327C8ull, 0xA831C66D983E5152ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);

      // Rounds 28-31
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0x1429296706CA6351ull, 0xD5A79147C6E00BF3ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);

      // Rounds 32-35
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x53380D134D2C6DFCull, 0x2E1B213827B70A85ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);

      // Rounds 36-39
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x92722C8581C2C92Eull, 0x766A0ABB650A7354ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);

      // Rounds 40-43
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0xC76C51A3C24B8B70ull, 0xA81A664BA2BFE8A1ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);

      // Rounds 44-47
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0x106AA070F40E3585ull, 0xD6990624D192E819ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);

      // Rounds 48-51
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x34B0BCB52748774Cull, 0x1E376C0819A4C116ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);

      // Rounds 52-55
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x682E6FF35B9CCA4Full, 0x4ED8AA4A391C0CB3ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);

      // Rounds 56-59
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0x8CC7020884C87814ull, 0x78A5636F748F82EEull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);

      // Rounds 60-63
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0xC67178F2BEF9A3F7ull, 0xA4506CEB90BEFFFAull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);

      // Add current hash values with previously saved
      statea0 = _mm_add_epi32(statea0, abef_savea);
      statea1 = _mm_add_epi32(statea1, cdgh_savea);

    // Round with padding
      // Save hash values for addition after rounds
      abef_savea = statea0;
      cdgh_savea = statea1;

      // Rounds 0-3
      msga     = _mm_loadu_si128((__m128i*) padding0);
      msgtmpa0 = _mm_shuffle_epi8(msga, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0xE9B5DBA5B5C0FBCFull, 0x71374491428A2F98ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);

      // Rounds 4-7
      msgtmpa1 = _mm_loadu_si128((__m128i*) padding1);
      msgtmpa1 = _mm_shuffle_epi8(msgtmpa1, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0xAB1C5ED5923F82A4ull, 0x59F111F13956C25Bull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);

      // Rounds 8-11
      msgtmpa2 = _mm_loadu_si128((__m128i*) padding1);
      msgtmpa2 = _mm_shuffle_epi8(msgtmpa2, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0x550C7DC3243185BEull, 0x12835B01D807AA98ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);

      // Rounds 12-15
      msgtmpa3 = _mm_loadu_si128((__m128i*) padding2);
      msgtmpa3 = _mm_shuffle_epi8(msgtmpa3, shuf_mask);
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0xC19BF1749BDC06A7ull, 0x80DEB1FE72BE5D74ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);

      // Rounds 16-19
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x240CA1CC0FC19DC6ull, 0xEFBE4786E49B69C1ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);

      // Rounds 20-23
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x76F988DA5CB0A9DCull, 0x4A7484AA2DE92C6Full));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);

      // Rounds 24-27
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0xBF597FC7B00327C8ull, 0xA831C66D983E5152ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);

      // Rounds 28-31
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0x1429296706CA6351ull, 0xD5A79147C6E00BF3ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);

      // Rounds 32-35
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x53380D134D2C6DFCull, 0x2E1B213827B70A85ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);

      // Rounds 36-39
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x92722C8581C2C92Eull, 0x766A0ABB650A7354ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa0 = _mm_sha256msg1_epu32(msgtmpa0, msgtmpa1);

      // Rounds 40-43
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0xC76C51A3C24B8B70ull, 0xA81A664BA2BFE8A1ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa1 = _mm_sha256msg1_epu32(msgtmpa1, msgtmpa2);

      // Rounds 44-47
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0x106AA070F40E3585ull, 0xD6990624D192E819ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa3, msgtmpa2, 4);
      msgtmpa0 = _mm_add_epi32(msgtmpa0, tmpa);
      msgtmpa0 = _mm_sha256msg2_epu32(msgtmpa0, msgtmpa3);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa2 = _mm_sha256msg1_epu32(msgtmpa2, msgtmpa3);

      // Rounds 48-51
         msga    = _mm_add_epi32(msgtmpa0, 
                  _mm_set_epi64x(0x34B0BCB52748774Cull, 0x1E376C0819A4C116ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa0, msgtmpa3, 4);
      msgtmpa1 = _mm_add_epi32(msgtmpa1, tmpa);
      msgtmpa1 = _mm_sha256msg2_epu32(msgtmpa1, msgtmpa0);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);
      msgtmpa3 = _mm_sha256msg1_epu32(msgtmpa3, msgtmpa0);

      // Rounds 52-55
         msga    = _mm_add_epi32(msgtmpa1, 
                  _mm_set_epi64x(0x682E6FF35B9CCA4Full, 0x4ED8AA4A391C0CB3ull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa1, msgtmpa0, 4);
      msgtmpa2 = _mm_add_epi32(msgtmpa2, tmpa);
      msgtmpa2 = _mm_sha256msg2_epu32(msgtmpa2, msgtmpa1);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);

      // Rounds 56-59
         msga    = _mm_add_epi32(msgtmpa2, 
                  _mm_set_epi64x(0x8CC7020884C87814ull, 0x78A5636F748F82EEull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
      tmpa     = _mm_alignr_epi8(msgtmpa2, msgtmpa1, 4);
      msgtmpa3 = _mm_add_epi32(msgtmpa3, tmpa);
      msgtmpa3 = _mm_sha256msg2_epu32(msgtmpa3, msgtmpa2);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);

      // Rounds 60-63
         msga    = _mm_add_epi32(msgtmpa3, 
                  _mm_set_epi64x(0xC67178F2BEF9A3F7ull, 0xA4506CEB90BEFFFAull));
         statea1 = _mm_sha256rnds2_epu32(statea1, statea0, msga);
         msga    = _mm_shuffle_epi32(msga, 0x0E);
         statea0 = _mm_sha256rnds2_epu32(statea0, statea1, msga);

      // Add current hash values with previously saved
      statea0 = _mm_add_epi32(statea0, abef_savea);
      statea1 = _mm_add_epi32(statea1, cdgh_savea);

       // Write hash values back in the correct order
       tmpa    = _mm_shuffle_epi32(statea0, 0x1B);    // FEBA
       statea1 = _mm_shuffle_epi32(statea1, 0xB1);    // DCHG
       statea0 = _mm_blend_epi16(tmpa, statea1, 0xF0); // DCBA
       statea1 = _mm_alignr_epi8(statea1, tmpa, 8);    // ABEF

       _mm_storeu_si128((__m128i*) output, statea0);
       _mm_storeu_si128((__m128i*) (output+16), statea1);

       for (int i = 0; i < 32; i += 4) {
           std::reverse(output + i, output + i +4);
       }

      output += 32;
      input += 64;
      blocks--;
}
}
