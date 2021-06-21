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

namespace ssz
{
    class Container
    {
        private:
            std::size_t size_;          // the serialized size
        
        protected:
            static std::vector<std::byte> serialize_(std::vector<const Container*>);

        public:

            Container(std::size_t size = 0) : size_ {size} {};

            virtual ~Container() = default;
            virtual std::vector<std::byte> serialize() const = 0;
    };

}


