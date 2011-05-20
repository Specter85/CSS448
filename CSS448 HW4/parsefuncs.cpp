// File: parsefuncs.cpp
// Function defintions for functions called by the parser.
// Programers Mark Zachacz, Ilya Novichenko

#include "parsefuncs.h"
#include "procfunc.h"
#include "constant.h"
#include "variable.h"
#include "symboltable.h"
#include "recordtype.h"
#include "pointertype.h"
#include "settype.h"
#include "type.h"
#include "codefuncs.h"
#include <list>
#include <string>
#include <stack>
#include <cmath>

using namespace std;

// The symbol table.
extern SymbolTable table;

// Stacks for idents and variables that have not been inserted yet.
extern stack<string> identStack;
extern list<Variable*> variableStack;

// Pointer to Symbols for the start and end of a set.
extern Symbol *setStart;
extern Symbol *setEnd;

// Pointer to the last type that was found.
extern Type *currentType;

// Strings for names that need to be held onto.
extern string symbolName;
extern string recordName;
extern string scopeName;

// SimpleTypes for stirng, Program params, and nil.
extern SimpleType pString;
extern SimpleType nil;
extern SimpleType pParam;

// Stack of unfinished pointers.
extern list<umfPointer> pointerStack;

//------------------------------------------------------------------------------
// addProgramVars
// Function for processing program variables. Takes variable names added to the
// identStack and creates variables from them which are then added to the
// variable stack.
void addProgramVars() {
	// Loop until the ident stack is empty.
	while(!identStack.empty()) { 
		// Get the name of the next program variable.
		string name = identStack.top();
		identStack.pop();

		// Create the new variable and add it to the variable stack.
		Variable *var = new Variable;
		var->name = name;
		var->type = &pParam;
		variableStack.push_back(var);
	}
}

//------------------------------------------------------------------------------
// addScope
// Function for creating a scope. Also adds the prameters of the scope to the
// scope.
void addScope(bool func) {
	// Create and enter the new scope.
	ProcFunc *temp = new ProcFunc(0);
	temp->name = scopeName;
	temp->type = NULL;
	table.enterScope(temp);

	// If the scope belongs to a functtion get its type.
	if(func == true) {
		Type *tVal = dynamic_cast<Type*>(table.lookUp(value));
		if(tVal == NULL) {
			tVal = dynamic_cast<Type*>(sit.getSymbol(value));
			// If type is not found after looking in the symbol table and sit output
			// an error and  continue.
			if(tVal == NULL) {
				cout << "***ERROR: function type " << value << " does not exist" << endl;
			}
			// Assign the type from the sit.
			else {
				temp->type = tVal;
			}
		}
		// Assign the type from the symbol table.
		else {
			temp->type = tVal;
		}
	}

	// Loop to add all scope params to the scope.
	while(!variableStack.empty()) {
		// Get the next param.
		Variable *sTemp = variableStack.front();
		variableStack.pop_front();

		// If the param is the same as another param output and error, delete it
		// and continue.
		Symbol *test = table.lookUpCS(sTemp->name);
		if(test != NULL) {
			cout << "***ERROR: " << sTemp->name << " already in symbol table" << endl;
			delete sTemp;
			continue;
		}

		// Add the param to the scope's params and scope.
		temp->params.push_back(sTemp);
		table.insert(sTemp);
	}
}

//------------------------------------------------------------------------------
// addRecord
// Function for creatig a recordtype. Emptys the variable stack putting its
// values into the new stack and sets currentType to the new record type.
void addRecord() {
	// Make the new record type.
	RecordType *temp = new RecordType;

	// Loop till variableStack is empty.
	while(!variableStack.empty()) {
		// Get the next variable.
		Variable *tVar = variableStack.front();
		variableStack.pop_front();

		// Determine if the next variable has the same name as another
		// member of the record.
		bool test = true;
		for(vector<Variable*>::iterator i = temp->vars.begin();
			i != temp->vars.end(); i++) {
				if(!tVar->name.compare((*i)->name)) {
					test = false;
					break;
				}
		}

		// If the varible is not a repeat add it.
		if(test) {
			temp->vars.push_back(tVar);
		}
		// If the variable is a repeat output an error and delete it.
		else {
			cout << "***ERROR: " << tVar->name << " already in record" << endl;
			delete tVar;
		}
	}

	// Set currentType to the new record.
	currentType = temp;
}

