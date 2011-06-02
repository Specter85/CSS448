// File: symboltable.h
// Header for SymbolTable class.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "scope.h"
#include "symbol.h"
#include "stdidents.h"
#include <string>

// Extern for sit defined in symboltable.cpp.
extern StdIdents sit;

//------------------------------------------------------------------------------
// class SymbolTable: A class which stores symbol and scope information.
//
// ADT SymbolTable class: A class which stores symbol and scope information for
// a Pascal progam, which supports insertion, lookUp(current scope), lookUP( for
// current scope and all parent scopes), scope entry, scope exit, and printing.
//------------------------------------------------------------------------------
class SymbolTable {
private:

	Scope *root;							// Pointer to the root scope.
	Scope *currentScope;					// Pointer to the current scope.

	// Recursive print helper function.
	void printHelper(Scope *rhs);
	// Recursive clear fucntion.
	void clear(Scope *rhs);

public:

	SymbolTable();							// Constructor.
	~SymbolTable();							// Destructor.

	// Function for inserting symbols.
	bool insert(Symbol *rhs);
	// Function for looking up symbols in the current scope and all its
	// parent's scopes.
	Symbol *lookUp(const string &rhs);
	// Function for looking up symbols in the current scope.
	Symbol *lookUpCS(const string &rhs);

	// Function for entering a scope.
	void enterScope(Scope *rhs);
	// Function for exiting the current scope.
	void exitScope();

	// Function for invalidating bad pointers.
	void cullBadPointers();

	// Function for invalidating bad types.
	void cullBadTypes(bool &clean);

	// Function for getting Scope level.
	int getScopeLevel()const;

	string getScopeName()const;

	// function for outputing the current object.
	void printST();

};

#endif