/*  bitvector.hpp 
 * 
 *  This file is part of sszpp. 
 *  sszpp is C++ implementation of the "Simple Serialize" specification. 
 *  https://github.com/ethereum/eth2.0-specs/tree/dev/ssz 
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
#include <cassert>
#include "ssz/ssz_container.hpp"

namespace eth
{
    template<unsigned N> 
    class Bitvector : public ssz::Container
    {
        private:

            std::array<bool, N>     m_arr;

        public:

            Bitvector (std::array<bool, N> vec) : ssz::Container (N), m_arr {vec} {};
            template<typename ...T>
            Bitvector (T&&...l) : ssz::Container (N), m_arr {{std::forward<T>(l)...}} {};

            virtual std::vector<std::byte> serialize() const
            {
                Bytes<(N+7)/8> ret {}; 
                for (int i=0; i < N; ++i)
                    ret[i/8] |= (std::byte(m_arr[i] << (i%8)));
                return ret;
            };


            friend std::ostream& operator<< (std::ostream& os, const Bitvector<N>& m_bits)
            {
                for (auto const &b : m_bits.m_arr)
                    os << b;
                return os;
            };
    };
}

