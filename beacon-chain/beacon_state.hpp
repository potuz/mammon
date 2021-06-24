/*  beacon_state.hpp 
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
#include "beacon_block.hpp" 
#include "ssz/ssz_container.hpp"

namespace eth
{
    class BeaconState : public ssz::Container
    {
        private:
            UnixTime   genesis_time_;
            Root       genesis_validators_root_;
            Slot    slot_;
            Fork    fork_;
            BeaconBlockHeader   latest_block_header_;
            VectorFixedSizedParts<Root, constants::SLOTS_PER_HISTORICAL_ROOT> block_roots_, state_roots_;
            ListFixedSizedParts<Root> historical_roots_;
            Eth1Data   eth1_data_;
            ListFixedSizedParts<Eth1Data>   eth1_data_votes_;
            DepositIndex                    eth1_deposit_index_;
            ListFixedSizedParts<Validator>  validators_;
            ListFixedSizedParts<Gwei>       balances_;
            VectorFixedSizedParts<Bytes32,constants::EPOCHS_PER_HISTORICAL_VECTOR>    randao_mixes_;
            VectorFixedSizedParts<Gwei, constants::EPOCHS_PER_SLASHING_VECTOR> slashings_;
            ListVariableSizedParts<PendingAttestation> previous_epoch_attestations_, current_epoch_attestations_;
            Bitvector<constants::JUSTIFICATION_BITS_LENGTH>   justification_bits_;
            Checkpoint  previous_justified_checkpoint_, current_justified_checkpoint_, finalized_checkpoint_;

        public:
            constexpr UnixTime genesis_time() const
            {
                return genesis_time_;
            }
            constexpr const Root& genesis_validators_root() const
            {
                return genesis_validators_root_;
            }
            constexpr Slot slot() const
            {
                return slot_;
            }
            constexpr const Fork& fork() const
            {
                return fork_;
            }
            constexpr const BeaconBlockHeader& latest_block_header() const
            {
                return latest_block_header_;
            }
            constexpr const auto& block_roots() const
            {
                return block_roots_;
            }
            constexpr const auto& state_roots() const
            {
                return state_roots_;
            }
            constexpr const auto& historical_roots() const
            {
                return historical_roots_;
            }
            constexpr const Eth1Data& eth1_data() const
            {
                return eth1_data_;
            }
            constexpr const auto& eth1_data_votes() const
            {
                return eth1_data_votes_;
            }
            constexpr const auto& eth1_deposit_index() const
            {
                return eth1_deposit_index_;
            }
            constexpr const auto& validators() const
            {
                return validators_;
            }
            constexpr const auto& balances() const
            {
                return balances_;
            }
            constexpr const auto& slashings() const
            {
                return slashings_;
            }
            constexpr const auto& randao_mixes() const
            {
                return randao_mixes_;
            }
            constexpr const auto& previous_epoch_attestations() const
            {
                return previous_epoch_attestations_;
            }
            constexpr const auto& current_epoch_attestations() const
            {
                return current_epoch_attestations_;
            }
            constexpr const auto& justification_bits() const
            {
                return justification_bits_;
            }
            constexpr const Checkpoint& previous_justified_checkpoint() const
            {
                return previous_justified_checkpoint_;
            }
            constexpr const Checkpoint& current_justified_checkpoint() const
            {
                return current_justified_checkpoint_;
            }
            constexpr const Checkpoint& finalized_checkpoint() const
            {
                return finalized_checkpoint_;
            }

/*
            void genesis_time(UnixTime);
            void genesis_validators_root(Root);
            void slot(Slot);
            void fork(Fork);
            void latest_block_header(BeaconBlockHeader);
            void block_roots(<Root>);
            void state_roots(std::vector<Root>);
            void historical_roots(std::vector<Root>);
            void eth1_data(Eth1Data);
            void eth1_data_votes(std::vector<Eth1Data>);
            void eth1_deposit_index(DepositIndex);
            void validators(std::vector<Validator>);
            void balances(std::vector<Gwei>);
            void slashings(std::vector<Gwei>);
            void randao_mixes(std::vector<Bytes32>);
            void previous_epoch_attestations(std::vector<PendingAttestation>);
            void current_epoch_attestations(std::vector<PendingAttestation>);
            void justification_bits(std::vector<bool>);
            void previous_justified_checkpoint(Checkpoint);
            void current_justified_checkpoint(Checkpoint);
            void finalized_checkpoint(Checkpoint);
            */

            BytesVector serialize() const
            {
                return serialize_({
                        &genesis_time_,
                        &genesis_validators_root_,
                        &slot_,
                        &fork_,
                        &latest_block_header_,
                        &block_roots_,
                        &state_roots_,
                        &historical_roots_,
                        &eth1_data_,
                        &eth1_data_votes_,
                        &eth1_deposit_index_,
                        &validators_,
                        &balances_,
                        &randao_mixes_,
                        &slashings_,
                        &previous_epoch_attestations_,
                        &current_epoch_attestations_,
                        &justification_bits_,
                        &previous_justified_checkpoint_,
                        &current_justified_checkpoint_,
                        &finalized_checkpoint_});
            }

            YAML::Node encode() const
            { 
                return encode_({
                        {"genesis_time", &genesis_time_},
                        {"genesis_validators_root", &genesis_validators_root_},
                        {"slot", &slot_},
                        {"fork", &fork_},
                        {"latest_block_header", &latest_block_header_},
                        {"block_roots", &block_roots_},
                        {"state_roots", &state_roots_},
                        {"historical_roots", &historical_roots_},
                        {"eth1_data", &eth1_data_},
                        {"eth1_data_votes", &eth1_data_votes_},
                        {"eth1_deposit_index", &eth1_deposit_index_},
                        {"validators", &validators_},
                        {"balances", &balances_},
                        {"randao_mixes", &randao_mixes_},
                        {"slashings", &slashings_},
                        {"previous_epoch_attestations", &previous_epoch_attestations_},
                        {"current_epoch_attestations", &current_epoch_attestations_},
                        {"justification_bits", &justification_bits_},
                        {"previous_justified_checkpoint", &previous_justified_checkpoint_},
                        {"current_justified_checkpoint", &current_justified_checkpoint_},
                        {"finalized_checkpoint", &finalized_checkpoint_} });
            }

            bool decode(const YAML::Node& node) 
            { 
                return decode_(node, {
                        {"genesis_time", &genesis_time_},
                        {"genesis_validators_root", &genesis_validators_root_},
                        {"slot", &slot_},
                        {"fork", &fork_},
                        {"latest_block_header", &latest_block_header_},
                        {"block_roots", &block_roots_},
                        {"state_roots", &state_roots_},
                        {"historical_roots", &historical_roots_},
                        {"eth1_data", &eth1_data_},
                        {"eth1_data_votes", &eth1_data_votes_},
                        {"eth1_deposit_index", &eth1_deposit_index_},
                        {"validators", &validators_},
                        {"balances", &balances_},
                        {"randao_mixes", &randao_mixes_},
                        {"slashings", &slashings_},
                        {"previous_epoch_attestations", &previous_epoch_attestations_},
                        {"current_epoch_attestations", &current_epoch_attestations_},
                        {"justification_bits", &justification_bits_},
                        {"previous_justified_checkpoint", &previous_justified_checkpoint_},
                        {"current_justified_checkpoint", &current_justified_checkpoint_},
                        {"finalized_checkpoint", &finalized_checkpoint_} });
            }
    };
}

