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
#include "common/basic_types.hpp"
#include "yaml-cpp/yaml.h"

namespace eth

{

    struct Fork : public ssz::Container
    {
        Version previous_version, current_version;
        Epoch epoch;

        Fork() : ssz::Container(16) {}
        BytesVector serialize() const
        {
            return serialize_({&previous_version, &current_version, &epoch});
        }
    };

    struct ForkData : public ssz::Container
    {
        Version current_version;
        Root    genesis_validators_root;

        ForkData() : ssz::Container(36) {}
        BytesVector serialize() const
        {
            return serialize_({&current_version, &genesis_validators_root});
        }

    };

    struct Checkpoint : public ssz::Container
    {
        Epoch epoch;
        Root  root;

        Checkpoint() : ssz::Container(40) {}
        BytesVector serialize() const { return serialize_({&epoch, &root}); }
    };

    struct SigningData : public ssz::Container
    {
        Root object_root;
        Domain domain;

        SigningData() : ssz::Container(64) {}
        BytesVector serialize() const { return serialize_({&object_root, &domain}); }
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
            node["previous_version"] = fork.previous_version;
            node["current_version"] = fork.current_version;
            node["epoch"] = fork.epoch;
            return node;
        }

        static bool decode(const Node& node, eth::Fork& fork)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 3)
                return false;

            fork.current_version = node["previous_version"].as<eth::Version>();
            fork.current_version = node["current_version"].as<eth::Version>();
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
            node["current_version"] = forkdata.current_version;
            node["genesis_validators_root"] = forkdata.genesis_validators_root;
            return node;
        }

        static bool decode(const Node& node, eth::ForkData& forkdata)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            forkdata.current_version  = node["current_version"].as<eth::Version>();
            forkdata.genesis_validators_root = node["genesis_validators_root"].as<eth::Root>();
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
            node["root"] = chk.root;
            return node;
        }

        static bool decode(const Node& node, eth::Checkpoint& chk)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            chk.epoch = node["epoch"].as<eth::Epoch>();
            chk.root  = node["root"].as<eth::Root>();
            return true;
        }
    };

    template<>
    struct convert<eth::SigningData>
    {
        static Node encode(const eth::SigningData& s)
        {
            Node node;
            node["object_root"] = s.object_root;
            node["domain"] = s.domain;
            return node;
        }

        static bool decode(const Node& node, eth::SigningData& s)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            s.object_root  = node["object_root"].as<eth::Root>();
            s.domain = node["domain"].as<eth::Domain>();
            return true;
        }
    };
}

