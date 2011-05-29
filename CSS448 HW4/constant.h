// File: constant.h
// Header for the Constant struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef CONSTANT_H
#define CONSTANT_H
#include "symbol.h"
#include "simpletype.h"
#include "stdidents.h"
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
// Constant struct: template struct for all Pascal constants.
//
// ADT Symbol struct: struct which maintains  infomation for Pascal constants
// including their type, name, and value supports equality, inequality, lessthan, 
// display operations, displayVal and clone operations.
//------------------------------------------------------------------------------
template<class TYPE>
struct Constant : public Symbol {
	SimpleType *type;			// The type of the constant.
	TYPE value;					// The value of the constant.

	Constant();					// Constructor

	// Template function called to display a Constant of a given scope.
	void display(int indent)const;

	// Template function called to display a Constant objects value.
	void displayVal()const;

	// Template function called to clone/duplicate a Constant object.
	Symbol *clone();
};

//------------------------------------------------------------------------------
// Constructor
template<class TYPE>
Constant<TYPE>::Constant() {
	isConst = true;
}

//------------------------------------------------------------------------------
// display
// Template function called to display a Constant of a given scope. Displays
// the Constants name and value.
template <class TYPE>
void Constant<TYPE>::display(int indent)const {
	// Outout a number of indents coresponding to the current scope.
	for(int i = 0; i < indent; i++) {
		cout << "\t";
	}

	// Output the name of the current object and its value.
	cout << name << " " << value << endl;
}

//------------------------------------------------------------------------------
// displayVal
// Template function called to display a Constant objects value.
template <class TYPE>
void Constant<TYPE>::displayVal()const {
	cout << value;
}

//------------------------------------------------------------------------------
// clone
// Template function called to clone/duplicate a Constant object and return a
// pointer to it as a pointer to a Symbol object.
template <class TYPE>
Symbol *Constant<TYPE>::clone() {
	// Create the new Constant object and set all its fields to that of the
	// current object.
	Constant<TYPE> *temp = new Constant<TYPE>;
	temp->name.assign(name);
	temp->type = type;
	temp->value = value;

	// Return the new Constant object.
	return temp;
}

#endif