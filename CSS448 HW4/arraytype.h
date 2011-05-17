// File: arraytype.h
// Header for the ArrayType struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef ARRAY_TYPE_H
#define ARRAY_TYPE_H
#include "symbol.h"
#include "type.h"
#include "stdidents.h"
#include <vector>
using namespace std;

//------------------------------------------------------------------------------
// ArrayType struct: struct for Pascal array types.
//
// ADT ArrayType struct: struct which maintains infomation for a Pascal array
// type including type, dimensions, and dimension ranges supports equality, 
// inequality, lessthan and display operations.
//------------------------------------------------------------------------------
struct ArrayType : public Type {
	// Structure to hold ranges of a dimension of the ArrayType.
	struct DimRange {
		Symbol *start;			// Start of a dimension's range.
		Symbol *end;			// End of a dimension's range.
	};

	Type *type;				// The type for the array.
	int numDim;				// The number of dimensions of the ArrayType

	// Vector of ArrayType DimRanges.
	vector<DimRange> dimRanges;

	// Destructor
	~ArrayType();

	// Function called to display an ArrayType variable of a given scope.
	void display(int indent)const;

};

#endif