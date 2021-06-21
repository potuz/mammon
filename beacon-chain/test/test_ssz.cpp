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
#include "acutest.h" 
#include "config.hpp"
#include "common/types.hpp" 
#include "beacon-chain/attestation.hpp" 
#include "snappy.h"
#include "yaml-cpp/yaml.h"

namespace fs = std::filesystem;

template<typename T>
void test_ssz(const std::string &path)
{
    auto base_path = constants::TEST_VECTORS_PATH + path;
    for (auto& p1 : fs::directory_iterator(base_path))
        for (auto& p2 : fs::directory_iterator(p1))
            for (auto& p3 : fs::directory_iterator(p2))
            {
               auto node = YAML::LoadFile(p2.path().string() + "/value.yaml");
               T ssz_type = node.as<T>();
               auto serialized = ssz_type.serialize();

               auto ssz_snappy_path = p2.path().string() + "/serialized.ssz_snappy";
               std::ifstream ssz_snappy(ssz_snappy_path, std::ios::in | std::ios::binary);
               if (not ssz_snappy.is_open())
                   throw std::filesystem::filesystem_error("could not open file", p2.path(), std::error_code());
               
               const std::size_t size = std::filesystem::file_size(ssz_snappy_path);
               std::vector<std::byte> content(size);
               ssz_snappy.read(reinterpret_cast<char*>(content.data()), size);
               ssz_snappy.close();

               std::vector<std::byte> output(serialized.size());
               if (!snappy::RawUncompress(reinterpret_cast<char*>(content.data()), size,
                           reinterpret_cast<char*>(output.data())))
                   throw std::filesystem::filesystem_error("could not uncompress file", p2.path(), 
                           std::error_code());

               TEST_CHECK(serialized == output);
               TEST_MSG("Processing file: %s", ssz_snappy_path.c_str());
               std::size_t i = 0;
               while(serialized[i] == output[i])
                  i++;
               TEST_MSG("First byte difference in position %x", i);
               TEST_DUMP("Expected:", output.data(), serialized.size());
               TEST_DUMP("Produced:", serialized.data(), serialized.size());

            }
}

auto test_fork = [](){ test_ssz<eth::Fork>("Fork"); }; 
auto test_forkdata = [](){ test_ssz<eth::ForkData>("ForkData"); }; 
auto test_checkpoint = [](){ test_ssz<eth::Checkpoint>("Checkpoint"); }; 
auto test_signingdata = [](){ test_ssz<eth::SigningData>("SigningData"); }; 
auto test_attestationdata = []() { test_ssz<eth::AttestationData>("AttestationData"); };
//auto test_indexedattestation = []() { test_ssz<eth::IndexedAttestation>("IndexedAttestation"); };
auto test_pendingattestation = []() { test_ssz<eth::PendingAttestation>("PendingAttestation"); };
//auto test_attestation = []() { test_ssz<eth::Attestation>("Attestation"); };
//auto test_depositmessage = []() { test_ssz<eth::DepositMessage>("DepositMessage"); };
//auto test_depositdata = []() { test_ssz<eth::DepositData>("DepositData"); };
//auto test_deposit = []() { test_ssz<eth::Deposit>("Deposit"); };
//auto test_eth1data = []() { test_ssz<eth::Eth1Data>("Eth1Data"); };
//auto test_beaconblockheader = []() { test_ssz<eth::BeaconBlockHeader>("BeaconBlockHeader"); };
//auto test_beaconblockbody = []() { test_ssz<eth::BeaconBlockBody>("BeaconBlockBody"); };
//auto test_beaconblock= []() { test_ssz<eth::BeaconBlock>("BeaconBlock"); };
//auto test_signedbeaconblockheader = []() { test_ssz<eth::SignedBeaconBlockHeader>("SignedBeaconBlockHeader"); };
//auto test_signedbeaconblock= []() { test_ssz<eth::SignedBeaconBlock>("SignedBeaconBlock"); };
//auto test_proposerslashing = []() { test_ssz<eth::ProposerSlashing>("ProposerSlashing"); };
//auto test_attesterslashing = []() { test_ssz<eth::AttesterSlashing>("AttesterSlashing"); };
//auto test_voluntaryexit = []() { test_ssz<eth::VoluntaryExit>("VoluntaryExit"); };
//auto test_signedvoluntaryexit = []() { test_ssz<eth::SignedVoluntaryExit>("SignedVoluntaryExit"); };

TEST_LIST = {
    { "serialize_fork", test_fork },
    { "serialize_forkdata", test_forkdata },
    { "serialize_checkpoint", test_checkpoint },
    { "serialize_signingdata", test_signingdata },
    { "serialize_attestationdata", test_attestationdata },
//    { "serialize_indexedattestation", test_indexedattestation },
    { "serialize_pendingattestation", test_pendingattestation },
//    { "serialize_attestation", test_attestation },
//    { "serialize_depositmessage", test_depositmessage},
//    { "serialize_depositdata", test_depositdata},
//    { "serialize_deposit", test_deposit},
//    { "serialize_eth1data", test_eth1data},
//    { "serialize_beaconblockheader", test_beaconblockheader},
//    { "serialize_beaconblockbody", test_beaconblockbody},
//    { "serialize_beaconblock", test_beaconblock},
//    { "serialize_signedbeaconblockheader", test_signedbeaconblockheader},
//    { "serialize_signedbeaconblock", test_signedbeaconblock},
//    { "serialize_proposerslashing", test_proposerslashing},
//    { "serialize_attesterslashing", test_attesterslashing},
//    { "serialize_voluntaryexit", test_voluntaryexit},
//    { "serialize_signedvoluntaryexit", test_signedvoluntaryexit},
    { NULL, NULL }
};


