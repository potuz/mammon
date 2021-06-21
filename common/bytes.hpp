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
#include "ssz/ssz_container.hpp"
#include "yaml-cpp/yaml.h"

namespace eth
{
    template<std::size_t N>
    class Bytes : public ssz::Container 
    {
        private:

            std::array<std::byte, N> m_arr;

            static auto bytes_from_int(std::integral auto value) -> std::array<std::byte, N> 
            {
                if (sizeof(value) > N)
                    throw std::out_of_range("integer type is larger than bytes size");

                std::array<std::byte, N> ret_arr = {};
                for (int i = 0; (i < sizeof(value)) && (i < N); ++i)
                    ret_arr[i] = std::byte( (value >> 8*i) & 0xff);
                
                return ret_arr;
            }

            static auto chars_to_byte(std::string const & c) -> std::byte
            {
                    unsigned int x;
                    std::stringstream ss;
                    ss << std::hex << c;
                    ss >> x;
                    return std::byte(x);
            }

            static auto bytes_from_str(std::string &str) ->  std::array<std::byte, N>
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

            Bytes() : ssz::Container(N), m_arr {} {}; 
            Bytes( const Bytes& b ) : ssz::Container(N), m_arr {b.m_arr} {};
            Bytes( std::string hex ) : ssz::Container(N), m_arr {bytes_from_str(hex)} {};
            Bytes( std::integral auto value ) : ssz::Container(N), m_arr { bytes_from_int(value) } {};
            Bytes( std::array<std::byte,N> arr ) : ssz::Container(N), m_arr {arr} {};
            Bytes( std::vector<std::byte> arr ) : ssz::Container(N)
            {
                if (arr.size() != N)
                    throw std::out_of_range ("vector of different size than bytes");
                std::copy_n(arr.begin(), N, m_arr.begin());
            }

            virtual std::vector<std::byte> serialize() const
            {
                std::vector<std::byte> ret(m_arr.cbegin(),m_arr.cend());
                return ret;
            }

            operator std::vector<std::byte>()
            {
                std::vector<std::byte> ret(m_arr.begin(),m_arr.end());
                return ret;
            }

            constexpr std::byte* data() noexcept
            {
                return m_arr.data();
            }

            const std::array<std::byte, N>& to_array() const
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

            std::size_t size(void) const
            {
                return N;
            }

            constexpr typename std::array<std::byte, N>::iterator begin() noexcept
            {
                return m_arr.begin();
            }

            constexpr typename std::array<std::byte, N>::const_iterator cbegin() const noexcept
            {
                return m_arr.cbegin();
            }

            constexpr typename std::array<std::byte, N>::iterator end() noexcept
            {
                return m_arr.end();
            }

            constexpr typename std::array<std::byte, N>::const_iterator cend() const noexcept
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

    using BytesVector = std::vector<std::byte>;
}
namespace YAML
{
    template <
        template <std::size_t> class C,
        std::size_t N
    >
    requires std::is_same<C<N>, eth::Bytes<N>>::value
    struct convert<C<N>>
    {
        static Node encode(const C<N>& b)
        {
            return Node(b.to_string());
        }

        static bool decode(const Node& node, C<N>& b)
        {
            b = node.as<std::string>();
            return true;
        }
    };
}
