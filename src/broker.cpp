/***************************************************************************
 *            broker.cpp
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

#include "broker.hpp"

namespace Opera {

void BrokerManager::add(Broker const& broker) {
    _brokers.insert(std::make_pair(broker.kind(),broker));
}

SizeType BrokerManager::num_brokers() const {
    return _brokers.size();
}

Broker& BrokerManager::get(BrokerKind const& kind) {
    ARIADNE_PRECONDITION(_brokers.has_key(kind))
    return _brokers.at(kind);
}

void BrokerManager::clear() {
    _brokers.clear();
}

}