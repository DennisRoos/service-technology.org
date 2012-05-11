/*!
\file DisjunctionStatePredicate.cc
\author Karsten
\status new

\brief class implementation for disjunction state predicates
*/

#include <cstdlib>
#include "Formula/DisjunctionStatePredicate.h"
#include "Net/Net.h"

DisjunctionStatePredicate::DisjunctionStatePredicate(index_t n)
{
    parent = NULL;
    sub = (StatePredicate**) malloc(n * SIZEOF_VOIDP);
    cardSub = n;
}

DisjunctionStatePredicate::~DisjunctionStatePredicate()
{
    for(index_t i = 0; i < cardSub; i++)
    {
	delete(sub[i]);
    }
    free(sub);
}

void DisjunctionStatePredicate::addSub(index_t i, StatePredicate* f)
{
    assert(i < cardSub);
    sub[i] = f;
    sub[i] -> position = i;
    sub[i]-> parent = this;
}

index_t DisjunctionStatePredicate::getUpSet(index_t* stack, bool* onstack)
{
    // call only if this formula is false
    assert(cardSat == 0);

    index_t stackpointer = 0;
    for (index_t i = 0; i < cardSub; i++)
    {
        stackpointer += sub[i] -> getUpSet(stack + stackpointer, onstack);
        assert(stackpointer <= Net::Card[TR]);
    }
    return stackpointer;
}

void DisjunctionStatePredicate::updateTF(index_t i)
{
    // assumption: satisfied left, unsatisfied right

    // --> sub[cardSat] is first unsatisfied
    if (cardSat-- == 1)
    {
        value = false;
        if (parent)
        {
            parent -> updateTF(position);
        }
    }

    StatePredicate* tmp = sub[cardSat];
    sub[cardSat] = sub[i];
    sub[i] = tmp;
    sub[i] -> position = i;
    sub[cardSat]->position = cardSat;
}

void DisjunctionStatePredicate::updateFT(index_t i)
{
    // assumption: satisfied left, unsatisfied right

    // --> sub[cardSat] is first satisfied
    StatePredicate* tmp = sub[cardSat];
    sub[cardSat] = sub[i];
    sub[i] = tmp;
    sub[i] -> position = i;
    sub[cardSat]->position = cardSat;

    if (++cardSat == 1)
    {
        value = true;
        if (parent)
        {
            parent -> updateFT(position);
        }
    }
}



void DisjunctionStatePredicate::evaluate()
{
    for (index_t i = 0; i < cardSub; i++)
    {
        sub[i] -> evaluate();
    }
    index_t left = 0;
    index_t right = cardSub;

    // sort satisfied to left, unsat to right of sub list
    // loop invariant: formulas left of left (not including left) are satisfied,
    // formulas right of right (including right) are unsatisfied
    while (true)
    {
        while (left < cardSub && sub[left]->value)
        {
            ++left;
        }
        while (right > 0 && !sub[right - 1]->value)
        {
            --right;
        }
        if (left >= right) // array sorted
        {
            break;
        }
        StatePredicate* tmp = sub[left];
        sub[left++] = sub[--right];
        sub[right] = tmp;
    }
    cardSat = left;

    value = (cardSat > 0);

    // update position in sub formulas
    for (index_t i = 0; i < cardSub; i++)
    {
        sub[i]->position = i;
    }
}

index_t DisjunctionStatePredicate::countAtomic()
{
    index_t result = 0;

    for (index_t i = 0; i < cardSub; i++)
    {
        result += sub[i] -> countAtomic();
    }
    return result;
}

index_t DisjunctionStatePredicate::collectAtomic(AtomicStatePredicate** p)
{
    index_t offset = 0;
    for (index_t i = 0; i < cardSub; i++)
    {
        offset += collectAtomic(p + offset);
    }
    return offset;
}