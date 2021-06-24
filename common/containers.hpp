/*  containers.hpp 
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
#include "common/basic_types.hpp"
#include "ssz/ssz.hpp" 
#include "ssz/ssz_container.hpp" 
#include "yaml-cpp/yaml.h"

namespace eth

{
    template<class T, std::size_t N> 
    class VectorFixedSizedParts : public ssz::Container
    {
        private:
            std::array<T,N> m_arr;

        public:
            static constexpr std::size_t ssz_size = N*T::ssz_size; 
            std::size_t get_ssz_size() const { return ssz_size;  }
            
            std::size_t size(void) const
            {
                return N;
            }

            constexpr typename std::array<T,N>::iterator begin() noexcept
            {
                return m_arr.begin();
            }

            constexpr typename std::array<T,N>::const_iterator cbegin() const noexcept
            {
                return m_arr.cbegin();
            }

            constexpr typename std::array<T,N>::iterator end() noexcept
            {
                return m_arr.end();
            }

            constexpr typename std::array<T,N>::const_iterator cend() const noexcept
            {
                return m_arr.cend();
            }

            BytesVector serialize() const
            {
                BytesVector ret;
                for (auto part : m_arr)
                {
                    auto part_ssz = part.serialize();
                    ret.insert(ret.end(), part_ssz.begin(), part_ssz.end());
                }
                return ret;
            }
            YAML::Node encode() const 
            {
                return YAML::convert<std::array<T,N>>::encode(m_arr);
            }
            bool decode(const YAML::Node& node)
            { 
                return YAML::convert<std::array<T,N>>::decode(node, m_arr);
            }
    };

    template<class T> 
    class ListFixedSizedParts : public ssz::Container
    {
        private:
            std::vector<T> m_arr;

        public:
            std::size_t size(void) const
            {
                return m_arr.size();
            }

            constexpr typename std::vector<T>::iterator begin() noexcept
            {
                return m_arr.begin();
            }

            constexpr typename std::vector<T>::const_iterator cbegin() const noexcept
            {
                return m_arr.cbegin();
            }

            constexpr typename std::vector<T>::iterator end() noexcept
            {
                return m_arr.end();
            }

            constexpr typename std::vector<T>::const_iterator cend() const noexcept
            {
                return m_arr.cend();
            }

            BytesVector serialize() const
            {
                BytesVector ret;
                for (auto part : m_arr)
                {
                    auto part_ssz = part.serialize();
                    ret.insert(ret.end(), part_ssz.begin(), part_ssz.end());
                }
                return ret;
            }
            YAML::Node encode() const 
            {
                return YAML::convert<std::vector<T>>::encode(m_arr);
            }
            bool decode(const YAML::Node& node)
            { 
                return YAML::convert<std::vector<T>>::decode(node, m_arr);
            }
    };

    template<class T> 
    class ListVariableSizedParts : public ssz::Container
    {
        private:
            std::vector<T> m_arr;

        public:
            std::size_t size(void) const
            {
                return m_arr.size();
            }

            constexpr typename std::vector<T>::iterator begin() noexcept
            {
                return m_arr.begin();
            }

            constexpr typename std::vector<T>::const_iterator cbegin() const noexcept
            {
                return m_arr.cbegin();
            }

            constexpr typename std::vector<T>::iterator end() noexcept
            {
                return m_arr.end();
            }

            constexpr typename std::vector<T>::const_iterator cend() const noexcept
            {
                return m_arr.cend();
            }

            BytesVector serialize() const
            {
                BytesVector offsets,ret;
                std::uint32_t offset = size()*constants::BYTES_PER_LENGTH_OFFSET;
                for (auto part : m_arr)
                {
                    auto offset_ssz = Bytes4(offset).serialize();
                    offsets.insert(offsets.end(), offset_ssz.begin(), offset_ssz.end());

                    auto part_ssz = part.serialize();
                    offset += part_ssz.size();
                    ret.insert(ret.end(), part_ssz.begin(), part_ssz.end());
                }
                if (offsets.size())
                    ret.insert(ret.begin(), offsets.begin(), offsets.end());
                return ret;
            }
            YAML::Node encode() const 
            {
                return YAML::convert<std::vector<T>>::encode(m_arr);
            }
            bool decode(const YAML::Node& node)
            { 
                return YAML::convert<std::vector<T>>::decode(node, m_arr);
            }
    };

    struct Fork : public ssz::Container
    {
        Version previous_version, current_version;
        Epoch epoch;

        static constexpr std::size_t ssz_size = 16;
        std::size_t get_ssz_size() const { return ssz_size; } 
        BytesVector serialize() const
        {
            return serialize_({&previous_version, &current_version, &epoch});
        }

        YAML::Node encode() const
        { 
            return encode_({
                    { "previous_version", &previous_version },
                    { "current_version", &current_version },
                    { "epoch", &epoch } });
        }

        bool decode(const YAML::Node& node) 
        { 
            return decode_(node, {
                    { "previous_version", &previous_version },
                    { "current_version", &current_version },
                    { "epoch", &epoch } });
        }

    };

    struct ForkData : public ssz::Container
    {
        Version current_version;
        Root    genesis_validators_root;

        static constexpr std::size_t ssz_size = 36;
        std::size_t get_ssz_size() const { return ssz_size; } 
        BytesVector serialize() const
        {
            return serialize_({&current_version, &genesis_validators_root});
        }

        YAML::Node encode() const
        { 
            return encode_({
                    { "current_version", &current_version },
                    { "genesis_validators_root", &genesis_validators_root } });
        }

        bool decode(const YAML::Node& node) 
        { 
            return decode_(node, {
                    { "current_version", &current_version },
                    { "genesis_validators_root", &genesis_validators_root } });
        }

    };

    struct Checkpoint : public ssz::Container
    {
        Epoch epoch;
        Root  root;

        static constexpr std::size_t ssz_size = 40;
        std::size_t get_ssz_size() const { return ssz_size; } 
        BytesVector serialize() const { return serialize_({&epoch, &root}); }

        YAML::Node encode() const
        { 
            return encode_({
                    { "epoch", &epoch },
                    { "root", &root } });
        }

        bool decode(const YAML::Node& node) 
        { 
            return decode_(node, {
                    { "epoch", &epoch },
                    { "root", &root } });
        }

    };

    struct SigningData : public ssz::Container
    {
        Root object_root;
        Domain domain;

        static constexpr std::size_t ssz_size = 64;
        std::size_t get_ssz_size() const { return ssz_size; } 
        BytesVector serialize() const { return serialize_({&object_root, &domain}); }

        YAML::Node encode() const
        { 
            return encode_({
                    { "object_root", &object_root },
                    { "domain", &domain} });
        }

        bool decode(const YAML::Node& node) 
        { 
            return decode_(node, {
                    { "object_root", &object_root },
                    { "domain", &domain} });
        }

    };

}
