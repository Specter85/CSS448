// File: codefuncs.cpp
// Function defitions for code generation functions.
// Programers Mark Zachacz, Ilya Novichenko

#include "codefuncs.h"
#include <iostream>
#include <string>
#include <list>
#include <typeinfo>
#include <set>
#include "simpletype.h"
#include "pointertype.h"
#include "recordtype.h"
#include "arraytype.h"
#include "settype.h"
#include "typeredef.h"
using namespace std;

// List for holding onto types before they are output.
extern list<Type*> typeList;

// The Scope/Indent Level
extern int gLevel;

void getArrayInfo(int &numDim, Symbol *&type, ArrayType *rhs) {
	numDim = 0;
	type = NULL;
	ArrayType *arr = rhs;
	for(;;) {
		numDim += arr->numDim;

		ArrayType *aTemp = dynamic_cast<ArrayType*>(arr->type);
		if(aTemp != NULL) {
			arr = aTemp;
		}
		else {
			type = arr->type;
			break;
		}
	}
}

// Function for comparing two pointers.
bool cmpPtrs(PComp rhs1, PComp rhs2) {
	if(!rhs1.isPointer || !rhs2.isPointer) {
		return false;
	}

	if(rhs1.type == "NULL" || rhs2.type == "NULL") {
		return true;
	}

	return rhs1.type == rhs2.type;
}

// Function for determining if a nonterminal is a pointer.
PComp isPointer(string rhs, void *sym) {
	PComp retVal;
	retVal.isPointer = false;
	retVal.type = "bad";

	Symbol *temp = static_cast<Symbol*>(sym);

	Constant<void*> *nTemp = dynamic_cast<Constant<void*>*>(temp);
	if(nTemp != NULL) {
		retVal.isPointer = true;
		retVal.type = "NULL";
	}

	PointerType *pTemp = dynamic_cast<PointerType*>(temp);
	if(pTemp != NULL) {
		retVal.isPointer = true;
		retVal.type = pTemp->typeTo->name.c_str();
	}

	return retVal;
}

// Function for determining if a nonterminal is a bool.
bool isBool(std::string rhs, void *sym) {
	if(rhs == "bool") {
		return true;
	}
	
	Symbol *temp = static_cast<Symbol*>(sym);
	if((temp != NULL) && (temp->name == "boolean")) {
		return true;
	}

	return false;
}

// Function for determining if a nonterminal is a number.
// Allows ints, reals, and bools.
bool isNumber(std::string rhs, void *sym) {
	if(rhs == "number" || rhs == "bool") {
		return true;
	}

	Symbol *temp = static_cast<Symbol*>(sym);
	if((temp != NULL) && (temp->name == "real" || temp->name == "integer" ||
		temp->name == "boolean")) {
			return true;
	}

	return false;	
}

//------------------------------------------------------------------------------
// outputFunctionHeader
// Function for outputing a C++ function header given a Pascal type
// ProcFunc and scope level. The function determines if return type of the
// generated function is void by checking if ProcFunc has a valid type.
// Parameters are generated using the ProcFuncs params vector. If a parameter 
// variable has its ref flag set, it is generated as a pass by reference 
// parameter variable. Otherwise, pass by value.
void outputFunctionHeader(ProcFunc *rhs, int level) {
	for(int i = 0; i < level; i++) {
		cout << "   ";
	}

	// Determine and output the functions return type.
	if(rhs->type == NULL) {
		cout << "void ";
	}
	else {
		cout << getFinalTypeName(rhs->type) << " ";
	}

	cout << rhs->name << "(";

	// Output the parameters in the ProcFunc params vector.
	if(!rhs->params.empty()) {
		for(int i = 0; i < (rhs->params.size() - 1); i++) {
			Variable *current = rhs->params[i];
			cout << getFinalTypeName(current->type) << " ";

			if(current->ref) {
				cout << "&";
			}

			cout << current->name << "," << endl;

			for(int j = 0; j < level + 1; j++) {
				cout << "   ";
			}
		}
		// Output the last parameter.
		Variable *current = rhs->params[rhs->params.size() - 1];
		cout << getFinalTypeName(current->type) << " ";

		if(current->ref) {
			cout << "&";
		}

		cout << current->name;
	}

	cout << ") {" << endl;

	if(rhs->type != NULL) {
		for(int i = 0; i < gLevel; i++) {
			cout << "   ";
		}

		// Make return var for the function.
		cout << getFinalTypeName(rhs->type) << " ret_" << rhs->name << ";";
	}
}

