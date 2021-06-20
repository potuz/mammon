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
#include "beacon_block.hpp" 

namespace eth
{
    class BeaconState
    {
        private:
            std::uint64_t   genesis_time_;
            Root            genesis_validators_root_;
            Slot    slot_;
            Fork    fork_;
            BeaconBlockHeader   latest_block_header_;
            std::vector<Root> blocks_roots_, state_roots_;
            Eth1Data   eth1_data_;
            std::vector<Eth1Data>   eth1_data_votes_;
            std::vector<Validator>  validators_;
            std::vector<Gwei>       balances_, slashings_;
            std::vector<Bytes32>    randao_mixes_;
            std::vector<PendingAttestation> previous_epoch_attestations_, current_epoch_attestations_;
            std::vector<bool>   justification_bits_;
            Checkpoint  previous_justified_checkpoint_, current_justified_checkpoint_, finalized_checkpoint_;

        public:
            constexpr std::uint64_t genesis_time() const
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
            constexpr const std::vector<Root>& blocks_roots() const
            {
                return blocks_roots_;
            }
            constexpr const std::vector<Root>& state_roots() const
            {
                return state_roots_;
            }
            constexpr const Eth1Data& eth1_data() const
            {
                return eth1_data_;
            }
            constexpr const std::vector<Eth1Data>& eth1_data_votes() const
            {
                return eth1_data_votes_;
            }
            constexpr const std::vector<Validator>& validators() const
            {
                return validators_;
            }
            constexpr const std::vector<Gwei>& balances() const
            {
                return balances_;
            }
            constexpr const std::vector<Gwei>& slashings() const
            {
                return slashings_;
            }
            constexpr const std::vector<Bytes32>& randao_mixes() const
            {
                return randao_mixes_;
            }
            constexpr const std::vector<PendingAttestation>& previous_epoch_attestations() const
            {
                return previous_epoch_attestations_;
            }
            constexpr const std::vector<PendingAttestation>& current_epoch_attestations() const
            {
                return current_epoch_attestations_;
            }
            constexpr const std::vector<bool> justification_bits() const
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

            void genesis_time(std::uint64_t);
            void genesis_validators_root(Root);
            void slot(Slot);
            void fork(Fork);
            void latest_block_header(BeaconBlockHeader);
            void blocks_roots(std::vector<Root>);
            void state_roots(std::vector<Root>);
            void eth1_data(Eth1Data);
            void eth1_data_votes(std::vector<Eth1Data>);
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
    };
}

