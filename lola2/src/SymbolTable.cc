/*!
 * \author Karsten
 * \file class implementation for a symbol table
 */

#include"Dimensions"
#include"Symbol"
#include<string>

/// Intialization amounts to setting all entries to NULL
SymbolTable::SymbolTable()
{
	for(int i = 0; i < SIZEOF_SYMBOLTABLE; i++)
	table[i] = NULL;
}
	
/// We use sum of ASCII values as hash value
unsigned int SymbolTable::hash(string s)
{	
	unsigned long int result = 0;
	
	for(int i = 0; i < s.length(); i++)
	{
		result += s[i];
		result %= SIZEOF_SYMBOLTABLE;
	}
	return (unsigned int) result;
}

/// lookup an element from symbol table
/// if input is correct, lookup is used for existing symbols
/// NULL is returned if key is not present which typically
/// indicates a syntax error "double definition"

Symbol * SymbolTable::lookup(string str)
{
	for(Symbol sym = table[hash(atr)]; sym; sym = sym -> next)
	{
		if(sym -> key == str)
		{
			return sym;
		}
	}	
	return NULL;
}

/// insert an element into symbol table
/// if input is correct, insert is used when key is not yet present
/// If input is present, false is returned.
/// This typically indicates a syntax error "used but not defined
bool SymbolTable::insert(Symbol * sym)
{
	int index = hash(sym -> key);
	for(Symbol * othersym = table[index];othersym;othersym=othersym -> next)
	{
		if(othersym -> key == sym -> key)
		{
			return false;
		}
	}
	sym -> next = table[index];
	table[index] = sym;
	return true;
}
