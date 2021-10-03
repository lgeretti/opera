/***************************************************************************
 *            discrete_state.hpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of Opera, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
