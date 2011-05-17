// File: function.h
// Header for the ProcFunc struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef PROC_FUNC_H
#define PROC_FUNC_H
#include "symbol.h"
#include "scope.h"
#include "variable.h"
#include "type.h"
#include <vector>
using namespace std;

//------------------------------------------------------------------------------
// Symbol struct: struct for Pascal procedures, functions, and programs.
//
// ADT Symbol struct: struct which maintains information about a procedure, 
// function, or program including its level, parent, next sibling, first child, 
// the symbols in it, its type(if it has one), and its parameters.
//------------------------------------------------------------------------------
struct ProcFunc : public Symbol, public Scope {
	Type *type;						// The type of the ProcFunc.
	vector<Variable*> params;		// The parameters of the ProcFunc.

	// Constructor
	ProcFunc(int level, Scope *parent = NULL, Scope *next = NULL, 
		Scope *child = NULL);
};

#endif