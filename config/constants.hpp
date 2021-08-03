/*  constants.hpp
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

#include "common/slot.hpp"

namespace constants {
constexpr auto GENESIS_SLOT = eth::Slot{0};
constexpr auto GENESIS_EPOCH = eth::Epoch{0};
constexpr auto FAR_FUTURE_EPOCH = eth::Epoch{0xffffffffffffffffull};
constexpr auto BASE_REWARDS_PER_EPOCH = 0x4ull;
constexpr auto DEPOSIT_CONTRACT_TREE_DEPTH = 0X20ull;
constexpr auto JUSTIFICATION_BITS_LENGTH = 0x4ull;
constexpr auto MAX_EFFECTIVE_BALANCE = eth::Gwei{32000000000};
constexpr auto MIN_DEPOSIT_AMOUNT = eth::Gwei{1000000000};
constexpr auto EFFECTIVE_BALANCE_INCREMENT = eth::Gwei{1000000000};
constexpr uint MIN_ATTESTATION_INCLUSION_DELAY = 1;
} // namespace constants
