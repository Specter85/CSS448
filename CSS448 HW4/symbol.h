// File: symbol.h
// Header file for the Symbol struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
using namespace std;

//------------------------------------------------------------------------------
// Symbol struct: base struct for all pascal symbols.
//
// ADT Symbol struct: struct which maintains the most basic infomation for
// a symbol supports equality, inequality, lessthan and display operations.
// Some derived classes support displayVal and clone operations.
//------------------------------------------------------------------------------
struct Symbol {
	string name;					// The name of the symbol.
	bool isConst;
	bool valid;						// Is the Symbol valid.

	Symbol();		// Constructor.

	virtual ~Symbol() {}			// Destructor.

	// Overloaded equality operator.
	virtual bool operator==(const Symbol &rhs)const;
	// Overloaded inequality operator.
	virtual bool operator!=(const Symbol &rhs)const;
	// Overloaded less than operator.
	virtual bool operator<(const Symbol &rhs)const;

	// Function for displaying the current object.
	virtual void display(int indent)const;
	// Function for displaying the value of the current object if it has one.
	virtual void displayVal()const;

	// Function for cloning the current object if it supports it.
	virtual Symbol *clone();
};

#endif