/*  boolean.hpp 
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

#pragma once

#include "ssz/ssz_container.hpp"
#include "common/basic_types.hpp"
#include "yaml-cpp/yaml.h"

namespace eth
{
    class Boolean : public ssz::Container
    {
        private:
            
            bool value_;

        public:

            Boolean(bool s=false) : value_ {s} {};
            operator bool() const {return value_;};
            operator bool&() {return value_; }
            operator Bytes<1>() const {return char(value_); }
            virtual std::vector<std::byte> serialize() const {return Bytes<1>(value_).serialize();}

            static constexpr std::size_t ssz_size = 1;
            std::size_t get_ssz_size() const { return ssz_size; }

            YAML::Node encode() const { return YAML::convert<bool>::encode(value_); } 
            bool decode(const YAML::Node& node) { return YAML::convert<bool>::decode(node, value_); }
    };
}

