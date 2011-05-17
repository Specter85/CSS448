// File: scope.h
// Header for the scope struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef SCOPE_H
#define SCOPE_H
#include "bintree.h"

//------------------------------------------------------------------------------
// Symbol struct: base struct for Pascal scopes.
//
// ADT Symbol struct: struct which maintains information about a scope including
// its level, parent, next sibling, first child, and the symbols in it.
//------------------------------------------------------------------------------
struct Scope {
	int level;					// The level of the scope.
	Scope *parent;				// The scope's parent.
	Scope *next;				// The scope's next sibling.
	Scope *child;				// The first child of the scope.
	BinTree idents;				// Binary tree of symbols in the scope.

	// Constructor
	Scope(int level, Scope *parent = NULL, Scope *next = NULL, 
		Scope *child = NULL);
	// Destructor
	virtual ~Scope();

};

#endif