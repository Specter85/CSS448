// File: arraytype.cpp
// Function definitions for the ArrayType struct.
// Programers Mark Zachacz, Ilya Novichenko

#include "arraytype.h"
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
// ~ArrayType
// Default Destructor. Clear/delete all of the ArrayType variables dimension
// ranges (start and end).
ArrayType::~ArrayType() {
	// Delete all the constants for range start and end.
	for(vector<DimRange>::iterator i = dimRanges.begin();
		i != dimRanges.end(); i++) {
			delete i->start;
			delete i->end;
	}
}

//------------------------------------------------------------------------------
// display
// Function called to display an ArrayType variable of a given scope. Displays
// the ArrayType variable name, dimensions with their ranges, and type. If the
// ArrayType variables type is invalid, displays an error message.
void ArrayType::display(int indent)const {
	// Outout a number of indents coresponding to the current scope.
	for(int i = 0; i < indent; i++) {
		cout << "   ";
	}

	// Output the name of the array and all but the last dimension's range
	// each folloed by a coma.
	cout << name << " ";
	for(int i = 0; i < dimRanges.size() - 1; i++) {
		dimRanges[i].start->displayVal();
		cout << "..";
		dimRanges[i].end->displayVal();
		cout << ", ";
	}
	// Output the final dimensions range without a coma.
	int end = dimRanges.size() - 1;
	dimRanges[end].start->displayVal();
	cout << "..";
	dimRanges[end].end->displayVal();
	cout << " ";

	// If the array's type exists and is valid output it.
	if(type != NULL && type->valid) {
		if(type->name == "") {
			type->display(0);
		}
		else {
			cout << type->name << endl;
		}
	}
	// Otherwise output an error.
	else {
		cout << "***ERROR: invalid type" << endl;
	}
}