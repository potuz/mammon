/*  validator.cpp
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
#include "beacon-chain/validator.hpp"

namespace eth {
    bool Validator::deserialize(ssz::SSZIterator it, ssz::SSZIterator end) {
        return deserialize_(it, end,
                            {&pubkey, &withdrawal_credentials, &effective_balance, &slashed,
                             &activation_eligibility_epoch, &activation_epoch, &exit_epoch, &withdrawable_epoch});
    }
    bool Validator::is_active(Epoch epoch) const noexcept { return activation_epoch <= epoch && epoch < exit_epoch; }
    bool Validator::is_eligible_for_activation_queue() const noexcept {
        return activation_eligibility_epoch == constants::FAR_FUTURE_EPOCH && 
            effective_balance  == constants::MAX_EFFECTIVE_BALANCE; 
    }

    YAML::Node Validator::encode() const {
        return encode_({{"pubkey", &pubkey},
                        {"withdrawal_credentials", &withdrawal_credentials},
                        {"effective_balance", &effective_balance},
                        {"slashed", &slashed},
                        {"activation_eligibility_epoch", &activation_eligibility_epoch},
                        {"activation_epoch", &activation_epoch},
                        {"exit_epoch", &exit_epoch},
                        {"withdrawable_epoch", &withdrawable_epoch}});
    }
    bool Validator::decode(const YAML::Node &node) {
        return decode_(node, {{"pubkey", &pubkey},
                              {"withdrawal_credentials", &withdrawal_credentials},
                              {"effective_balance", &effective_balance},
                              {"slashed", &slashed},
                              {"activation_eligibility_epoch", &activation_eligibility_epoch},
                              {"activation_epoch", &activation_epoch},
                              {"exit_epoch", &exit_epoch},
                              {"withdrawable_epoch", &withdrawable_epoch}});
    }
} // namespace eth
