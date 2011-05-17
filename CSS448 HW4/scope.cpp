// File: scope.cpp
// Function definitions for the Scope struct.
// Programers Mark Zachacz, Ilya Novichenko

#include "scope.h"

//------------------------------------------------------------------------------
// Constructor
Scope::Scope(int level, Scope *parent, Scope *next, Scope *child) {
	this->level = level;
	this->parent = parent;
	this->next = next;
	this->child = child;
}

//------------------------------------------------------------------------------
// Destructor
Scope::~Scope() {
}