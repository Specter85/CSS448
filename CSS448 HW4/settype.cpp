// File: settype.cpp
// Header for the SetType struct.
// Programers Mark Zachacz, Ilya Novichenko

#include "settype.h"
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
// Destructor
SetType::~SetType() {
	delete start;
	delete end;
}

//------------------------------------------------------------------------------
// display
// Function for displaying the current object with indent indent.
void SetType::display(int indent)const {
	// Output a number of tabs equivalent to indent.
	for(int i = 0; i < indent; i++) {
		cout << "   ";
	}

	// Display the name and range of the set type.
	cout << name << " ";
	start->displayVal();
	cout << "..";
	end->displayVal(); 
	cout << endl;
}