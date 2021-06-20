/*  containers.hpp 
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

    struct Fork
    {
        Version previous_version, current_version;
        Epoch epoch;

        Bytes<16> serialize() const
        {
            return previous_version + current_version + Bytes<8>(epoch); 
        }

    };

    struct ForkData
    {
        Version current_version;
        Root genesis_validators_root;
        Bytes<36> serialize() const
        {
            return current_version + genesis_validators_root;
        }
    };

    struct Checkpoint
    {
        Epoch epoch;
        Root root;
        Bytes<40> serialize() const
        {
            return Bytes<8>(epoch) + root;
        }
    };

    struct SigningData
    {
        Root object_root;
        Domain domain;
        Bytes<64> serialize() const
        {
            return object_root + domain;
        }
    };

}

namespace YAML
{
    template<>
    struct convert<eth::Fork>
    {
        static Node encode(const eth::Fork& fork)
        {
            Node node;
            node["previous_version"] = fork.previous_version.to_string();
            node["current_version"] = fork.current_version.to_string();
            node["epoch"] = fork.epoch;
            return node;
        }

        static bool decode(const Node& node, eth::Fork& fork)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 3)
                return false;

            fork.previous_version  = node["previous_version"].as<std::string>();
            fork.current_version  = node["current_version"].as<std::string>();
            fork.epoch = node["epoch"].as<eth::Epoch>();
            return true;
        }
    };

    template<>
    struct convert<eth::ForkData>
    {
        static Node encode(const eth::ForkData& forkdata)
        {
            Node node;
            node["current_version"] = forkdata.current_version.to_string();
            node["genesis_validators_root"] = forkdata.genesis_validators_root.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::ForkData& forkdata)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            forkdata.current_version  = node["current_version"].as<std::string>();
            forkdata.genesis_validators_root = node["genesis_validators_root"].as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<eth::Checkpoint>
    {
        static Node encode(const eth::Checkpoint& chk)
        {
            Node node;
            node["epoch"] = chk.epoch;
            node["root"] = chk.root.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::Checkpoint& chk)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            chk.epoch = node["epoch"].as<eth::Epoch>();
            chk.root  = node["root"].as<std::string>();
            return true;
        }
    };
}

