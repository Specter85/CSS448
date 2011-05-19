// File: codefuncs.cpp
// Function defitions for code generation functions.
// Programers Mark Zachacz, Ilya Novichenko

#include "codefuncs.h"
#include <iostream>
#include <string>
#include "simpletype.h"
#include "pointertype.h"
#include "recordtype.h"
#include "arraytype.h"
using namespace std;

//------------------------------------------------------------------------------
// outputConst
// Function for outputing a C++ constant defintion given a Pascal constant
// Symbol and scope level.
void outputConst(Symbol *rhs, int level) {
	// Output the number of tabs in the scope level.
	for(int i = 0; i < level; i++) {
		cout << "\t";
	}

	// If the Symbol is of type boolean output a C++ bool definition for it.
	Constant<bool> *bTemp = dynamic_cast<Constant<bool>*>(rhs);
	if(bTemp != NULL) {
		cout << "const bool " << bTemp->name << " = ";
		if(bTemp->value) {
			cout << "true;" << endl;
		}
		else {
			cout << "false;" << endl;
		}
		return;
	}

	// If the Symbol is of type integer output a C++ int defintion for it.
	Constant<int> *iTemp = dynamic_cast<Constant<int>*>(rhs);
	if(iTemp != NULL) {
		cout << "const int " << iTemp->name << " = " << iTemp->value << ";" << endl;
		return;
	}

	// If the Symbol is of type real output a C++ double defintion for it.
	Constant<double> *dTemp = dynamic_cast<Constant<double>*>(rhs);
	if(dTemp != NULL) {
		cout << "const double " << dTemp->name << " = " << dTemp->value << ";" << endl;
		return;
	}

	// If the Symbol is of type string output a C++ c-string defintion for it.
	Constant<string> *sTemp = dynamic_cast<Constant<string>*>(rhs);
	if(sTemp != NULL) {
		cout << "const char " << sTemp->name << "[] = \"" << sTemp->value << "\";" << endl;
		return;
	}
}

//------------------------------------------------------------------------------
// outputVar
// Function for outputing a C++ variable defintion given a Pascal variable
// Symbol and scople level.
void outputVar(Variable *rhs, int level) {
	// Output the number of tabs in the scope level.
	for(int i = 0; i < level; i++) {
		cout << "\t";
	}

	// Get the name of the final C++ type.
	string type = getFinalTypeName(rhs->type);
	// If the type has no name output the definition for the type here.
	if(type == "") {
		// Check if the type is a pointer type.
		PointerType *pTemp = dynamic_cast<PointerType*>(rhs->type);
		if(pTemp != NULL) {
			cout << getFinalTypeName(pTemp->typeTo) << " *" << rhs->name << ";" << endl;
		}

		// Check if the type is an array type.
		ArrayType *aTemp = dynamic_cast<ArrayType*>(rhs->type);
		if(aTemp != NULL) {
			cout << getFinalTypeName(aTemp->type) << " " << rhs->name;
			outputArrayRanges(aTemp);
			cout << ";" << endl;
		}

		// Check if the type is a record type.
		RecordType *rTemp = dynamic_cast<RecordType*>(rhs->type);
		if(rTemp != NULL) {
			cout << "struct {" << endl;
			for(vector<Variable*>::iterator i = rTemp->vars.begin();
				i != rTemp->vars.end(); i++) {
					outputVar(*i, level+1);
			}
			cout << "} " << rhs->name << ";" << endl;
		}
	}
	// If the type of the variable has a name simply output the variable
	// defintion.
	else {
		cout << type << " " << rhs->name << ";" << endl;
	}
}

//------------------------------------------------------------------------------
// outputArrayRanges
// Function for outputing the C++ ranges of a an array given a Pascal ArrayType.
void outputArrayRanges(ArrayType *rhs) {
	typedef ArrayType::DimRange dRange;

	// Loop through all the ranges of the array.
	for(vector<dRange>::iterator i = rhs->dimRanges.begin();
		i != rhs->dimRanges.end(); i++) {
			// Make sure the start and end of the range are of the same type if not
			// output an error.
			if(i->end != i->start) {
				cout << 
					"***ERROR: the start and end of an array range must be the same type"
					<< endl;
				return;
			}

			// If the current range is defined by a real output an error since this
			// is not allowed.
			Constant<double> *dTemp = dynamic_cast<Constant<double>*>(i->end);
			if(dTemp != NULL) {
				cout << "***ERROR: array ranges cannot be defined by reals" << endl;
				return;
			}

			// If the current range is defined by a string make sure it has only one char
			// if not output an error since this is not allowed.
			Constant<string> *sTemp = dynamic_cast<Constant<string>*>(i->end);
			if(sTemp != NULL) {
				if(sTemp->value.size() == 1) {
					cout << "['" << sTemp->value[0] << "']";
					continue;
				}
				else {
					cout << "***ERROR: array ranges cannot be defined by strings" << endl;
					return;
				}
			}

			// Otherwise just output the array range.
			cout << "[";
			i->end->displayVal();
			cout << "]";
	}
}

//------------------------------------------------------------------------------
// getFinalTypeName
// Function for getting the final C++ type name from a Pascal type. This converts
// Pascal primitives to C++ primitives.
string getFinalTypeName(Type *rhs) {
	// If rhs is NULL output an error.
	if(rhs == NULL) {
		return "***ERROR: bad type";
	}

	// If rhs is a SimpleType return the proper C++ primitive.
	SimpleType *sType = dynamic_cast<SimpleType*>(rhs);
	if(sType != NULL) {
		if(sType->name == "integer") {
			return "int";
		}
		else if(sType->name == "real") {
			return "double";
		}
		else if(sType->name == "boolean") {
			return "bool";
		}
	}
	// Otherwise simply return the name of rhs.
	else {
		return rhs->name;
	}
}