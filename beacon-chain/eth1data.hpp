/*  eth1data.hpp 
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
#include "common/types.hpp" 
#include "yaml-cpp/yaml.h"

namespace eth
{
    struct Eth1Data
    {
        Root deposit_root;
        std::uint64_t deposit_count;
        Hash32  block_hash;

        eth::Bytes<72> serialize() const
        {
            return deposit_root + Bytes8(deposit_count) + block_hash;
        }
    };

}

namespace YAML
{
    template<>
    struct convert<eth::Eth1Data>
    {
        static Node encode(const eth::Eth1Data& eth1data)
        {
            Node node;
            node["deposit_root"] = eth1data.deposit_root.to_string();
            node["deposit_count"] = eth1data.deposit_count;
            node["block_hash"] = eth1data.block_hash.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::Eth1Data& eth1data)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 3)
                return false;

            eth1data.deposit_root = node["deposit_root"].as<std::string>();
            eth1data.deposit_count = node["deposit_count"].as<std::uint64_t>();
            eth1data.block_hash = node["block_hash"].as<std::string>();
            return true;
        }
    };
} 
