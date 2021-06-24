/*  mainnet.hpp
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

#include <cstdint>
#include "common/types.hpp"

using namespace std::string_literals;
namespace constants
{

const std::uint64_t MIN_GENESIS_ACTIVE_VALIDATOR_COUNT = 16384;
const std::uint64_t MIN_GENESIS_TIME = 1606824000;
const eth::Version GENESIS_FORK_VERSION = 0x00000000;
const std::uint64_t GENESIS_DELAY = 604800;


// ALTAIR_FORK_VERSION: 0x01000001
// ALTAIR_FORK_EPOCH: 18446744073709551615
// MERGE_FORK_VERSION: 0x02000001
// MERGE_FORK_EPOCH: 18446744073709551615
// SHARDING_FORK_VERSION: 0x03000001
// SHARDING_FORK_EPOCH: 18446744073709551615
// MIN_ANCHOR_POW_BLOCK_DIFFICULTY: 4294967296

const std::uint64_t SECONDS_PER_SLOT = 12;
const std::uint64_t SECONDS_PER_ETH1_BLOCK = 14;
const std::uint64_t MIN_VALIDATOR_WITHDRAWABILITY_DELAY = 256;
const std::uint64_t SHARD_COMMITTEE_PERIOD = 256;
const std::uint64_t ETH1_FOLLOW_DISTANCE = 2048;


const eth::Gwei EJECTION_BALANCE = 16000000000;
const std::uint64_t MIN_PER_EPOCH_CHURN_LIMIT = 4;
const std::uint64_t CHURN_LIMIT_QUOTIENT = 65536;


const int DEPOSIT_CHAIN_ID = 1;
const int DEPOSIT_NETWORK_ID = 1;
const eth::Eth1Address DEPOSIT_CONTRACT_ADDRESS = "0x00000000219ab540356cBB839Cbe05303d7705Fa"s;

const std::string TEST_VECTORS_PATH = "../../../eth2.0-spec-tests/tests/mainnet/phase0/ssz_static/";

constexpr int DEPOSIT_CONTRACT_TREE_DEPTH = 32;
constexpr std::uint64_t SLOTS_PER_HISTORICAL_ROOT = 8192;
constexpr std::uint64_t EPOCHS_PER_SLASHING_VECTOR = 8192;
constexpr std::uint64_t EPOCHS_PER_HISTORICAL_VECTOR = 65536;
constexpr std::uint64_t JUSTIFICATION_BITS_LENGTH = 4;
}
