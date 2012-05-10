/*!
\file StatePredicate.h
\author Karsten
\status new

\brief derives deadlock checking from SimpleProperty
*/

#pragma once

#include "Core/Dimensions.h"

class AtomicStatePredicate;

/// A state predicate is a formula that assigns a Boolean value to Marking::Current
class StatePredicate
{
    public:
        /// Stack is an array of distinct transition indices. onstack is an array of booleans, initially all false.
        /// getUpSet fills stack with an up set of the predicate. Added elements are marked true in onstack.
        /// getUpSet can be called only if the predicate is not satisfied. The returned set of transitions has the
        /// property that it is impossible to turn the predicate true without firing one of the transitions.
        /// The return value is the size of the returned up set.
        virtual index_t getUpSet(index_t* stack, bool* onstack) = 0;

        /// The value of the predicate in Marking::Current;
        bool value;

        /// updates value of predicate upon change of value in subformula (TF = true to false, FT = false to true).
        /// If value of this changes, the parent formula is
        /// triggered for updating. This means that updating is started at the leafs of the formula tree.
        /// Parts of the formula that did not change are not examined.
        /// The parameter is the position of this in the parent's subformula list

        virtual void updateTF(index_t) = 0;
        virtual void updateFT(index_t) = 0;

        /// evaluates a formula, e.g. upon initialization. Evaluation starts top/down, so the whole formula is
        /// examined.
        virtual void evaluate() = 0;

        /// The parent formula in the syntax tree of the formula
        StatePredicate* parent;

        /// the position of this in parent's subformula list
        index_t position;

	/// counts atomic subformulas
	virtual index_t countAtomic() = 0;

	/// collects atomic subformulas; array must be malloced beforehand
	/// result is number of inserted elements
	virtual index_t collectAtomic(AtomicStatePredicate **) = 0;
};
