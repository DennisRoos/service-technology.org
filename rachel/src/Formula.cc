/*****************************************************************************\
 Rachel -- Reparing Service Choreographies

 Copyright (c) 2008, 2009 Niels Lohmann

 Rachel is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Rachel.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <config.h>
#include <set>

#include "Formula.h"

/*
 * constructors
 */
FormulaAND::FormulaAND(Formula* _left, Formula* _right)
    : left(_left), right(_right) {
    assert(_left);
    assert(_right);
}

FormulaOR::FormulaOR(Formula* _left, Formula* _right)
    : left(_left), right(_right) {
    assert(_left);
    assert(_right);
}

FormulaLit::FormulaLit(const Label& _literal)
    : literal(_literal) {
}


/*
 * Dot output
 */

std::string FormulaAND::toDot(bool noBrackets) const {
    if (noBrackets) {
        return left->toDot(false) + "<FONT>& and; </FONT>" + right->toDot(false);
    } else {
        return "(" + left->toDot(false) + "<FONT>& and; </FONT>" + right->toDot(false) + ")";
    }
}

std::string FormulaOR::toDot(bool noBrackets) const {
    if (noBrackets) {
        return left->toDot(false) + "<FONT>& or; </FONT>" + right->toDot(false);
    } else {
        return "(" + left->toDot(false) + "<FONT>& or; </FONT>" + right->toDot(false) + ")";
    }
}

std::string FormulaLit::toDot(bool) const {
    return literal;
}

std::string FormulaTrue::toDot(bool) const {
    return "true";
}

std::string FormulaFalse::toDot(bool) const {
    return "false";
}

std::string FormulaFinal::toDot(bool) const {
    return "final";
}


/*
 * satisfaction test
 */

bool FormulaAND::sat(const std::set<Label>& l) const {
    return (left->sat(l) and right->sat(l));
}

bool FormulaOR::sat(const std::set<Label>& l) const {
    return (left->sat(l) or right->sat(l));
}

bool FormulaLit::sat(const std::set<Label>& l) const {
    return (l.find(literal) != l.end());
}

bool FormulaTrue::sat(const std::set<Label>&) const {
    return true;
}

bool FormulaFalse::sat(const std::set<Label>&) const {
    return false;
}


/*!
 * When calculating the correcting matching between a service automaton and an
 * OG, we require the service automaton's final states to be sink states, i.e.
 * have no successors (see BPM2008 paper). Therefore, evaulation of formulas of
 * nodes in which "final" occurred were not necessary, because in that case, a
 * further correction of the automaton would be suboptimal (because adding more
 * nodes beyond that point would increase the edit distance and will never be
 * necessary due to the requirement that final states are sink states).
 *
 * When counting the services characterizes by an OG, however, the evaluation
 * for formulas in which "final" occurs is important. Then, we can set the
 * predicate "final" to true, because it does not change the number of
 * characterized services, but only whether the characterized services have a
 * final state or not.
 *
 * This bug was found by Martin Znamirowski. See <http://gna.org/bugs/?11944>.
 */
bool FormulaFinal::sat(const std::set<Label>& l) const {
    return true;  // true by default
}

