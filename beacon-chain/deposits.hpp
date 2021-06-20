/*  deposits.hpp 
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
#include <numeric>
#include "config.hpp"
#include "common/types.hpp"
#include "yaml-cpp/yaml.h"

namespace eth
{
    struct DepositMessage
    {
        BLSPubkey pubkey;
        Bytes32 withdrawal_credentials;
        Gwei amount;

        eth::Bytes<88> serialize() const
        {
            return pubkey + withdrawal_credentials + Bytes8(amount);
        }
    };

    struct DepositData
    {
        BLSPubkey pubkey;
        Bytes32 withdrawal_credentials;
        Gwei amount;
        BLSSignature signature;

        eth::Bytes<184> serialize() const
        {
            return pubkey + withdrawal_credentials + Bytes8(amount) + signature;
        }
    };

    struct Deposit
    {
        std::array<Bytes32, constants::DEPOSIT_CONTRACT_TREE_DEPTH + 1> proof;
        eth::DepositData data;

        eth::Bytes<32*constants::DEPOSIT_CONTRACT_TREE_DEPTH + 216> serialize() const
        {
            Bytes<32*(constants::DEPOSIT_CONTRACT_TREE_DEPTH + 1)> ret(0);
            for (int i = 0; i < proof.size(); ++i)
                std::copy(proof[i].cbegin(), proof[i].cend(), ret.begin()+32*i);
            return ret + data.serialize();
        }
    };
}

namespace YAML
{
    template<>
    struct convert<eth::DepositMessage>
    {
        static Node encode(const eth::DepositMessage& dep)
        {
            Node node;
            node["pubkey"] = dep.pubkey.to_string();
            node["withdrawal_credentials"] = dep.withdrawal_credentials.to_string();
            node["amount"] = dep.amount; 
            return node;
        }

        static bool decode(const Node& node, eth::DepositMessage& dep)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 3)
                return false;

            dep.pubkey = node["pubkey"].as<std::string>();
            dep.withdrawal_credentials = node["withdrawal_credentials"].as<std::string>();
            dep.amount = node["amount"].as<eth::Gwei>();
            return true;
        }
    };
    
    template<>
    struct convert<eth::DepositData>
    {
        static Node encode(const eth::DepositData& dep)
        {
            Node node;
            node["pubkey"] = dep.pubkey.to_string();
            node["withdrawal_credentials"] = dep.withdrawal_credentials.to_string();
            node["amount"] = dep.amount; 
            node["signature"] = dep.signature.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::DepositData& dep)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 4)
                return false;

            dep.pubkey = node["pubkey"].as<std::string>();
            dep.withdrawal_credentials = node["withdrawal_credentials"].as<std::string>();
            dep.amount = node["amount"].as<eth::Gwei>();
            dep.signature = node["signature"].as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<eth::Deposit>
    {
        static Node encode(const eth::Deposit& dep)
        {
            Node node;
            std::vector<std::string> proof;
            for (const auto &root : dep.proof)
                proof.push_back(root.to_string());
            node["proof"] = proof;
            node["data"] = dep.data;
            return node;
        }

        static bool decode(const Node& node, eth::Deposit& dep)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            auto proof = node["proof"].as<std::array<std::string, constants::DEPOSIT_CONTRACT_TREE_DEPTH +1>>();
            for (int i = 0; i < constants::DEPOSIT_CONTRACT_TREE_DEPTH + 1; ++i)
                dep.proof[i] = proof[i];
            dep.data = node["data"].as<eth::DepositData>();
            return true;
        }
    };

}
 
