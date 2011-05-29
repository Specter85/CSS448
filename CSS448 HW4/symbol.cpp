// File: symbol.cpp
// Funciton definitions for the Symbol struct.
// Programers Mark Zachacz, Ilya Novichenko

#include "symbol.h"

//------------------------------------------------------------------------------
// Constructor
Symbol::Symbol():valid(true), isConst(false) {}

//------------------------------------------------------------------------------
// ==
// Overloaded equality operator.
bool Symbol::operator==(const Symbol &rhs)const {
	return name == rhs.name;
}

//------------------------------------------------------------------------------
// !=
// Overloaded inequality operator.
bool Symbol::operator!=(const Symbol &rhs)const {
	return !(name == rhs.name);
}

//------------------------------------------------------------------------------
// <
// Overloaded less than operator.
bool Symbol::operator<(const Symbol &rhs)const {
	return name < rhs.name;
}

//------------------------------------------------------------------------------
// display
// Function for displaying the current object.
void Symbol::display(int indent)const {
}

//------------------------------------------------------------------------------
// displayVal
// Function for displaying the value of the current object if it has one. I the
// current object is of a derived type that does not have a type this function
// does nothing.
void Symbol::displayVal()const {
}

//------------------------------------------------------------------------------
// clone
// Function which makes an exact copy of the current object if it supports it. If
// the current object is of a type that does not support cloning this function
// returns NULL.
Symbol *Symbol::clone() {
	return NULL;
}