/*  ssz_container.hpp 
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
#include <vector>
#include <cstddef>
#include <string>
#include "yaml-cpp/yaml.h"

namespace ssz
{
    class Container;
    using Part = std::pair<std::string, Container*>;
    using ConstPart = std::pair<std::string, const Container*>;

    class Container
    {
        protected:
            static std::vector<std::byte> serialize_(std::vector<const Container*>);
            static YAML::Node encode_(std::vector<ConstPart> parts);
            static bool decode_(const YAML::Node& node, std::vector<Part> parts);

        public:

            virtual ~Container() = default;

            virtual std::size_t get_ssz_size() const { return 0; }
            virtual std::vector<std::byte> serialize() const = 0;

            virtual YAML::Node encode() const = 0;
            virtual bool decode(const YAML::Node& node) = 0; 
    };

}

template<class T> requires std::is_base_of<ssz::Container, T>::value

struct YAML::convert<T>
{
    static YAML::Node encode(const ssz::Container& c) { return c.encode(); }
    static bool decode(const YAML::Node& node, ssz::Container& c) { return c.decode(node); }
};