/*!
\author Karsten
\file SymbolTable.cc
\status approved 25.01.2012
\ingroup g_frontend g_symboltable
\todo Mal Kollisionen zählen

\brief class implementation for a symbol table
*/

#include <inttypes.h>
#include <cstring>
#include <cstdlib>
#include "Dimensions.h"
#include "SymbolTable.h"

/// Intialization amounts to setting all entries to NULL
SymbolTable::SymbolTable()
    : table(reinterpret_cast<Symbol**>(calloc(SIZEOF_SYMBOLTABLE, sizeof(Symbol*)))),
      card(0),
      currentIndex(0),
      currentSymbol(NULL)
{
}

/// Destructor calls destructors for all present symbols
SymbolTable::~SymbolTable()
{
    for (int i = 0; i < SIZEOF_SYMBOLTABLE; i++)
    {
        while (table[i])
        {
            Symbol* tmp = table[i];
            table[i] = table[i]->getNext();
            delete tmp;
        }
    }
    free(table);
}

/// We use sum of ASCII values as hash value
unsigned int SymbolTable::hash(const char* s) const
{
    uint64_t result = 0;

    for (size_t i = 0; s[i]; i++)
    {
        result += (uint64_t)s[i];
    }
    result %= SIZEOF_SYMBOLTABLE;
    return (unsigned int) result;
}

/*!
\brief lookup an element from symbol table

if input is correct, lookup is used for existing symbols

\return NULL is returned if key is not present which typically
        indicates a syntax error "double definition"
*/
Symbol* SymbolTable::lookup(const char* str) const
{
    for (Symbol* sym = table[hash(str)]; sym; sym = sym -> getNext())
    {
        if (!strcmp(sym -> getKey(), str))
        {
            return sym;
        }
    }
    return NULL;
}

/*!
\brief insert an element into symbol table

if input is correct, insert is used when key is not yet present

\return If input is present, false is returned.
        This typically indicates a syntax error "used but not defined
*/
bool SymbolTable::insert(Symbol* sym)
{
    const unsigned int index = hash(sym -> getKey());
    for (Symbol* othersym = table[index]; othersym; othersym = othersym -> getNext())
    {
        if (!strcmp(othersym -> getKey(), sym -> getKey()))
        {
            return false;
        }
    }
    sym -> setNext(table[index]);
    table[index] = sym;
    ++card;
    return true;
}

/// \return pointer to first element in symbol table
Symbol* SymbolTable::first()
{
    for (currentIndex = 0; currentIndex < SIZEOF_SYMBOLTABLE; currentIndex++)
    {
        if (table[currentIndex])
        {
            return(currentSymbol = table[currentIndex]);
        }
    }
    return NULL;
}

/// \return pointer to next element in symbol table
Symbol* SymbolTable::next()
{
    if (currentSymbol -> getNext())
    {
        // there is another element in the same hash bucket
        return(currentSymbol = currentSymbol -> getNext());
    }
    else
    {
        // there is no other element in the same bucket
        for (++currentIndex; currentIndex < SIZEOF_SYMBOLTABLE; currentIndex++)
        {
            if (table[currentIndex])
            {
                return(currentSymbol = table[currentIndex]);
            }
        }
        return NULL;
    }
}

/// returns number of entries in table
unsigned int SymbolTable::getCard() const
{
    return card;
}
