// File: record.cpp
// Function definitions for the RecordType struct.
// Programers Mark Zachacz, Ilya Novichenko

#include "recordtype.h"
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
// Destructor
RecordType::~RecordType() {
	// Delete all variables in the record.
	for(vector<Variable*>::iterator i = vars.begin();
		i != vars.end(); i++) {
			delete *i;
	}
}

Variable *RecordType::getMember(string name) {
	for(vector<Variable*>::iterator i = vars.begin();
		i != vars.end(); i++) {
			if((*i)->name == name) {
				return (*i);
			}
	}

	return NULL;
}

//------------------------------------------------------------------------------
// display
// Function for displaying the current object with indent indent.
void RecordType::display(int indent)const {
	// Output a number of tabs equivalent to indent.
	for(int i = 0; i < indent; i++) {
		cout << "   ";
	}

	// Output the name of the current object and all its variables.
	cout << name << endl;
	for(vector<Variable*>::const_iterator i = vars.begin(); i != vars.end();
		i++) {
			(*i)->display(indent + 1);
	}
}