/*  minimal.hpp
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
#include "common/basic_types.hpp"

using namespace std::string_literals;

namespace constants
{

const std::uint64_t MIN_GENESIS_ACTIVE_VALIDATOR_COUNT = 64;
const std::uint64_t MIN_GENESIS_TIME = 1578009600;
const eth::Version GENESIS_FORK_VERSION = 0x00000001;
const std::uint64_t GENESIS_DELAY = 300;


// ALTAIR_FORK_VERSION: 0x01000001
// ALTAIR_FORK_EPOCH: 18446744073709551615
// MERGE_FORK_VERSION: 0x02000001
// MERGE_FORK_EPOCH: 18446744073709551615
// SHARDING_FORK_VERSION: 0x03000001
// SHARDING_FORK_EPOCH: 18446744073709551615
// MIN_ANCHOR_POW_BLOCK_DIFFICULTY: 4294967296

const std::uint64_t SECONDS_PER_SLOT = 6;
const std::uint64_t SECONDS_PER_ETH1_BLOCK = 14;
const std::uint64_t MIN_VALIDATOR_WITHDRAWABILITY_DELAY = 256;
const std::uint64_t SHARD_COMMITTEE_PERIOD = 64;
const std::uint64_t ETH1_FOLLOW_DISTANCE = 16;


const eth::Gwei EJECTION_BALANCE = 16000000000;
const std::uint64_t MIN_PER_EPOCH_CHURN_LIMIT = 4;
const std::uint64_t CHURN_LIMIT_QUOTIENT = 65536;


const int DEPOSIT_CHAIN_ID = 5;
const int DEPOSIT_NETWORK_ID = 5;
const eth::Eth1Address DEPOSIT_CONTRACT_ADDRESS = "0x1234567890123456789012345678901234567890"s;

const std::string TEST_VECTORS_PATH = "../../../eth2.0-spec-tests/tests/minimal/phase0/ssz_static/";

constexpr int DEPOSIT_CONTRACT_TREE_DEPTH = 32;
constexpr std::uint64_t JUSTIFICATION_BITS_LENGTH = 4;

constexpr uint MAX_COMMITTEES_PER_SLOT = 4;
constexpr uint TARGET_COMMITTEE_SIZE = 4;
constexpr uint MAX_VALIDATORS_PER_COMMITTEE = 2048;
constexpr uint SHUFFLE_ROUND_COUNT = 10;
constexpr uint HYSTERESIS_QUOTIENT = 4;
constexpr uint HYSTERESIS_DOWNWARD_MULTIPLIER = 1;
constexpr uint HYSTERESIS_UPWARD_MULTIPLIER = 5;
constexpr uint SAFE_SLOTS_TO_UPDATE_JUSTIFIED = 2;
constexpr uint64_t MIN_DEPOSIT_AMOUNT = 1000000000;
constexpr uint64_t MAX_EFFECTIVE_BALANCE = 32000000000;
constexpr uint64_t EFFECTIVE_BALANCE_INCREMENT = 1000000000;
constexpr uint MIN_ATTESTATION_INCLUSION_DELAY = 1;
constexpr uint SLOTS_PER_EPOCH = 8;
constexpr uint MIN_SEED_LOOKAHEAD = 1;
constexpr uint MAX_SEED_LOOKAHEAD = 4;
constexpr uint EPOCHS_PER_ETH1_VOTING_PERIOD = 4;
constexpr uint SLOTS_PER_HISTORICAL_ROOT = 64;
constexpr uint MIN_EPOCHS_TO_INACTIVITY_PENALTY = 4;
constexpr uint EPOCHS_PER_HISTORICAL_VECTOR = 64;
constexpr uint EPOCHS_PER_SLASHINGS_VECTOR = 64;
constexpr uint64_t HISTORICAL_ROOTS_LIMIT = 16777216;
constexpr uint64_t VALIDATOR_REGISTRY_LIMIT = 1099511627776;
constexpr uint BASE_REWARD_FACTOR = 64;
constexpr uint WHISTLEBLOWER_REWARD_QUOTIENT = 512;
constexpr uint PROPOSER_REWARD_QUOTIENT = 8;
constexpr uint64_t INACTIVITY_PENALTY_QUOTIENT = 33554432;
constexpr uint MIN_SLASHING_PENALTY_QUOTIENT = 64;
constexpr uint PROPORTIONAL_SLASHING_MULTIPLIER = 2;
constexpr uint MAX_PROPOSER_SLASHINGS = 16;
constexpr uint MAX_ATTESTER_SLASHINGS = 2;
constexpr uint MAX_ATTESTATIONS = 128;
constexpr uint MAX_DEPOSITS = 16;
constexpr uint MAX_VOLUNTARY_EXITS = 16;

};
