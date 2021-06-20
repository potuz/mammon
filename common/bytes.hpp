/*  bytes.hpp 
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

#include <stdexcept>
#include <cstddef>
#include <array>
#include <vector>
#include <iomanip>
#include <iostream>
#include <concepts>

namespace eth
{
    template<std::size_t N>
    class Bytes {
        private:

            std::array<std::byte, N> m_arr;
            
            static constexpr std::array<std::byte, N> bytes_from_int(std::integral auto value)
            {
                if (sizeof(value) > N)
                    throw std::out_of_range("integer type is larger than bytes size");

                std::array<std::byte, N> ret_arr = {};
                for (int i = 0; (i < sizeof(value)) && (i < N); ++i)
                    ret_arr[i] = std::byte( (value >> 8*i) & 0xff);
                
                return ret_arr;
            }

            static constexpr auto chars_to_byte(std::string const & c) -> std::byte
            {
                    unsigned int x;
                    std::stringstream ss;
                    ss << std::hex << c;
                    ss >> x;
                    return std::byte(x);
            }

            static constexpr std::array<std::byte, N> bytes_from_str(std::string &str)
            {
                if (!str.starts_with("0x"))
                    throw std::invalid_argument("string not prepended with 0x");

                str.erase(0,2);
               
                if (str.size() > 2*N)
                    throw std::out_of_range("integer larger than bytes size");
                
                if (str.size() % 2 == 1)
                    str.insert(0,"0");

                std::array<std::byte, N> ret_arr = {};
                for (int i = 0; i < str.size(); i += 2)
                    ret_arr[i/2] = chars_to_byte(str.substr(i,2));
                return ret_arr;
            }

        public:
            
            template<typename ...T>
            Bytes (T&&...l) : m_arr {{std::forward<std::byte>(std::byte(l))...}} {};

            Bytes (std::string hex) :
                m_arr {bytes_from_str(hex)} {};
            
            Bytes (std::integral auto value) : 
                m_arr { bytes_from_int(value) } {};

            Bytes (std::array<std::byte,N> arr) :
                m_arr {arr} {};

            Bytes (std::vector<std::byte> arr)
            {
                if (arr.size() != N)
                    throw std::out_of_range ("vector of different size than bytes");
                std::copy_n(arr.begin(), N, m_arr.begin());
            }

            constexpr std::byte* data() noexcept
            {
                return m_arr.data();
            }
            const std::array<std::byte, N> &to_array() const
            {
                return m_arr;
            }

            std::string to_string() const
            {
                std::stringstream os;
                std::ios_base::fmtflags save = std::cout.flags();
                os << "0x";
                for (auto i = m_arr.cbegin(); i != m_arr.cend(); ++i)
                    os << std::setfill('0') << std::setw(2) << std::hex << std::to_integer<int>(*i);
                std::cout.flags(save);
                return os.str();
            };

            std::byte& operator [](std::size_t index)
            {
                return m_arr[index];
            }
            
            std::byte operator [](std::size_t index) const
            {
                return m_arr[index];
            }

            void operator ^=(Bytes<N> const & b)
            {
                for (int i = 0; i < N; i++)
                    m_arr[i]^=b[i];
            }

            template<std::size_t M>
            constexpr bool operator==(Bytes<M> const &r)
            {
                if ( N!=M )
                    return false;
                return m_arr == r.to_array();
            }

            constexpr bool operator==(std::vector<std::byte> const &r)
            {
                if (N!=r.size())
                    return false;
                for (int i=0; i<N; ++i)
                    if (m_arr[i] != r[i])
                        return false;
                return true;
            }

            template <std::size_t M>
            Bytes<N+M> operator+(Bytes<M> const &r) const
            {
                std::array<std::byte, N+M> ret;
                std::copy(m_arr.cbegin(), m_arr.cend(), ret.begin());
                std::copy(r.to_array().cbegin(), r.to_array().cend(), ret.begin() + N);
                return Bytes<N+M>(ret);
            }

            std::uint64_t to_uint64()
            {
                if (N > 8)
                    throw std::out_of_range("bytes size larger than uint64");
                uint64_t ret_uint = 0;
                for (int i=0; i < N; ++i)
                    ret_uint |= (std::to_integer<std::uint64_t>(m_arr[i]) << 8*i);
                return ret_uint;
            }

            std::size_t size(void) const
            {
                return N;
            }

            constexpr std::array<std::byte, N>::iterator begin() noexcept
            {
                return m_arr.begin();
            }

            constexpr std::array<std::byte, N>::const_iterator cbegin() const noexcept
            {
                return m_arr.cbegin();
            }

            constexpr std::array<std::byte, N>::iterator end() noexcept
            {
                return m_arr.end();
            }

            constexpr std::array<std::byte, N>::const_iterator cend() const noexcept
            {
                return m_arr.cend();
            }
    };

    using Bytes4 = Bytes<4>;
    using Bytes8 = Bytes<8>;
    using Bytes20 = Bytes<20>;
    using Bytes32 = Bytes<32>;
    using Bytes48 = Bytes<48>;
    using Bytes96 = Bytes<96>;
}

