// File: codefuncs.h
// Header for for function prototypes of code generation functions.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef CODE_FUNCS_H
#define CODE_FUNCS_H
#include "constant.h"
#include "type.h"
#include "variable.h"
#include "arraytype.h"

// Function for outputing a C++ consant definition.
void outputConst(Symbol *rhs, int level);

// Function for outputing a C++ variable difinition.
void outputVar(Variable *rhs, int level);

// Function for ooutputing the ranges of a C++ array.
void outputArrayRanges(ArrayType *rhs);

// Function for getting the final C++ type name of a Pascal Type.
string getFinalTypeName(Type *rhs);

#endif