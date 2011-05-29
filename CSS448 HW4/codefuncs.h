// File: codefuncs.h
// Header for for function prototypes of code generation functions.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef CODE_FUNCS_H
#define CODE_FUNCS_H
#include <string>
#include "constant.h"
#include "procfunc.h"
#include "type.h"
#include "variable.h"
#include "arraytype.h"

// Function for determining if a nonterminal is a number.
// Allows ints, reals, and bools.
bool isNumber(std::string rhs, void *sym);

// Function for determining if a nonterminal is a bool.
bool isBool(std::string rhs, void *sym);

// Function for outputing the header of a function.
void outputFunctionHeader(ProcFunc *rhs, int level);

// Function for outputing all valid types in typeList.
void outputTypes(int level);

// Function for outputing a C++ constant definition.
void outputConst(Symbol *rhs, int level);

// Function for outputing a C++ variable difinition.
void outputVar(Variable *rhs, int level);

// Function for ooutputing the ranges of a C++ array.
void outputArrayRanges(ArrayType *rhs);

// Function for getting the final C++ type name of a Pascal Type.
string getFinalTypeName(Type *rhs);

// Function for outputing C++ header.
void outputHeader();

#endif