//------------------------------------------------------------------------------
// outputTypes
// Function for outputing a C++ type/typedef defintion for all valid Pascal type
// Type, in the typeList list, and a scope level. The function uses dynamic casting 
// to determine what the Pascal Type in the list is so that it can then generate 
// and output the correct C++ type/typedef decleration.
void outputTypes(int level) {
	while(!typeList.empty()) {
		Type *type = typeList.front();
		typeList.pop_front();

		if(type->valid) {
			for(int i = 0; i < level; i++) {
				cout << "   ";
			}
			
			// If the Type is of type ArrayType output a C++ array typedef defintion for it.
			ArrayType *aType = dynamic_cast<ArrayType*>(type);
			if(aType != NULL) {
				cout << "typedef " << getFinalTypeName(aType->type) << " "
					<< aType->name;

				outputArrayRanges(aType);

				cout << ";" << endl;
				continue;
			}

			// If the Type is of type SetType output a C++ set<int> typedef defintion for it.
			SetType *sType = dynamic_cast<SetType*>(type);
			if(sType != NULL) {
				cout << "typedef set<int> " << sType->name << ";" << endl;
				continue;
			}

			// If the Type is of type TypeRedef output a C++ typedef defintion for it.
			TypeRedef *reType = dynamic_cast<TypeRedef*>(type);
			if(reType != NULL) {
				cout << "typedef " << getFinalTypeName(reType->typeTo) 
					<< " " << reType->name << ";" << endl;
				continue;
			}

			// If the Type is of type PointerType output a C++ pointer typedef defintion for it.
			PointerType *pType = dynamic_cast<PointerType*>(type);
			if(pType != NULL) {
				cout << "struct " << getFinalTypeName(pType->typeTo) << ";" << endl;
				cout << "typedef " << getFinalTypeName(pType->typeTo) 
					<< " *" << pType->name << ";" << endl;
				continue;
			}

			// If the Type is of type RecordType output a C++ struct defintion for it.
			RecordType *rType = dynamic_cast<RecordType*>(type);
			if(rType != NULL) {
				cout << "struct " << rType->name << " {" << endl;
				for(vector<Variable*>::iterator i = rType->vars.begin();
					i != rType->vars.end(); i++) {
						outputVar(*i, level+1);
				}
				cout << "};" << endl;
				continue;
			}
		}
	}
}

//------------------------------------------------------------------------------
// outputConst
// Function for outputing a C++ constant defintion given a Pascal constant
// Symbol and scope level. The function uses dynamic casting to determine
// if the parameter Symbol is of type bool, int, double, or char.
void outputConst(Symbol *rhs, int level) {
	// Output the number of tabs in the scope level.
	for(int i = 0; i < gLevel; i++) {
		cout << "   ";
	}

	// If the Symbol is of type boolean output a C++ bool definition for it.
	Constant<bool> *bTemp = dynamic_cast<Constant<bool>*>(rhs);
	if(bTemp != NULL) {
		cout << "const bool " << bTemp->name << " = ";
		if(bTemp->value) {
			cout << "true;";
		}
		else {
			cout << "false;";
		}
		return;
	}

	// If the Symbol is of type integer output a C++ int defintion for it.
	Constant<int> *iTemp = dynamic_cast<Constant<int>*>(rhs);
	if(iTemp != NULL) {
		cout << "const int " << iTemp->name << " = " << iTemp->value << ";";
		return;
	}

	// If the Symbol is of type real output a C++ double defintion for it.
	Constant<double> *dTemp = dynamic_cast<Constant<double>*>(rhs);
	if(dTemp != NULL) {
		cout << "const double " << dTemp->name << " = " << dTemp->value << ";";
		return;
	}

	// If the Symbol is of type string output a C++ c-string defintion for it.
	Constant<string> *sTemp = dynamic_cast<Constant<string>*>(rhs);
	if(sTemp != NULL) {
		cout << "const char " << sTemp->name << "[] = \"" << sTemp->value << "\";";
		return;
	}
}

//------------------------------------------------------------------------------
// outputVar
// Function for outputing a C++ variable defintion given a Pascal variable
// Symbol and scope level. The function uses dynamic casting to determine
// if the parameter Variable is a pointer, an array type, a record type, or
// a single/standard variable with a type.
void outputVar(Variable *rhs, int level) {
	// Output the number of tabs in the scope level.
	for(int i = 0; i < gLevel; i++) {
		cout << "   ";
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
// Function for outputing the C++ ranges of an array given a Pascal ArrayType.
// The function uses a for loop to traverse the ranges of the ArrayType parameter 
// object and error checks them to make sure that the ranges are not reals or more
// than a single character in size. If these is an invalid array range, an error 
// message is displayed, otherwise the array ranges are simply output.
void outputArrayRanges(ArrayType *rhs) {
	typedef ArrayType::DimRange dRange;

	// Loop through all the ranges of the array.
	for(vector<dRange>::iterator i = rhs->dimRanges.begin();
		i != rhs->dimRanges.end(); i++) {
			// Make sure the start and end of the range are of the same type if not
			// output an error.
			if(typeid(*i) != typeid(*i)) {
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
// Pascal primitives to C++ primitives. The function uses if-else statements to 
// convert Pascal SimpleType into the corresponding C++ type. If it is not a 
// primitive type, it is returned as it is stored.
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
		else if(sType->name == "char") {
			return "char";
		}
	}
	// Otherwise simply return the name of rhs.
	else {
		return rhs->name;
	}
}

//------------------------------------------------------------------------------
// outputHeader
// Function for generating the sandard C++ header from a Pascal program. This 
// outputs the #include statements and the global using namespace std statement.
void outputHeader() {
	cout << "#include <iostream>" << endl
		<< "#include <set>" << endl
		<< "using namespace std;" << endl << endl;
}