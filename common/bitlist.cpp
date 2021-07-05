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

#include <bit>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "ssz/ssz.hpp"

namespace eth {
std::vector<std::byte> Bitlist::serialize() const {
    std::vector<std::byte> ret(m_arr.size() / constants::BITS_PER_BYTE + 1, std::byte(0));
    for (int i = 0; i < m_arr.size(); ++i)
        ret[i / constants::BITS_PER_BYTE] |=
            std::byte(static_cast<unsigned char>(m_arr[i] << (i % constants::BITS_PER_BYTE)));
    ret.back() |= std::byte(1 << (m_arr.size() % constants::BITS_PER_BYTE));
    return ret;
}
bool Bitlist::deserialize(ssz::SSZIterator it, ssz::SSZIterator end) {
    auto last = end;
    --last;

    int msb = std::countl_zero(static_cast<unsigned char>(*last));
    m_arr.clear();

    for (auto i = it; i != last; ++i)
        for (int j = 0; j < constants::BITS_PER_BYTE; ++j)
            m_arr.push_back(std::to_integer<unsigned char>((*i >> j) & std::byte(1)));

    for (int i = 0; i < constants::BITS_PER_BYTE - 1 - msb; ++i)
        m_arr.push_back(std::to_integer<unsigned char>((*last >> i) & std::byte(1)));
    return true;
}

// Does not check for errors, assumes strings is 0x valid hex bytes! In
// particular even # of chars
void Bitlist::from_hexstring(std::string str) {
    if (!str.starts_with("0x")) throw std::invalid_argument("string not prepended with 0x");
    str.erase(0, 2);

    // The most significant bit is the length of the bitlist and it's not to be
    // counted
    std::stringstream ss;
    unsigned int buffer = 0;
    std::vector<std::byte> hex;
    for (int offset = 0; offset < str.length(); offset += 2) {
        ss.clear();
        ss << std::hex << str.substr(offset, 2);
        ss >> buffer;
        hex.push_back(static_cast<std::byte>(buffer));
    }
    deserialize(hex.begin(), hex.end());
}

std::string Bitlist::to_string() const {
    std::stringstream os;
    std::ios_base::fmtflags save = std::cout.flags();
    auto serial = this->serialize();
    os << "0x";
    for (auto i = serial.cbegin(); i != serial.cend(); ++i)
        os << std::setfill('0') << std::setw(2) << std::hex << std::to_integer<int>(*i);
    std::cout.flags(save);
    return os.str();
};

YAML::Node Bitlist::encode() const {
    auto str = this->to_string();
    return YAML::convert<std::string>::encode(str);
}
bool Bitlist::decode(const YAML::Node &node) {
    std::string str;
    if (!YAML::convert<std::string>::decode(node, str)) return false;
    this->from_hexstring(str);
    return true;
}

}  // namespace eth
