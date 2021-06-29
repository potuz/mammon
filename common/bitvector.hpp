/*  bitvector.hpp 
 * 
 *  This file is part of sszpp. 
 *  sszpp is C++ implementation of the "Simple Serialize" specification. 
 *  https://github.com/ethereum/eth2.0-specs/tree/dev/ssz 
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
#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include "common/bytes.hpp"
#include "ssz/ssz_container.hpp"
#include "yaml-cpp/yaml.h"

namespace eth
{
    template<unsigned N> 
    class Bitvector : public ssz::Container
    {
        private:

            std::array<bool, N>     m_arr;

        public:

            static constexpr std::size_t ssz_size = (N+7)/8;
            std::size_t get_ssz_size() const { return ssz_size; } 
            
            Bitvector (std::array<bool, N> vec) : m_arr {vec} {};
            template<typename ...T>
            Bitvector (T&&...l) : m_arr {{std::forward<T>(l)...}} {};

            void from_hexstring(std::string str)
            {
               if (!str.starts_with("0x"))
                   throw std::invalid_argument("string not prepended with 0x");
               str.erase(0,2);

               if (4*str.length() > N+4)
                   throw std::invalid_argument("hexstring larger than bitvector length");

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

               m_arr.fill(0);
               for (int i = 0; i < hex.size(); ++i)
                   for (int j = 0; j < 8 && 8*i+j < N; ++j)
                        m_arr[8*i+j] = ( (hex[i]  >> j) & 1 );
            }

            std::string to_string() const
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


            friend std::ostream& operator<< (std::ostream& os, const Bitvector<N>& m_bits)
            {
                for (auto const &b : m_bits.m_arr)
                    os << b;
                return os;
            };

            std::vector<std::byte> serialize() const
            {
                Bytes<(N+7)/8> ret {}; 
                for (int i=0; i < N; ++i)
                    ret[i/8] |= (std::byte(m_arr[i] << (i%8)));
                return ret;
            }

            YAML::Node encode() const
            {
                auto str = this->to_string();
                return YAML::convert<std::string>::encode(str);
            }

            bool decode(const YAML::Node& node)
            { 
                std::string str;
                if (!YAML::convert<std::string>::decode(node, str))
                    return false;
                this->from_hexstring(str);
                return true;
            }

    };
}

