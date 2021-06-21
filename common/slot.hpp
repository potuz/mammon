/*  slot.hpp 
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

#include "bytes.hpp"
#include "ssz/ssz_container.hpp"
#include <cstdint>
#include "yaml-cpp/yaml.h"

namespace eth
{
    class Slot : public ssz::Container
    {
        private:
            
            std::uint64_t value_;

        public:

            Slot(std::uint64_t s=0) : ssz::Container(8), value_ {s} {};
            operator std::uint64_t() const {return value_;};
            operator std::uint64_t&() {return value_; }
            operator Bytes8() const {return value_; }
            virtual std::vector<std::byte> serialize() const {return Bytes8(value_).serialize();}
    };
}

namespace YAML
{
    template<>
    struct convert<eth::Slot>
    {
        static Node encode(const eth::Slot& s)
        {
            return Node(std::uint64_t(s));
        }

        static bool decode(const Node& node, eth::Slot& s)
        {
            s = node.as<std::uint64_t>();
            return true;
        }
    };
}
