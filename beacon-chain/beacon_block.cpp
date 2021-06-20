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
    std::vector<std::byte> BeaconBlockBody::serialize() const
    {
        // Insert the fixed parts  200 bytes. 
        std::vector<std::byte> ret;
        ret.insert(ret.begin(), randao_reveal_.cbegin(), randao_reveal_.cend());

        auto eth1_ssz = eth1_data_.serialize();
        ret.insert(ret.end(), eth1_ssz.begin(), eth1_ssz.end());
        ret.insert(ret.end(), graffiti_.cbegin(), graffiti_.cend());

        //The first offset is fixed
        auto offset = Bytes4( std::uint32_t( 5*constants::BYTES_PER_LENGTH_OFFSET + 200));
        ret.insert(ret.end(), offset.begin(), offset.end());

        auto ps_ssz = ssz::serialize_list<ProposerSlashing,false>(proposer_slashings_);

        offset = Bytes4 (std::uint32_t(
                        5*constants::BYTES_PER_LENGTH_OFFSET + 200 + ps_ssz.size()));
        ret.insert(ret.end(), offset.begin(), offset.end()); 

        auto as_ssz = ssz::serialize_list<AttesterSlashing>(attester_slashings_);
        offset = Bytes4(std::uint32_t( 5*constants::BYTES_PER_LENGTH_OFFSET + 200 + ps_ssz.size() +
                    as_ssz.size()));
        ret.insert(ret.end(), offset.begin(), offset.end()); 

        auto atts_ssz = ssz::serialize_list<Attestation>(attestations_); 
        offset = Bytes4(std::uint32_t( 5*constants::BYTES_PER_LENGTH_OFFSET + 200 + 
                        ps_ssz.size() + as_ssz.size() + atts_ssz.size()));
        ret.insert(ret.end(), offset.begin(), offset.end()); 

        auto deps_ssz = ssz::serialize_list<Deposit, false>(deposits_);
        offset = Bytes4(std::uint32_t( 5*constants::BYTES_PER_LENGTH_OFFSET + 200 + 
                ps_ssz.size() + as_ssz.size() + atts_ssz.size() + deps_ssz.size()));
        ret.insert(ret.end(), offset.begin(), offset.end()); 

        // Insert the variable size parts (TODO: perhaps use make_move_iterator)
        ret.insert(ret.end(), ps_ssz.begin(), ps_ssz.end());
        ret.insert(ret.end(), as_ssz.begin(), as_ssz.end());
        ret.insert(ret.end(), atts_ssz.begin(), atts_ssz.end());
        ret.insert(ret.end(), deps_ssz.begin(), deps_ssz.end());

        auto ve_ssz = ssz::serialize_list<SignedVoluntaryExit,false>(voluntary_exits_);
        ret.insert(ret.end(), ve_ssz.begin(), ve_ssz.end());
        return ret;
    }
        

    std::vector<std::byte> BeaconBlock::serialize() const
    {
        Bytes<84> fixed = Bytes8(slot_) + Bytes8 (proposer_index_) + parent_root_ + state_root_ + 
            Bytes4 (std::uint32_t(84));
        auto ret = body_.serialize();
        ret.insert(ret.begin(), fixed.begin(), fixed.end()); 
        return ret;
    }

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
    void BeaconBlockBody::proposer_slashings(std::vector<ProposerSlashing> p)
    {
        proposer_slashings_ = std::move(p);
    }
    void BeaconBlockBody::attester_slashings(std::vector<AttesterSlashing> a)
    {
        attester_slashings_ = std::move(a);
    }
    void BeaconBlockBody::attestations(std::vector<Attestation> a)
    {
        attestations_ = std::move(a);
    }
    void BeaconBlockBody::deposits(std::vector<Deposit> d)
    {
        deposits_ = std::move(d);
    }
    void BeaconBlockBody::voluntary_exits(std::vector<SignedVoluntaryExit> s)
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

    std::vector<std::byte> SignedBeaconBlock::serialize() const
    {
        auto block_ssz = message.serialize();
        Bytes4 offset = std::uint32_t(100);
        block_ssz.insert(block_ssz.begin(), signature.cbegin(), signature.cend());
        block_ssz.insert(block_ssz.begin(), offset.begin(), offset.end());
        return block_ssz;
    }
        
}
