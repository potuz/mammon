/*  bytes_test.cpp 
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

#include "acutest.h"
#include "bytes.hpp"
#include <iostream>

void test_bytes_from_int()
{
    eth::Bytes4 got (std::uint32_t(0x10203040));
    eth::Bytes4 expected { 0x40, 0x30, 0x20, 0x10 };
    TEST_CHECK(expected == got);

    eth::Bytes8 got2 (std::uint32_t(0x10203040));
    eth::Bytes8 expected2 { 0x40, 0x30, 0x20, 0x10, 0x0, 0x0, 0x0, 0x0 };
    TEST_CHECK(expected2 == got2);

}

void test_bytes_to_integer_little_endian()
{
    eth::Bytes1 got { 0x0a };
    std::uint8_t expected = 0x0a;
    TEST_CHECK ( expected == got.to_integer_little_endian<std::uint8_t>() );

    eth::Bytes4 got2 { 0x40, 0x30, 0x20, 0x10 };
    std::uint32_t expected2 = 0x10203040;
    TEST_CHECK ( expected2 == got2.to_integer_little_endian<std::uint32_t>() );
    
    eth::Bytes8 got3 { 0x40, 0x30, 0x20, 0x10, 0x0, 0x0, 0x0, 0x0 };
    std::uint64_t expected3 = expected2;
    TEST_CHECK ( expected3 == got3.to_integer_little_endian<std::uint64_t>() );
}

TEST_LIST = {
    { "bytes_from_int", test_bytes_from_int },
    { "bytes_to_integer_little_endian", test_bytes_to_integer_little_endian },
    { NULL, NULL }
};