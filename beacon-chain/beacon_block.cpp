/*  beacon_block.cpp 
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

#include "beacon_block.hpp"
#include "ssz/ssz.hpp"

namespace eth
{
    void BeaconBlockBody::randao_reveal(BLSSignature s) 
    {
        randao_reveal_ = std::move(s);
    }
    void BeaconBlockBody::eth1_data(Eth1Data data)
    {
        eth1_data_ = std::move(data);
    }
    void BeaconBlockBody::graffiti(Bytes32 g)
    {
        graffiti_ = std::move(g);
    }
    void BeaconBlockBody::proposer_slashings(ListFixedSizedParts<ProposerSlashing> p)
    {
        proposer_slashings_ = std::move(p);
    }
    void BeaconBlockBody::attester_slashings(ListVariableSizedParts<AttesterSlashing> a)
    {
        attester_slashings_ = std::move(a);
    }
    void BeaconBlockBody::attestations(ListVariableSizedParts<Attestation> a)
    {
        attestations_ = std::move(a);
    }
    void BeaconBlockBody::deposits(ListFixedSizedParts<Deposit> d)
    {
        deposits_ = std::move(d);
    }
    void BeaconBlockBody::voluntary_exits(ListFixedSizedParts<SignedVoluntaryExit> s)
    {
        voluntary_exits_ = std::move(s);
    }

    void BeaconBlock::slot(Slot s)
    {
        slot_ = s;
    }
    void BeaconBlock::proposer_index(ValidatorIndex idx)
    {
        proposer_index_ = idx;
    }
    void BeaconBlock::parent_root(Root r)
    {
        parent_root_ = std::move(r);
    }
    void BeaconBlock::state_root(Root r)
    {
        state_root_ = std::move(r);
    }
    void BeaconBlock::body(BeaconBlockBody b)
    {
        body_ = std::move(b);
    }
}
