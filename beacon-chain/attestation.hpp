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

#include "common/bitlist.hpp"
#include "common/containers.hpp"
#include "yaml-cpp/yaml.h"

namespace eth {
struct AttestationData : public ssz::Container {
    Slot slot;
    CommitteeIndex index;

    Root beacon_block_root;
    Checkpoint source, target;

    static constexpr std::size_t ssz_size = 128;
    std::size_t get_ssz_size() const override { return ssz_size; }
    BytesVector serialize() const override { return serialize_({&slot, &index, &beacon_block_root, &source, &target}); }
    bool deserialize(ssz::SSZIterator it, ssz::SSZIterator end) override {
        return deserialize_(it, end, {&slot, &index, &beacon_block_root, &source, &target});
    }

    YAML::Node encode() const override {
        return encode_({{"slot", &slot},
                        {"index", &index},
                        {"beacon_block_root", &beacon_block_root},
                        {"source", &source},
                        {"target", &target}});
    }

    bool decode(const YAML::Node &node) override {
        return decode_(node, {{"slot", &slot},
                              {"index", &index},
                              {"beacon_block_root", &beacon_block_root},
                              {"source", &source},
                              {"target", &target}});
    }
};

struct IndexedAttestation : public ssz::Container {
    ListFixedSizedParts<ValidatorIndex> attesting_indices;
    AttestationData data;
    BLSSignature signature;

    BytesVector serialize() const override { return serialize_({&attesting_indices, &data, &signature}); }
    bool deserialize(ssz::SSZIterator it, ssz::SSZIterator end) override {
        return deserialize_(it, end, {&attesting_indices, &data, &signature});
    }

    YAML::Node encode() const override {
        return encode_({{"attesting_indices", &attesting_indices}, {"data", &data}, {"signature", &signature}});
    }

    bool decode(const YAML::Node &node) override {
        return decode_(node, {{"attesting_indices", &attesting_indices}, {"data", &data}, {"signature", &signature}});
    }
};

struct PendingAttestation : public ssz::Container {
    eth::Bitlist aggregation_bits;
    AttestationData data;
    Slot inclusion_delay;
    ValidatorIndex proposer_index;

    BytesVector serialize() const override {
        return serialize_({&aggregation_bits, &data, &inclusion_delay, &proposer_index});
    }
    bool deserialize(ssz::SSZIterator it, ssz::SSZIterator end) override {
        return deserialize_(it, end, {&aggregation_bits, &data, &inclusion_delay, &proposer_index});
    }

    YAML::Node encode() const override {
        return encode_({{"aggregation_bits", &aggregation_bits},
                        {"data", &data},
                        {"inclusion_delay", &inclusion_delay},
                        {"proposer_index", &proposer_index}});
    }

    bool decode(const YAML::Node &node) override {
        return decode_(node, {{"aggregation_bits", &aggregation_bits},
                              {"data", &data},
                              {"inclusion_delay", &inclusion_delay},
                              {"proposer_index", &proposer_index}});
    }
};

struct Attestation : public ssz::Container {
    eth::Bitlist aggregation_bits;
    AttestationData data;
    BLSSignature signature;

    BytesVector serialize() const override { return serialize_({&aggregation_bits, &data, &signature}); }
    bool deserialize(ssz::SSZIterator it, ssz::SSZIterator end) override {
        return deserialize_(it, end, {&aggregation_bits, &data, &signature});
    }
    YAML::Node encode() const override {
        return encode_({{"aggregation_bits", &aggregation_bits}, {"data", &data}, {"signature", &signature}});
    }

    bool decode(const YAML::Node &node) override {
        return decode_(node, {{"aggregation_bits", &aggregation_bits}, {"data", &data}, {"signature", &signature}});
    }
};

};  // namespace eth
