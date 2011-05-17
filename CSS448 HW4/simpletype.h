// File: simpletype.h
// Header for the SimpleType struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef SIMPLE_TYPE_H
#define SIMPLE_TYPE_H
#include "type.h"
#include <string>
using namespace std;

//------------------------------------------------------------------------------
// SimpleType struct: struct for Pascal built in types.
//
// ADT SimpleType struct: struct which maintains infomation for a Pascal built in
// type including name supports equality, inequality, lessthan and display 
// operations.
//------------------------------------------------------------------------------
struct SimpleType : public Type {
	// Constructors
	SimpleType();
	SimpleType(string rhs);
};

#endif