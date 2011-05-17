// File: settype.h
// Header for the SetType struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef SET_TYPE_H
#define SET_TYPE_H
#include "type.h"
#include "symbol.h"

//------------------------------------------------------------------------------
// SetType struct: struct for Pascal set types.
//
// ADT SetType struct: struct which maintains infomation for a Pascal set
// type including name, set start, and set end supports equality, inequality, 
// lessthan and display operations.
//------------------------------------------------------------------------------
struct SetType : public Type {
	Symbol *start;				// The set's start.
	Symbol *end;				// The set's end.

	~SetType();					// Destructor

	// Function for displaying the current object.
	void display(int indent)const;

};

#endif