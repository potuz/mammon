/*  validator.hpp 
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
#include "ssz/ssz_container.hpp"

namespace eth
{
    class Validator : public ssz::Container
    {
        private:
            BLSPubkey pubkey;
            Bytes32 withdrawal_credentials;
            Gwei effective_balance;
            Boolean slashed;
            Epoch activation_eligibility_epoch, activation_epoch, exit_epoch, withdrawable_epoch;

        public:
            static constexpr std::size_t ssz_size = 153;
            std::size_t get_ssz_size() const { return ssz_size; } 
            BytesVector serialize() const
            {
                return serialize_({
                        &pubkey, 
                        &withdrawal_credentials,
                        &effective_balance,
                        &slashed,
                        &activation_eligibility_epoch,
                        &activation_epoch,
                        &exit_epoch,
                        &withdrawable_epoch}); 
            }

            YAML::Node encode() const
            { 
                return encode_({
                        { "pubkey", &pubkey}, 
                        { "withdrawal_credentials", &withdrawal_credentials},
                        { "effective_balance", &effective_balance},
                        { "slashed", &slashed},
                        { "activation_eligibility_epoch", &activation_eligibility_epoch},
                        { "activation_epoch", &activation_epoch},
                        { "exit_epoch", &exit_epoch},
                        { "withdrawable_epoch", &withdrawable_epoch} }); 
            }

            bool decode(const YAML::Node& node) 
            { 
                return decode_(node, {
                        { "pubkey", &pubkey}, 
                        { "withdrawal_credentials", &withdrawal_credentials},
                        { "effective_balance", &effective_balance},
                        { "slashed", &slashed},
                        { "activation_eligibility_epoch", &activation_eligibility_epoch},
                        { "activation_epoch", &activation_epoch},
                        { "exit_epoch", &exit_epoch},
                        { "withdrawable_epoch", &withdrawable_epoch} }); 
            }

    };
}
