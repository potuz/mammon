/*  hashtree.hpp
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

#include <array>
#include <cstddef>
#include <cstring>
#include <vector>

#include "ssz/ssz.hpp"

namespace ssz {
using Chunk = std::array<std::byte, constants::BYTES_PER_CHUNK>;

class HashTree {
   private:
    std::vector<Chunk> hash_tree_;
    int depth_, effective_depth_;
    std::size_t effective_start_, inner_limit_, cache_size_;
    const Chunk& hash_node(const std::vector<Chunk>& vec, std::size_t idx);

   public:
    HashTree(const std::vector<Chunk>& chunks, std::uint64_t limit = 0);
    HashTree(const std::vector<std::byte>& vec, std::uint64_t limit = 0);

    void mix_in(std::size_t length);
    std::vector<Chunk>& hash_tree() { return hash_tree_; }
    Chunk hash_tree_root() const { return hash_tree_[0]; }
};

}  // namespace ssz
