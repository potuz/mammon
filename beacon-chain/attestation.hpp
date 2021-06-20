/*  attestation.hpp 
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
    struct AttestationData
    {
        Slot slot;
        CommitteeIndex index;

        Root beacon_block_root;
        Checkpoint source, target;

        Bytes<128> serialize() const
        {
            return Bytes<8>(slot) + Bytes<8>(index) + beacon_block_root + source.serialize() + target.serialize();
        }

    };

    struct IndexedAttestation
    {
        std::vector<ValidatorIndex> attesting_indices;
        AttestationData data;
        BLSSignature signature;

        std::vector<std::byte> serialize() const;
    };

    struct PendingAttestation
    {
        eth::Bitlist aggregation_bits;
        AttestationData data;
        Slot inclusion_delay;
        ValidatorIndex proposer_index;

        std::vector<std::byte> serialize() const;
    };

    struct Attestation
    {
        eth::Bitlist aggregation_bits;
        AttestationData data;
        BLSSignature signature;

        std::vector<std::byte> serialize() const;
    };
            
};

namespace YAML
{
    template<>
    struct convert<eth::AttestationData>
    {
        static Node encode(const eth::AttestationData& attdata)
        {
            Node node;
            node["slot"] = attdata.slot;
            node["index"] = attdata.index;
            node["beacon_block_root"] = attdata.beacon_block_root.to_string();
            node["source"] = attdata.source;
            node["target"] = attdata.target;
            return node;
        }

        static bool decode(const Node& node, eth::AttestationData& attdata)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 5)
                return false;

            attdata.slot = node["slot"].as<eth::Slot>();
            attdata.index = node["index"].as<eth::CommitteeIndex>();
            attdata.beacon_block_root = node["beacon_block_root"].as<std::string>();
            attdata.source = node["source"].as<eth::Checkpoint>();
            attdata.target = node["target"].as<eth::Checkpoint>();
            return true;
        }
    };
    template<>
    struct convert<eth::IndexedAttestation>
    {
        static Node encode(const eth::IndexedAttestation& indexed)
        {
            Node node;
            node["attesting_indices"] = indexed.attesting_indices;
            node["data"] = indexed.data;
            node["signature"] = indexed.signature.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::IndexedAttestation& indexed)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 3)
                return false;

            indexed.attesting_indices = node["attesting_indices"].as<std::vector<eth::ValidatorIndex>>();
            indexed.data = node["data"].as<eth::AttestationData>();
            indexed.signature = node["signature"].as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<eth::PendingAttestation>
    {
        static Node encode(const eth::PendingAttestation& pending)
        {
            Node node;
            node["aggregation_bits"] = pending.aggregation_bits.to_string();
            node["data"] = pending.data;
            node["inclusion_delay"] = pending.inclusion_delay;
            node["proposer_index"] = pending.proposer_index;
            return node;
        }

        static bool decode(const Node& node, eth::PendingAttestation& pending)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 4)
                return false;

            eth::Bitlist aggrbits;
            aggrbits.from_hexstring(node["aggregation_bits"].as<std::string>());
            pending.aggregation_bits = aggrbits;
            pending.data = node["data"].as<eth::AttestationData>();
            pending.inclusion_delay = node["inclusion_delay"].as<eth::Slot>();
            pending.proposer_index = node["proposer_index"].as<eth::ValidatorIndex>();
            return true;
        }
    };
    
    template<>
    struct convert<eth::Attestation>
    {
        static Node encode(const eth::Attestation& att)
        {
            Node node;
            node["aggregation_bits"] = att.aggregation_bits.to_string();
            node["data"] = att.data;
            node["signature"] = att.signature.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::Attestation& att)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 3)
                return false;

            eth::Bitlist aggrbits;
            aggrbits.from_hexstring(node["aggregation_bits"].as<std::string>());
            att.aggregation_bits = aggrbits;
            att.data = node["data"].as<eth::AttestationData>();
            att.signature = node["signature"].as<std::string>();
            return true;
        }
    };
}
 