//------------------------------------------------------------------------------
// processUmfPointers
// Function for processing unfinished pointers. Gives the pointers their type or
// outputs an error if their type does not exist.
void processUmfPointers() {
	// Loop till pointerStack is empty.
	while(!pointerStack.empty()) {
		// Get the next pointer.
		umfPointer temp = pointerStack.front();
		pointerStack.pop_front();

		// Get the pointers type.
		RecordType *type = dynamic_cast<RecordType*>(table.lookUp(temp.type));
		if(type == NULL) {
			type = dynamic_cast<RecordType*>(sit.getSymbol(temp.type));
			// If the type does not exist output an error and contiue.
			if(type == NULL) {
				temp.point->valid = false;
				cout << "***ERROR: type " << temp.type << " is invalid." << endl;
				continue;
			}
		}

		// If the type exists set it.
		temp.point->typeTo = type;
	}

	// Invalidate any bad pointer types.
	table.cullBadPointers();

	// Invalidate any types that contained bad pointer types.
	bool clean;
	do {
		clean = true;
		table.cullBadTypes(clean);

		// Invalidate any pointers to bad types.
		table.cullBadPointers();
	} while(!clean);
}

//------------------------------------------------------------------------------
// makePointerType
// Function for making a pointer type. currentType will be set to the new type
// If the pointer's type does not exist currentType will still be set but the
// pointer will also be added to pointerStack for later processing.
void makePointerType() {
	// Make the new pointer type.
	PointerType *temp = new PointerType;
	temp->typeTo = NULL;

	// Get the pointers type.
	Type *type = dynamic_cast<Type*>(table.lookUp(value));
	if(type == NULL) {
		type = dynamic_cast<Type*>(sit.getSymbol(value));
		// If the type does nto exist set currentType and add the pointerType
		// to pointerStack. Then return.
		if(type == NULL) {
			umfPointer pTemp;
			pTemp.point = temp;
			pTemp.type = value;
			pointerStack.push_back(pTemp);
			currentType = temp;
			return;
		}
	}

	// If the type does exist set it. Then set current type.
	temp->typeTo = type;
	currentType = temp;
}

//------------------------------------------------------------------------------
// addRecordVars
// Function for processing the variables in a record. Takes in the variable names
// from identStack, makes them into variables, and puts them onto variableStack.
void addRecordVars() {
	// Get the type of the variable.
	Type *tVal = dynamic_cast<Type*>(table.lookUp(value));
	if(tVal == NULL) {
		tVal = dynamic_cast<Type*>(sit.getSymbol(value));
		if(tVal == NULL) {
			// If the type does not exist output an error, clear the ident
			// stack and return.
			cout << "***ERROR: " << value << " type does not exist" << endl;
			while(!identStack.empty()) {
				identStack.pop();
			}
			return;
		}
	}

	// If the type does exist add all the variables to the variable stack.
	while(!identStack.empty()) {
		// Get the name of the variable.
		string name = identStack.top();
		identStack.pop();

		// Make the new variable and add it to the variable stack.
		Variable *var = new Variable;
		var->name = name;
		var->type = tVal;
		variableStack.push_back(var);
	}
}

//------------------------------------------------------------------------------
// addVars
// Function for adding variables to the symbol table. Takes idents off
// identStack converts them to variables and puts them in the symbol table.
// If currentType is NULL empties identStack and returns.
void addVars() {
	// Get the type for the variables.
	Type *tVal = NULL;
	// If currentType is NULL output an error, empty identStack, and return.
	if(currentType == NULL) {
		cout << "***ERROR: variables not added because type does not exist" << endl;
		while(!identStack.empty()) {
			identStack.pop();
		}
		return;
	}
	// Otherwise set tVal to currentType.
	else {
		tVal = currentType;
		currentType = NULL;
	}

	// Loop unitl identStack is empty.
	while(!identStack.empty()) {
		// Get the next ident Name from identStack.
		string name = identStack.top();
		identStack.pop();

		// Determine a Symbol with name name already exists if so continue
		// without making a new variable.
		Symbol *temp = table.lookUpCS(name);
		if(temp != NULL) {
			cout << "***ERROR: " << name << " already in symbol table" << endl;
			continue;
		}

		// Make the new variable and add it to the symbol table.
		Variable *var = new Variable;
		var->name = name;
		var->type = tVal;
		table.insert(var);
		outputVar(var, table.getScopeLevel());
	}
}

