/*  basic_types.h 
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
#include "slot.hpp"

namespace eth 
{
    using Epoch = Slot;
    using CommitteeIndex = Slot;
    using ValidatorIndex = Slot;
    using Gwei = Slot;

    using Root = Bytes32;
    using Hash32 = Bytes32;

    using Version = Bytes4;
    using DomainType = Bytes4;
    using ForkDigest = Bytes4;

    using Domain = Bytes32;

    using BLSPubkey = Bytes48;
    using BLSSignature = Bytes96;

    using Eth1Address = Bytes20;
}

