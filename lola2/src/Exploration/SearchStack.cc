/*!
\file SearchStack.cc
\author Karsten
\status new

\brief Organizes the search stack for dfs or bfs.
*/

#include <cstdlib>
#include <cstdio>
#include "Core/Dimensions.h"
#include "Exploration/SearchStack.h"


SearchStack::SearchStack() : StackPointer(0), currentchunk(NULL)
{
}

void SearchStack::push(index_t c, index_t* f)
{
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
        // need new chunk
        Chunk* newchunk = new Chunk();
        newchunk->prev = currentchunk;
        currentchunk = newchunk;
    }
    currentchunk->current[StackPointer % SIZEOF_STACKCHUNK] = c;
    currentchunk->list[StackPointer++ % SIZEOF_STACKCHUNK] = f;
}

/*!
\param[in,out] c  index within firelist
\param[in,out] f  firelist
*/
void SearchStack::pop(index_t* c, index_t** f)
{
    *c = currentchunk->current[(--StackPointer) % SIZEOF_STACKCHUNK];
    *f = currentchunk->list[StackPointer % SIZEOF_STACKCHUNK];
    if ((StackPointer % SIZEOF_STACKCHUNK) == 0)
    {
        // need to jump to previous chunk
        Chunk* tempchunk = currentchunk;
        currentchunk = currentchunk->prev;
        delete tempchunk;
    }
}

index_t SearchStack::topTransition() const
{
    return currentchunk->list[(StackPointer - 1) % SIZEOF_STACKCHUNK][currentchunk->current[(StackPointer - 1) % SIZEOF_STACKCHUNK]];
}
