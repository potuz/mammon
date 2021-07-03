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
#include "common/basic_types.hpp"
#include "ssz/ssz_container.hpp"
#include "yaml-cpp/yaml.h"

namespace eth
{
    struct DepositMessage : public ssz::Container
    {
        BLSPubkey pubkey;
        Bytes32 withdrawal_credentials;
        Gwei amount;

        static constexpr std::size_t ssz_size = 88;
        std::size_t get_ssz_size() const { return ssz_size; } 
        BytesVector serialize() const { return serialize_({&pubkey, &withdrawal_credentials, &amount}); }
        YAML::Node encode() const
        { 
            return encode_({
                    { "pubkey", &pubkey },
                    { "withdrawal_credentials", &withdrawal_credentials },
                    { "amount", &amount } });
        }

        bool decode(const YAML::Node& node) 
        { 
            return decode_(node, {
                    { "pubkey", &pubkey },
                    { "withdrawal_credentials", &withdrawal_credentials },
                    { "amount", &amount } });
        }
    };

    struct DepositData : public ssz::Container
    {
        BLSPubkey pubkey;
        Bytes32 withdrawal_credentials;
        Gwei amount;
        BLSSignature signature;

        static constexpr std::size_t ssz_size = 184;
        std::size_t get_ssz_size() const { return ssz_size; } 
        BytesVector serialize() const { return serialize_({&pubkey, &withdrawal_credentials, &amount, &signature}); }
        YAML::Node encode() const
        { 
            return encode_({
                    { "pubkey", &pubkey },
                    { "withdrawal_credentials", &withdrawal_credentials },
                    { "amount", &amount },
                    { "signature", &signature} });
        }

        bool decode(const YAML::Node& node) 
        { 
            return decode_(node, {
                    { "pubkey", &pubkey },
                    { "withdrawal_credentials", &withdrawal_credentials },
                    { "amount", &amount },
                    { "signature", &signature} });
        }
    };

    struct Deposit : public ssz::Container
    {
        VectorFixedSizedParts<Bytes32, constants::DEPOSIT_CONTRACT_TREE_DEPTH + 1> proof;
        eth::DepositData data;

        static constexpr std::size_t ssz_size = 32*constants::DEPOSIT_CONTRACT_TREE_DEPTH + 216;
        std::size_t get_ssz_size() const { return ssz_size; } 
        BytesVector serialize() const { return serialize_({&proof, &data}); }

        YAML::Node encode() const
        { 
            return encode_({
                    { "proof", &proof },
                    { "data", &data } });
        }

        bool decode(const YAML::Node& node) 
        { 
            return decode_(node, {
                    { "proof", &proof },
                    { "data", &data } });
        }

    };
}
