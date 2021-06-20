/*  bitlist.cpp 
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

#include "bitlist.hpp"
#include <cstddef>
#include <bit>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace eth
{
    std::vector<std::byte> Bitlist::serialize() const
    {
        std::vector<std::byte> ret ( m_arr.size()/8 + 1, std::byte(0));
        for (int i = 0; i < m_arr.size(); ++i)
            ret[i/8] |= std::byte(static_cast<unsigned char>(m_arr[i] << (i%8)));
        ret.back() |= std::byte(1 << (m_arr.size() % 8));
        return ret;
    }

    //Does not check for errors, assumes strings is 0x valid hex bytes! In particular even # of chars
    void Bitlist::from_hexstring(std::string str)
    {
       if (!str.starts_with("0x"))
           throw std::invalid_argument("string not prepended with 0x");
       str.erase(0,2);

       //The most significant bit is the length of the bitlist and it's not to be counted
       std::stringstream ss;
       unsigned int buffer;
       std::vector<unsigned char> hex;
       for (int offset = 0; offset < str.length(); offset += 2)
       {
           ss.clear();
           ss << std::hex << str.substr(offset,2);
           ss >> buffer;
           hex.push_back(static_cast<unsigned char>(buffer));
       }

       int msb = std::countl_zero(hex.back());

       std::vector<bool> ret(hex.size()*8 - msb -1);
       
       m_arr.clear();

       for (int i = 0; i < hex.size()-1; ++i)
           for (int j = 0; j < 8; ++j)
                m_arr.push_back ( (hex[i]  >> j) & 1 );

       for (int i = 0; i < 7-msb; ++i)
           m_arr.push_back( (hex.back() >> i) & 1 );
    }

    std::string Bitlist::to_string() const
    {
        std::stringstream os;
        std::ios_base::fmtflags save = std::cout.flags();
        auto serial = this->serialize(); 
        os << "0x";
        for (auto i = serial.cbegin(); i != serial.cend(); ++i)
            os << std::setfill('0') << std::setw(2) << std::hex << std::to_integer<int>(*i);
        std::cout.flags(save);
        return os.str();
    };

 

} 
