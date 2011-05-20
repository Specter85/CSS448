// File: typeredef.h
// Header for the TypeRedef struct.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef TYPE_REDEF_H
#define TYPE_REDEF_H
#include "type.h"

struct TypeRedef : public Type {
	Type *typeTo;
};

#endif