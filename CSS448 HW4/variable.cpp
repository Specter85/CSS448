// File: variable.cpp
// Function definitions for the Variable struct.
// Programers Mark Zachacz, Ilya Novichenko

#include "variable.h"
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
// Constuctor
// Default Constuctor. Sets "ref" to false by default.
Variable::Variable():ref(false) {}

//------------------------------------------------------------------------------
// Destructor
Variable::~Variable() {
}

//------------------------------------------------------------------------------
// display
// Function called to display a variable of a given scope. Signifies if the
// variable is a "var" (parameter passed by reference), displays the variable
// name and type. If the variable type is invalid, and error is displayed.
void Variable::display(int indent)const {
	// Output a number of tabs equivalent to indent.
	for(int i = 0; i < indent; i++) {
		cout << "   ";
	}

	// If the variable is not a refrence don't output var.
	if(ref == false) {
		// If type exists and is valid output the variable's name
		// and the name of the type or if the type has no name display
		// the type.
		if(type != NULL && type->valid) {
			if(type->name == "") {
				cout << name << " ";
				type->display(0);
			}
			else {
				cout << name << " " << type->name << endl;
			}	
		}
		// Otherwise output the variable's name and an error.
		else {
			cout << name << " ***ERROR: invalid type" << endl;
		}
	}
	// If the variable is a refrence output var.
	else {
		// If type exists and is valid output the variable's name
		// and the name of the type or if the type has no name display
		// the type.
		if(type != NULL && type->valid) {
			if(type->name == "") {
				cout << "var " << name << " ";
				type->display(0);
			}
			else {
				cout << "var " << name << " " << type->name << endl;
			}
		}
		// Otherwise output the variable's name and an error.
		else {
			cout << "var " << name << " ***ERROR: invalid type" << endl;
		}
	}
}