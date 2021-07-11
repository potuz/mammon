/*  test_ssz.cpp
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

#include <cstdint>
#include <filesystem>
#include <fstream>

#include "beacon-chain/attestation.hpp"
#include "beacon-chain/beacon_block.hpp"
#include "beacon-chain/beacon_state.hpp"
#include "beacon-chain/deposits.hpp"
#include "beacon-chain/eth1data.hpp"
#include "beacon-chain/validator.hpp"
#include "common/bitlist.hpp"
#include "common/bitvector.hpp"
#include "include/acutest.h"
#include "include/config.hpp"
#include "snappy.h"
#include "yaml-cpp/yaml.h"

namespace fs = std::filesystem;

template <typename T>
void test_ssz(const std::string &&path) {
    auto base_path = constants::TEST_VECTORS_PATH + path;
    for (auto &p1 : fs::directory_iterator(base_path))
        for (auto &p2 : fs::directory_iterator(p1))
            for (auto &p3 : fs::directory_iterator(p2)) {
                auto node = YAML::LoadFile(p2.path().string() + "/value.yaml");
                T ssz_type = node.as<T>();
                auto computed_root = ssz_type.hash_tree_root();
                auto node_root = YAML::LoadFile(p2.path().string() + "/roots.yaml");
                auto root = node_root["root"].as<eth::Bytes32>();

                TEST_CHECK(computed_root == root.to_array());  // NOLINT

                auto serialized = ssz_type.serialize();
                auto ssz_snappy_path = p2.path().string() + "/serialized.ssz_snappy";
                std::ifstream ssz_snappy(ssz_snappy_path, std::ios::in | std::ios::binary);
                if (not ssz_snappy.is_open())
                    throw std::filesystem::filesystem_error("could not open file", p2.path(), std::error_code());

                const std::size_t size = std::filesystem::file_size(ssz_snappy_path);
                std::vector<std::byte> content(size);
                // NOLINTNEXTLINE
                ssz_snappy.read(reinterpret_cast<char *>(content.data()), size);
                ssz_snappy.close();

                std::vector<std::byte> output(serialized.size());
                if (!snappy::RawUncompress(reinterpret_cast<char *>(content.data()),  // NOLINT
                                           size,
                                           reinterpret_cast<char *>(output.data())))  // NOLINT
                    throw std::filesystem::filesystem_error("could not uncompress file", p2.path(), std::error_code());

                T obj;

                obj.deserialize(output.begin(), output.end());

                TEST_CHECK(obj == ssz_type);                               // NOLINT
                TEST_MSG("Processing file: %s", ssz_snappy_path.c_str());  // NOLINT
                TEST_DUMP("Expected:", output.data(), serialized.size());
                TEST_DUMP("Produced:", serialized.data(), serialized.size());

                TEST_CHECK(serialized == output);                          // NOLINT
                TEST_MSG("Processing file: %s", ssz_snappy_path.c_str());  // NOLINT
                TEST_DUMP("Expected:", output.data(), serialized.size());
                TEST_DUMP("Produced:", serialized.data(), serialized.size());
            }
}

template <typename T>
void test_ssz_generic(const std::string &&path) {
    auto base_path = constants::TEST_VECTORS_GENERAL_PATH + path + "/valid/";
    for (auto &p1 : fs::directory_iterator(base_path)) {
        auto node_root = YAML::LoadFile(p1.path().string() + "/meta.yaml");
        auto root = node_root["root"].as<eth::Bytes32>();

        auto ssz_snappy_path = p1.path().string() + "/serialized.ssz_snappy";
        std::ifstream ssz_snappy(ssz_snappy_path, std::ios::in | std::ios::binary);
        if (not ssz_snappy.is_open())
            throw std::filesystem::filesystem_error("could not open file", p1.path(), std::error_code());

        const std::size_t size = std::filesystem::file_size(ssz_snappy_path);
        std::vector<std::byte> content(size);
        // NOLINTNEXTLINE
        ssz_snappy.read(reinterpret_cast<char *>(content.data()), size);
        ssz_snappy.close();

        std::size_t ssz_size{};
        if (!snappy::GetUncompressedLength(reinterpret_cast<char *>(content.data()), size, &ssz_size))
            throw std::filesystem::filesystem_error("could not uncompress file", p1.path(), std::error_code());

        std::vector<std::byte> output(ssz_size);
        if (!snappy::RawUncompress(reinterpret_cast<char *>(content.data()),  // NOLINT
                                   size,
                                   reinterpret_cast<char *>(output.data())))  // NOLINT
            throw std::filesystem::filesystem_error("could not uncompress file", p1.path(), std::error_code());

        T ssz_type;
        ssz_type.deserialize(output.begin(), output.end());

        auto computed_root = ssz_type.hash_tree_root();
        TEST_CHECK(computed_root == root.to_array());
    }
}

const auto test_fork = []() { test_ssz<eth::Fork>("Fork"); };
const auto test_forkdata = []() { test_ssz<eth::ForkData>("ForkData"); };
const auto test_checkpoint = []() { test_ssz<eth::Checkpoint>("Checkpoint"); };
const auto test_signingdata = []() { test_ssz<eth::SigningData>("SigningData"); };
const auto test_attestationdata = []() { test_ssz<eth::AttestationData>("AttestationData"); };
const auto test_indexedattestation = []() { test_ssz<eth::IndexedAttestation>("IndexedAttestation"); };
const auto test_pendingattestation = []() { test_ssz<eth::PendingAttestation>("PendingAttestation"); };
const auto test_attestation = []() { test_ssz<eth::Attestation>("Attestation"); };
const auto test_depositmessage = []() { test_ssz<eth::DepositMessage>("DepositMessage"); };
const auto test_depositdata = []() { test_ssz<eth::DepositData>("DepositData"); };
const auto test_deposit = []() { test_ssz<eth::Deposit>("Deposit"); };
const auto test_eth1data = []() { test_ssz<eth::Eth1Data>("Eth1Data"); };
const auto test_beaconblockheader = []() { test_ssz<eth::BeaconBlockHeader>("BeaconBlockHeader"); };
const auto test_beaconblockbody = []() { test_ssz<eth::BeaconBlockBody>("BeaconBlockBody"); };
const auto test_beaconblock = []() { test_ssz<eth::BeaconBlock>("BeaconBlock"); };
const auto test_signedbeaconblockheader = []() { test_ssz<eth::SignedBeaconBlockHeader>("SignedBeaconBlockHeader"); };
const auto test_signedbeaconblock = []() { test_ssz<eth::SignedBeaconBlock>("SignedBeaconBlock"); };
const auto test_proposerslashing = []() { test_ssz<eth::ProposerSlashing>("ProposerSlashing"); };
const auto test_attesterslashing = []() { test_ssz<eth::AttesterSlashing>("AttesterSlashing"); };
const auto test_voluntaryexit = []() { test_ssz<eth::VoluntaryExit>("VoluntaryExit"); };
const auto test_signedvoluntaryexit = []() { test_ssz<eth::SignedVoluntaryExit>("SignedVoluntaryExit"); };
const auto test_validator = []() { test_ssz<eth::Validator>("Validator"); };
const auto test_beaconstate = []() { test_ssz<eth::BeaconState>("BeaconState"); };

TEST_LIST = {{"serialize_fork", test_fork},
             {"serialize_forkdata", test_forkdata},
             {"serialize_checkpoint", test_checkpoint},
             {"serialize_signingdata", test_signingdata},
             {"serialize_attestationdata", test_attestationdata},
             {"serialize_indexedattestation", test_indexedattestation},
             {"serialize_pendingattestation", test_pendingattestation},
             {"serialize_attestation", test_attestation},
             {"serialize_depositmessage", test_depositmessage},
             {"serialize_depositdata", test_depositdata},
             {"serialize_deposit", test_deposit},
             {"serialize_eth1data", test_eth1data},
             {"serialize_beaconblockheader", test_beaconblockheader},
             {"serialize_beaconblockbody", test_beaconblockbody},
             {"serialize_beaconblock", test_beaconblock},
             {"serialize_signedbeaconblockheader", test_signedbeaconblockheader},
             {"serialize_signedbeaconblock", test_signedbeaconblock},
             {"serialize_proposerslashing", test_proposerslashing},
             {"serialize_attesterslashing", test_attesterslashing},
             {"serialize_voluntaryexit", test_voluntaryexit},
             {"serialize_signedvoluntaryexit", test_signedvoluntaryexit},
             {"serialize_validator", test_validator},
             {"serialize_beaconstate", test_beaconstate},
             {NULL, NULL}};
