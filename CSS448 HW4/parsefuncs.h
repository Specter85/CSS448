// File: parsefuncs.h
// Header file for the prototypes of functions called by the 
// parser.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef PARSE_FUNCS_H
#define PARSE_FUNCS_H
#include "symbol.h"
#include "yylex.h"

// Function for processing program variables.
void addProgramVars();

// Function for creating a scope.
void addScope(bool func = false);

// Function for creatig a recordtype.
void addRecord();

// Function for processing the variables in a record.
void addRecordVars();

// Function for processing unfinished pointers.
void processUmfPointers();

// Function for makeing a pointer type.
void makePointerType();

// Function for adding variables to the symbol table.
void addVars();

// Function for adding function params to variableStack.
void addFuncVars(bool ref);

// Function for creating a set type.
void addSetType();

// Function for creating a string constant.
Symbol *getString();

// Function for creating a bool true constant.
Symbol *getTrue();

// Function for creating a bool false constant.
Symbol *getFalse();

// Function for creating a nil pointer constant.
Symbol *getNil();

// Function for creating a number constant.
Symbol *getNumber();

#endif