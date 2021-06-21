/*  main.cpp 
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

#include "common/types.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include "config.hpp"
#include "snappy.h"
#include <filesystem>
#include <fstream>
#include "yaml-cpp/yaml.h"

int main(int argc, char *argv[])
{
    std::uint64_t hola = 0x10;
    std::cout << hola << std::endl;
    std::cout << constants::DEPOSIT_CONTRACT_ADDRESS.to_string() << std::endl;


    std::cout << "Bitvector test\n";

    eth::Bitvector<4> vec = {true, true, false, true};
    std::cout << "Bitvector :" << vec << std::endl;


    std::cout << "\nBitlist test\n";

    eth::Bitlist myl = {false,true,false,false,false,false,true};
    std::cout <<"Expect: 010 0001\nObtained: " << myl << std::endl;
    auto serial = myl.serialize();
    std::cout << "Serialized exppect : 0xc2\nObtained : ";
    for (auto const &b : myl.serialize())
        std::cout << std::to_integer<int>(b);
    std::cout << std::endl;
    std::cout << "Bitlist from hexstring 0xc2 :";
    eth::Bitlist anotherbitlist;
    anotherbitlist.from_hexstring("0xc2");
    std::cout << anotherbitlist << "\n";
    std::cout << "Serialization : "<< anotherbitlist.to_string() << std::endl; 
    


    std::filesystem::path path = "/home/heluani/Documents/code/eth2.0-spec-tests/tests/minimal/phase0/ssz_static/Checkpoint/ssz_random/case_0/serialized.ssz_snappy";
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (not file.is_open())
        return {};

    const std::size_t& size = std::filesystem::file_size(path);
    std::vector<std::byte> content(size);
    file.read(reinterpret_cast<char*>(content.data()), size);
    file.close();
    std::cout << "Read " << size << " bytes into string\n";
    
    std::vector<std::byte> output(40);

    if (!snappy::RawUncompress(reinterpret_cast<char*>(content.data()), size, reinterpret_cast<char*>(output.data())))
        std::cout << "Error! \n";

    std::cout << "Uncompressed " << output.size() << " bytes\n";
    std::cout << "0x";
     
    for(int i = 0; i < output.size(); ++i)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << std::to_integer<int>(output[i]);

    return 0;
}
