/*  beacon_block.hpp 
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

#include <utility>
#include "config.hpp"
#include "ssz/ssz.hpp"
#include "common/types.hpp"
#include "attestation.hpp"
#include "eth1data.hpp"
#include "deposits.hpp"
#include "yaml-cpp/yaml.h"

namespace eth
{
    struct BeaconBlockHeader
    {
        Slot slot;
        ValidatorIndex proposer_index;
        Root parent_root, state_root, body_root;

        eth::Bytes<112> serialize() const
        {
            return Bytes8(slot) + Bytes8(proposer_index) + parent_root + state_root + body_root;
        }
    };

    struct VoluntaryExit
    {
        Epoch epoch;
        ValidatorIndex validator_index;

        Bytes<16> serialize() const
        {
            return Bytes8(epoch) + Bytes8(validator_index);
        }
    };

    struct SignedVoluntaryExit
    {
        VoluntaryExit   message;
        BLSSignature    signature;

        Bytes<112> serialize() const
        {
            return message.serialize() + signature;
        }
    };

    struct ProposerSlashing;
    struct AttesterSlashing;

    class BeaconBlockBody
    {
        private:
            BLSSignature    randao_reveal_;     // 96 bytes
            Eth1Data        eth1_data_;         // 72 bytes
            Bytes32         graffiti_;

            std::vector<ProposerSlashing>   proposer_slashings_;      //224 bytes per slashing
            std::vector<AttesterSlashing>   attester_slashings_;
            std::vector<Attestation>        attestations_;
            std::vector<Deposit>            deposits_;
            std::vector<SignedVoluntaryExit> voluntary_exits_;

        public:
            std::vector<std::byte> serialize() const;
            constexpr BLSSignature const & randao_reveal() const 
            {
                return randao_reveal_;
            }
            constexpr Eth1Data const & eth1_data() const
            {
                return eth1_data_;
            }
            constexpr Bytes32 const & graffiti() const
            {
                return graffiti_;
            }
            constexpr std::vector<ProposerSlashing> const & proposer_slashings() const
            {
                return proposer_slashings_;
            }
            constexpr std::vector<AttesterSlashing> const & attester_slashings() const
            {
                return attester_slashings_;
            }
            constexpr std::vector<Attestation> const & attestations() const
            {
                return attestations_;
            }
            constexpr std::vector<Deposit> const & deposits() const
            {
                return deposits_;
            }
            constexpr std::vector<SignedVoluntaryExit> const & voluntary_exits() const
            {
                return voluntary_exits_;
            }

            void randao_reveal(BLSSignature s);
            void eth1_data(Eth1Data);
            void graffiti(Bytes32);
            void proposer_slashings(std::vector<ProposerSlashing>);
            void attester_slashings(std::vector<AttesterSlashing>);
            void attestations(std::vector<Attestation>);
            void deposits(std::vector<Deposit>);
            void voluntary_exits(std::vector<SignedVoluntaryExit>);
    };

    
    class BeaconBlock
    {
        Slot slot_;
        ValidatorIndex proposer_index_;
        Root    parent_root_, state_root_;
        BeaconBlockBody body_;

        public:

            std::vector<std::byte> serialize() const;
            constexpr Slot slot() const
            {
                return slot_;
            }
            constexpr ValidatorIndex proposer_index() const
            {
                return proposer_index_;
            }
            constexpr const Root&  parent_root() const
            {
                return parent_root_;
            }
            constexpr const Root&  state_root() const
            {
                return state_root_;
            }
            constexpr const BeaconBlockBody& body() const
            {
                return body_;
            }
            void slot(Slot);
            void proposer_index(ValidatorIndex);
            void parent_root(Root);
            void state_root(Root);
            void body(BeaconBlockBody);

    };

    struct SignedBeaconBlockHeader              //208 bytes ssz
    {
        BeaconBlockHeader   message;            // 112 bytes ssz
        BLSSignature        signature;

        auto serialize() const
        {
            return message.serialize() + signature;
        }
    };

    struct ProposerSlashing 
    {
        SignedBeaconBlockHeader signed_header_1, signed_header_2;
        Bytes<416> serialize() const
        {
            return signed_header_1.serialize() + signed_header_2.serialize();
        }
    };

    struct AttesterSlashing
    {
        IndexedAttestation attestation_1, attestation_2;

        auto serialize() const
        {
            auto ser1 = attestation_1.serialize();
            auto ser2 = attestation_2.serialize();
            Bytes8 offsets = Bytes4( std::uint32_t(2*constants::BYTES_PER_LENGTH_OFFSET)) +
                Bytes4(std::uint32_t(2*constants::BYTES_PER_LENGTH_OFFSET + ser1.size())); 
            ser1.insert(ser1.begin(), offsets.begin(), offsets.end());
            ser1.insert(ser1.end(), ser2.begin(), ser2.end());
            return ser1;
        }
    };

    struct SignedBeaconBlock
    {
        BeaconBlock     message;
        BLSSignature    signature;

        std::vector<std::byte> serialize() const;
    };

}

namespace YAML
{
    template<>
    struct convert<eth::BeaconBlockHeader>
    {
        static Node encode(const eth::BeaconBlockHeader& bb)
        {
            Node node;
            node["slot"] = bb.slot;
            node["proposer_index"] = bb.proposer_index;
            node["parent_root"] = bb.parent_root.to_string();
            node["state_root"] = bb.state_root.to_string();
            node["body_root"] = bb.body_root.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::BeaconBlockHeader& bb)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 5)
                return false;

            bb.slot = node["slot"].as<eth::Slot>();
            bb.proposer_index = node["proposer_index"].as<eth::ValidatorIndex>();
            bb.parent_root = node["parent_root"].as<std::string>();
            bb.state_root = node["state_root"].as<std::string>();
            bb.body_root = node["body_root"].as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<eth::SignedBeaconBlockHeader>
    {
        static Node encode(const eth::SignedBeaconBlockHeader& bb)
        {
            Node node;
            node["message"] = bb.message;
            node["signature"] = bb.signature.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::SignedBeaconBlockHeader& bb)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            bb.message = node["message"].as<eth::BeaconBlockHeader>();
            bb.signature = node["signature"].as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<eth::ProposerSlashing>
    {
        static Node encode(const eth::ProposerSlashing& bb)
        {
            Node node;
            node["signed_header_1"] = bb.signed_header_1;
            node["signed_header_2"] = bb.signed_header_2;
            return node;
        }

        static bool decode(const Node& node, eth::ProposerSlashing& bb)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            bb.signed_header_1 = node["signed_header_1"].as<eth::SignedBeaconBlockHeader>();
            bb.signed_header_2 = node["signed_header_2"].as<eth::SignedBeaconBlockHeader>();
            return true;
        }
    };

    template<>
    struct convert<eth::AttesterSlashing>
    {
        static Node encode(const eth::AttesterSlashing& bb)
        {
            Node node;
            node["attestation_1"] = bb.attestation_1;
            node["attestation_2"] = bb.attestation_2;
            return node;
        }

        static bool decode(const Node& node, eth::AttesterSlashing& bb)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            bb.attestation_1 = node["attestation_1"].as<eth::IndexedAttestation>();
            bb.attestation_2 = node["attestation_2"].as<eth::IndexedAttestation>();
            return true;
        }
    };

    template<>
    struct convert<eth::VoluntaryExit>
    {
        static Node encode(const eth::VoluntaryExit& bb)
        {
            Node node;
            node["epoch"] = bb.epoch;
            node["validator_index"] = bb.validator_index;
            return node;
        }

        static bool decode(const Node& node, eth::VoluntaryExit& bb)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            bb.epoch = node["epoch"].as<eth::Epoch>();
            bb.validator_index = node["validator_index"].as<eth::ValidatorIndex>();
            return true;
        }
    };

    template<>
    struct convert<eth::SignedVoluntaryExit>
    {
        static Node encode(const eth::SignedVoluntaryExit& bb)
        {
            Node node;
            node["message"] = bb.message;
            node["signature"] = bb.signature.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::SignedVoluntaryExit& bb)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            bb.message = node["message"].as<eth::VoluntaryExit>();
            bb.signature = node["signature"].as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<eth::BeaconBlockBody>
    {
        static Node encode(const eth::BeaconBlockBody& bb)
        {
            Node node;
            node["randao_reveal"] = bb.randao_reveal().to_string();
            node["eth1_data"] = bb.eth1_data();
            node["graffiti"] = bb.graffiti().to_string();
            node["proposer_slashings"] = bb.proposer_slashings(); 
            node["attester_slashings"] = bb.attester_slashings();
            node["attestations"] = bb.attestations();
            node["deposits"] = bb.deposits();
            node["voluntary_exits"] = bb.voluntary_exits();
            return node;
        }

        static bool decode(const Node& node, eth::BeaconBlockBody& bb)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 8)
                return false;

            bb.randao_reveal(node["randao_reveal"].as<std::string>());
            bb.eth1_data(node["eth1_data"].as<eth::Eth1Data>());
            bb.graffiti(node["graffiti"].as<std::string>());
            bb.proposer_slashings(node["proposer_slashings"].as<std::vector<eth::ProposerSlashing>>());
            bb.attester_slashings(node["attester_slashings"].as<std::vector<eth::AttesterSlashing>>());
            bb.attestations(node["attestations"].as<std::vector<eth::Attestation>>());
            bb.deposits(node["deposits"].as<std::vector<eth::Deposit>>());
            bb.voluntary_exits(node["voluntary_exits"].as<std::vector<eth::SignedVoluntaryExit>>());
            return true;
        }
    };

    template<>
    struct convert<eth::BeaconBlock>
    {
        static Node encode(const eth::BeaconBlock& bb)
        {
            Node node;
            node["slot"] = bb.slot();
            node["proposer_index"] = bb.proposer_index();
            node["parent_root"] = bb.parent_root().to_string();
            node["state_root"] = bb.state_root().to_string();
            node["body"] = bb.body();
            return node;
        }

        static bool decode(const Node& node, eth::BeaconBlock& bb)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 5)
                return false;

            bb.slot(node["slot"].as<eth::Slot>());
            bb.proposer_index(node["proposer_index"].as<eth::ValidatorIndex>());
            bb.parent_root(node["parent_root"].as<std::string>());
            bb.state_root(node["state_root"].as<std::string>());
            bb.body(node["body"].as<eth::BeaconBlockBody>());
            return true;
        }
    };

    template<>
    struct convert<eth::SignedBeaconBlock>
    {
        static Node encode(const eth::SignedBeaconBlock& bb)
        {
            Node node;
            node["message"] = bb.message;
            node["signature"] = bb.signature.to_string();
            return node;
        }

        static bool decode(const Node& node, eth::SignedBeaconBlock& bb)
        {
            if ((node.Type() != NodeType::Map) || node.size() != 2)
                return false;

            bb.message = node["message"].as<eth::BeaconBlock>();
            bb.signature = node["signature"].as<std::string>();
            return true;
        }
    };

}
