/*  bench_sha256_impl.cpp
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

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "beacon-chain/beacon_state.hpp"
#include "bench/bench_sha256_impl.hpp"

namespace {
constexpr auto BENCH_ROUNDS = 10;
} // namespace

int bench::bench_sha256_impl(const char* path) {
    std::ifstream ssz_file(path, std::ios::in | std::ios::binary);
    if (not ssz_file.is_open())
    {
        std::cout << "could not open file" <<  path << std::endl;
        return 1;
    }

    const auto size = std::filesystem::file_size(path);
    std::vector<std::uint8_t> content(size);
    // NOLINTNEXTLINE
    if (!ssz_file.read(reinterpret_cast<char *>(content.data()), size)) {
        std::cout << "could not read from file.\n";
        return 1;
    }
    ssz_file.close();

    auto obj = std::make_unique<eth::BeaconState>();
    if (!obj->deserialize(content.begin(), content.end())) {
        std::cout << "could not deserialize Beacon State\n";
        return 1;
    }

    std::chrono::nanoseconds hashtime{0}, longest{0};

    for (auto i = 0; i < BENCH_ROUNDS; i++) {
        auto start = std::chrono::steady_clock::now();
        auto root = obj->hash_tree_root();
        auto end = std::chrono::steady_clock::now();
        hashtime += end - start;
        longest = (longest >  end - start) ? longest : end - start;
    }
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(hashtime).count() / BENCH_ROUNDS;
    std::cout << "              ";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(longest).count() << std::endl;
    return 0;
}
