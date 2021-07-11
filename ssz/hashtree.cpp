/*  hashtree.cpp
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

#include "hashtree.hpp"

#include <stdexcept>

#include "common/bytes.hpp"
#include "helpers/math.hpp"
#include "openssl/sha.h"
#include "ssz/ssz.hpp"

namespace {
using namespace ssz;
constexpr Chunk zero_hash{};

Chunk hash_2_chunks(const std::byte* data) {
    Chunk ret{};
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, 2 * constants::BYTES_PER_CHUNK);
    SHA256_Final(reinterpret_cast<unsigned char*>(ret.data()), &sha256);
    return ret;
}

Chunk hash_2_chunks(const Chunk& first, const Chunk& second) {
    std::vector<std::byte> sum(first.begin(), first.end());
    sum.insert(sum.end(), second.begin(), second.end());
    return hash_2_chunks(sum.data());
}

// clang-format off
template <std::size_t N> requires(N > 0)
auto zero_hash_array_helper() {
    std::array<Chunk, N> ret{};
    ret[0] = zero_hash;
    for (auto it = ret.begin() + 1; it != ret.end(); ++it) *it = hash_2_chunks(*std::prev(it), *std::prev(it));
    return ret;
}
// clang-format on

const auto ZERO_HASH_DEPTH{42};
const auto zero_hash_array = zero_hash_array_helper<ZERO_HASH_DEPTH>();

/**
 *   \brief Packs a vector of bytes into chunks of 32 bytes
 *   \details If the length of vec is not a multiple of 32, it pads with null bytes in the end
 *   \param[in] vec  a vector of std::bytes.
 */
std::vector<Chunk> pack_and_pad(const std::vector<std::byte>& vec) {
    std::vector<Chunk> ret{};
    if (vec.empty())
        ret.push_back(zero_hash);
    else {
        ret.reserve(vec.size() + sizeof(Chunk) - vec.size() % sizeof(Chunk));
        for (auto it = vec.cbegin(); it < vec.end(); it += constants::BYTES_PER_CHUNK) {
            Chunk chunk{};
            std::copy(it, std::min(it + constants::BYTES_PER_CHUNK, vec.end()), chunk.begin());
            ret.push_back(chunk);
        }
    }
    return ret;
}

}  // namespace

namespace ssz {
/**
 * \brief Compute the hash tree of a node in a partial merkle tree
 *
 * \param[in] vec       the data in the leaves of the tree, will be padded by zero.
 * \param[in] idx       the index of the node we want to compute
 *
 */
const Chunk& HashTree::hash_node(const std::vector<Chunk>& vec, std::size_t idx) {
    if (vec.size() == 1) {
        if (depth_ == 1)
            hash_tree_ = vec;
        else if (vec[0] == zero_hash)
            std::reverse_copy(zero_hash_array.begin() + 1, zero_hash_array.begin() + hash_tree_.size() + 1,
                              hash_tree_.begin());
        else {
            hash_tree_.back() = hash_2_chunks(vec[0], zero_hash);
            auto it = hash_tree_.rbegin() + 1;
            auto zhit = zero_hash_array.begin() + 1;
            while (it != hash_tree_.rend()) {
                *it = hash_2_chunks(*std::prev(it), *zhit);
                ++it;
                ++zhit;
            }
        }
        return hash_tree_[0];
    }
    if (idx < effective_start_)
        hash_tree_[idx] = hash_2_chunks(hash_node(vec, idx + 1), zero_hash_array[depth_ - idx - 2]);  // NOLINT
    else if (idx < inner_limit_)
        hash_tree_[idx] = hash_2_chunks(hash_node(vec, 2 * idx - effective_start_ + 1),
                                        hash_node(vec, 2 * idx - effective_start_ + 2));
    else if (idx < cache_size_) {
        if (2 * (idx - inner_limit_) + 1 < vec.size())
            hash_tree_[idx] = hash_2_chunks(vec[2 * (idx - inner_limit_)], vec[2 * (idx - inner_limit_) + 1]);
        else if (2 * (idx - inner_limit_) + 1 == vec.size())
            hash_tree_[idx] = hash_2_chunks(vec.back(), zero_hash);
        else
            hash_tree_[idx] = zero_hash_array[1];
    } else
        throw std::out_of_range("index larger that tree size.");

    return hash_tree_[idx];
}

HashTree::HashTree(const std::vector<Chunk>& chunks, std::uint64_t limit) {
    if (chunks.empty()) throw std::out_of_range("empty chunks is not allowed");
    effective_depth_ = helpers::log2ceil(chunks.size()) + 1;
    depth_ = (limit == 0) ? effective_depth_ : helpers::log2ceil(limit) + 1;
    cache_size_ = depth_ - effective_depth_ + std::bit_ceil(chunks.size()) - 1;
    effective_start_ = depth_ - effective_depth_;
    // NOLINTNEXTLINE
    inner_limit_ = (chunks.size() == 1) ? effective_start_ : effective_start_ + (1 << (effective_depth_ - 2)) - 1;
    if (cache_size_) hash_tree_.resize(cache_size_);
    hash_node(chunks, 0);
}

void HashTree::mix_in(std::size_t length) {
    auto length_bytes = eth::Bytes32(length);
    hash_tree_.insert(hash_tree_.begin(), hash_2_chunks(hash_tree_[0], length_bytes.to_array()));
}

HashTree::HashTree(const std::vector<std::byte>& vec, std::uint64_t limit) : HashTree{pack_and_pad(vec), limit} {};

}  // namespace ssz