//------------------------------------------------------------------------------
// addFuncVars
// Function for adding function params to variableStack. This removes any
// pending ident names in identStack. If ref is true the variables will be
// pass by refrence otherwise they will be pass by value.
void addFuncVars(bool ref) {
	// Get the type for the variables.
	Type *tVal = dynamic_cast<Type*>(table.lookUp(value));
	if(tVal == NULL) {
		tVal = dynamic_cast<Type*>(sit.getSymbol(value));
		// If the type does not exist output an error, empty identStack, and
		// return.
		if(tVal == NULL) {
			cout << "***ERROR: " << value << " type does not exist" << endl;
			while(!identStack.empty()) {
				identStack.pop();
			}
			return;
		}
	}

	// If the type does exist loop till identStack is empty.
	while(!identStack.empty()) {
		// Get the next ident name.
		string name = identStack.top();
		identStack.pop();

		// Make the new variable and add it to variableStack.
		Variable *var = new Variable;
		var->name = name;
		var->type = tVal;
		var->ref = ref;
		variableStack.push_back(var);
	}
}

//------------------------------------------------------------------------------
// addSetType
// Function for creating a set type. currentType will be set to the new type
// and setStart and setEnd will be set to NULL. If setStart or setEnd is NULL
// an error will be returned, setStart and setEnd will be deleted, and
// current type will be unchanged.
void addSetType() {
	// If setStart or setEnd is NULL output an error, delete them, and return.
	if(setStart == NULL || setEnd == NULL) {
		cout << "***ERROR: bad range" << endl;
		delete setStart;
		delete setEnd;
		return;
	}

	// Create the new SetType and set currentType to it.
	SetType *temp = new SetType;
	temp->start = setStart;
	temp->end = setEnd;
	currentType = temp;

	// Set setStart and setEnd to NULL.
	setStart = NULL;
	setEnd = NULL;
}

//------------------------------------------------------------------------------
// getString
// Function for creating a string constant.
Symbol *getString() {
	Constant<string> *temp = new Constant<string>;
	temp->value = value;
	temp->type = &pString;
	return temp;
}

//------------------------------------------------------------------------------
// getTrue
// Function for creating a bool true constant.
Symbol *getTrue() {
	Constant<bool> *temp = new Constant<bool>; 
	temp->value = true;
    temp->type = (SimpleType*)sit.getSymbol(string("boolean"));
	return temp;
}

//------------------------------------------------------------------------------
// getFalse
// Function for creating a bool false constant.
Symbol *getFalse() {
	Constant<bool> *temp = new Constant<bool>; 
	temp->value = false;
    temp->type = (SimpleType*)sit.getSymbol(string("boolean"));
	return temp;
}

//------------------------------------------------------------------------------
// getNil
// Function for creating a nil pointer constant.
Symbol *getNil() {
	Constant<void*> *temp = new Constant<void*>;
	temp->value = NULL;
	temp->type = &nil;
	return temp;
}

//------------------------------------------------------------------------------
// getNumber
// Function for creating a number constant. Returns either a real of integer
// constant.
Symbol *getNumber() {
	// See if there is a decimal point.
	int test = value.find('.', 0);
	
	// If there is no decimal point make and return an integer constant.
	if(test == value.npos) {
		Constant<int> *temp = new Constant<int>;
		temp->type = (SimpleType*)sit.getSymbol(string("integer"));
		temp->value = atoi(value.c_str());
		return temp;
	}
	// Otherwise make and return a real constant.
	else {
		// See if there is an "e" representing an exponent.
		test = value.find('e', 0);

		// If there is no exponent just make and return the real constant.
		if(test == value.npos) {
			Constant<double> *temp = new Constant<double>;
			temp->value = atof(value.c_str());
			return temp;
		}
		// If there is an exponent get it, apply it, create the real
		// constant, and return the real constant.
		else {
			// Get the base number and its constant.
			string sNumber = value.substr(0, test - 1);
			string sExponent = value.substr(test + 1);

			// Get double for the number and the integer for the exponent.
			double number = atof(sNumber.c_str());
			int exponent = atoi(sExponent.c_str());

			// Apply the exponent.
			number = pow(number, exponent);

			// Create and return the constant.
			Constant<double> *temp = new Constant<double>;
			temp->type = (SimpleType*)sit.getSymbol(string("real"));
			temp->value = number;
			return temp;
		}
	}
}