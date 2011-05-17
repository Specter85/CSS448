// File: type.h
// Header for the Type struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef TYPE_H
#define TYPE_H
#include "symbol.h"

//------------------------------------------------------------------------------
// Type struct: base struct for all pascal types. Derives from Symbol. Does
// not add any functionality to Symbol simply exists to seperate types from
// other symbols.
//
// ADT Type struct: struct which maintains the most basic infomation for
// a type supports equality, inequality, lessthan and display operations.
//------------------------------------------------------------------------------
struct Type : public Symbol {
};

#endif