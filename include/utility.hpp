/***************************************************************************
 *            utility.hpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Opera.
 *
 *  Opera is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Opera is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Opera.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OPERA_UTILITY_HPP
#define OPERA_UTILITY_HPP

#include <filesystem>
#include <map>
#include "declarations.hpp"
#include "config.hpp"

using FilePath = std::filesystem::path;

namespace Opera {

class Resources {
  public:
    static FilePath path(std::string const& filename) {
        return (std::string(RESOURCES_PATH) + filename).c_str();
    }
};

template<class T> inline std::string to_string(const T& t) { std::stringstream ss; ss << t; return ss.str(); }

//! \brief Soft wrapper to expose the key finding functionality
template<class K, class V> class Map : public std::map<K,V> {
  public:
    bool has_key(K const& key) const { return this->find(key) != this->end(); }
};

template<class T> std::ostream& operator<<(std::ostream& os, List<T> const& l) {
    if (l.empty()) return os << "[]";
    os << "[" << l.at(0);
    for (SizeType i=1;i<l.size();++i) os << "," << l.at(i);
    return os << "]";
}

}

#endif //OPERA_UTILITY_HPP
