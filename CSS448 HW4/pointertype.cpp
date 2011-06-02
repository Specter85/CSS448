// File: pointertype
// Function definitions for the PointerType struct.
// Programers Mark Zachacz, Ilya Novichenko

#include "pointertype.h"
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
// display
// Function for displaying the current object with indent indent.
void PointerType::display(int indent)const {
	// Make sure the pointer type is valid.
	if(valid) {
		// Output a number of tabs equivalent to indent.
		for(int i = 0; i < indent; i++) {
			cout << "   ";
		}

		// If typeTo exists and is valid output the pointer type's name
		// and the name of the type or if the type has no name display
		// the type.
		if(typeTo != NULL && typeTo->valid) {
			if(typeTo->name == "") {
				cout << name << " ^ ";
				typeTo->display(0);
			}
			else {
				cout << name << " ^ " << typeTo->name << endl;
			}
		}
		// Otherwise output the pointer type's name and an error.
		else {
			cout << name << " ^ ***ERROR: invalid type" << endl;
		}
	}
}