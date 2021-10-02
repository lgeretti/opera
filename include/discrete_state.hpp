/***************************************************************************
 *            discrete_state.hpp
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

#ifndef OPERA_DISCRETE_STATE_HPP
#define OPERA_DISCRETE_STATE_HPP

#include <initializer_list>
#include "declarations.hpp"
#include "utility.hpp"

namespace Opera {

using String = std::string;

//! \brief A map of string variables with values
class DiscreteState {
  public:
    //! \brief Construct empty
    DiscreteState() = default;
    //! \brief Construct from a single pair
    DiscreteState(const Pair<String,String>& pair);
    //! \brief Construct from a map directly
    DiscreteState(const Map<String,String>& sm);
    //! \brief Construct from an initialiser list of pairs
    DiscreteState(std::initializer_list<Pair<String,String>> const& vals);

    //! \brief Whether there are no variables defined
    bool is_empty() const;

    //! \brief The values held
    Map<String,String> const& values() const;

    //! \relates DiscreteState \brief Equality test.
    //! Throws an IndistinguishableLocationsError if the valuations are not identical but have no variable with distinct values.
    friend bool operator==(const DiscreteState& loc1, const DiscreteState& loc2);
    //! \relates DiscreteState \brief Inequality test.
    friend bool operator!=(const DiscreteState& loc1, const DiscreteState& loc2);
    //! \relates DiscreteState \brief A total order on DiscreteState, allowing comparison of non-distinuishable valuations.
    friend bool operator<(const DiscreteState& loc1, const DiscreteState& loc2);

    //! \brief Print to the standard output
    friend std::ostream& operator<<(std::ostream& os, DiscreteState const& s);
  private:
    Map<String,String> _mapping;
};

} //namespace Opera

#endif /* OPERA_DISCRETE_STATE_HPP */
