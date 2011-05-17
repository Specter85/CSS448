// File: simpletype.cpp
// Function definitions for the SimpleType struct.
// Programers Mark Zachacz, Ilya Novichenko

#include "simpletype.h"

//------------------------------------------------------------------------------
// Constructor
SimpleType::SimpleType() {
}

//------------------------------------------------------------------------------
// Constructor
// Constructor which set the name of the SimpleType to rhs.
SimpleType::SimpleType(string rhs) {
	name.assign(rhs);
}