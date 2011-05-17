// File: procfunc.cpp
// Function definitions for the ProcFunc struct.
// Programers Mark Zachacz, Ilya Novichenko

#include "procfunc.h"

//------------------------------------------------------------------------------
// Constructor
ProcFunc::ProcFunc(int level, Scope *parent, Scope *next, Scope *child):
Scope(level, parent, next, child) {}