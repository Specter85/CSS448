// File: recordtype.h
// Header for the RecordType struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef RECORD_TYPE_H
#define RECORD_TYPE_H
#include "symbol.h"
#include "type.h"
#include "variable.h"
#include "stdidents.h"
#include <vector>

//------------------------------------------------------------------------------
// RecordType struct: struct for Pascal record types.
//
// ADT RecordType struct: struct which maintains infomation for a Pascal record
// type including name, and the variables in the record type supports equality, 
// inequality, lessthan and display operations.
//------------------------------------------------------------------------------
struct RecordType : public Type {
	vector<Variable*> vars;				// The variables in the record type.

	~RecordType();						// Destructor

	// Function for displaying the current object.
	void display(int indent)const;
};

#endif