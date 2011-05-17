// File: variable.h
// Header for the Variable struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef VARIABLE_H
#define VARIABLE_H
#include "symbol.h"
#include "type.h"
#include "stdidents.h"

//------------------------------------------------------------------------------
// Symbol struct: struct for all Pascal variables.
//
// ADT Symbol struct: struct which maintains infomation for a variable 
// including name, type, and if it is a refrence supports equality, inequality, 
// lessthan and display operations. Some derived classes support displayVal 
// and clone operations.
//------------------------------------------------------------------------------
struct Variable : public Symbol {
	Type *type;			// The type of the variable.
	bool ref;			// Is the variable a refence.

	Variable();   		// Constructor
	~Variable();		// Destructor

	// Function called to display a variable of a given scope.
	void display(int indent)const;
};

#endif