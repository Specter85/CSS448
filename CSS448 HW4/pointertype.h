// File: pointertype.h
// Header for the PointerType struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef POINTER_TYPE_H
#define POINTER_TYPE_H
#include "type.h"

//------------------------------------------------------------------------------
// PointerType struct: struct for Pascal array types.
//
// ADT PointerType struct: struct which maintains infomation for a Pascal pointer
// type including type it is to and its name supports equality, inequality, 
// lessthan and display operations.
//------------------------------------------------------------------------------
struct PointerType : public Type {

	Type *typeTo;			// The type the PointerType it to.

	// Function for displaying the current object.
	void display(int indent)const;

};

// Struck for unfinished pointers.
struct umfPointer {
	PointerType *point;			// The unfinished Pointer.
	string type;				// The type of the unfinished pointer.
};


#endif