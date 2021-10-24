/*  main.cpp
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

#include <iostream>
#include "common/bytes.hpp"
#include "beacon-chain/transaction.hpp"
#include "helpers/bytes_to_string.hpp"

int main(int argc, char *argv[]) { 
    
    eth::Bytes<121> txBytes{"0x02f8768314677280843b9aca07843b9aca0782520894aaec86394441f915bce3e6ab399977e9906f3b69893635c9adc5dea0000080c001a0d83c6dd0abcd2d5ba2f4307342eb9027c197dc3db0a27cc497e6098a2348aa87a01647170d490f7adfc45ee5045502e75da3425b5d86d7088a086fdeec78361699"};

    eth::Transaction tx{};
    auto vec = tx.data();
    vec.insert(vec.end(), txBytes.cbegin(), txBytes.cend());
    auto txhash = tx.hash_tree_root();
    std::cout << "tx Bytes : " << txBytes.to_string() << std::endl;
    std::cout << "tx root: " << helpers::bytes_to_string(txhash) << std::endl;

    return 0; 

}
