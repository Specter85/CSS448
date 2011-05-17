// File: stdidents.h
// Header for the StdIdents class.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef STD_IDENTS_H
#define STD_IDENTS_H

#include "symbol.h"
using namespace std;

//------------------------------------------------------------------------------
// StdIdents class: A class which stores Pascal's stdIdents and allows one to
// determine if a string is a Pascal stdIdent.
//
// ADT StdIdent class: A class which maintains the stdIdents of a language and
// supports determining if a string is a stdIdent and returning the token
// associated with a stdIdent given its string.
//------------------------------------------------------------------------------
class StdIdents {
public:

	// Struct for a StdIdent.
	struct StdIdent {
		Symbol *sym;			// The string representation of a StdIdent.

		~StdIdent() {}			// Destructor for strings.
	};

private:

	// Const for the number of stdIdents in the class.
	static const int NUM_STD_IDENTS = 34;
	// Const for invalid strings.
	static const int INVALID_STD_IDENT = -1;
	// The list of stdIdents in the class.
	StdIdent stdIdentList[NUM_STD_IDENTS];

public:

	StdIdents();					// Constructor
	~StdIdents();				    // Destructor

	// Function which determines if a string is a Pascal StdIdent.
	bool isStdIdent(const string)const;
	// Function which retrieves/returns a pointer to a StdIdent Symbol.
	Symbol *getSymbol(const string rhs)const;

};

#endif