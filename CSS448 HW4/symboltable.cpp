// File: symboltable.cpp
// Function defintions for the SymbolTable class.
// Programers Mark Zachacz, Ilya Novichenko

#include "symboltable.h"
#include "procfunc.h"

// Object which stores the standard identifiers for Pascal.
StdIdents sit;

//------------------------------------------------------------------------------
// Constructor
SymbolTable::SymbolTable() {
	root = NULL;
	currentScope = NULL;
}

//------------------------------------------------------------------------------
// Destructor
SymbolTable::~SymbolTable() {
	clear(root);
}

//------------------------------------------------------------------------------
// clear
// Recursive function for destroying a SymbolTable.
void SymbolTable::clear(Scope *rhs) {
	// Make sure rhs is not NULL.
	if(rhs != NULL) {
		// Get a pointer to the first child of rhs.
		Scope *current = rhs->child;
		// Loop through and call clear for all rhs's children.
		while(current != NULL) {
			Scope *cNext = current->next;
			clear(current);
			current = cNext;
		}
		// Destroy rhs.
		delete rhs;
	}
}

//------------------------------------------------------------------------------
// enterScope
// Function for entering a scope rhs. If rhs is NULL this function does
// nothing.
void SymbolTable::enterScope(Scope *rhs) {
	// Make sure rhs is not NULL.
	if(rhs == NULL) {
		return;
	}
	// If root is NULL set rhs's level to 0 and set root to rhs. then make
	// rhs the current scope.
	if(root == NULL) {
		rhs->level = 0;
		root = rhs;
		currentScope = rhs;
	}
	// Otherwise set rhs's level to currentScope's level plus one. Insert rhs
	// into currentScope's list of children and make rhs the current scope.
	else {
		rhs->level = currentScope->level + 1;
		rhs->next = currentScope->child;
		currentScope->child = rhs;
		rhs->parent = currentScope;
		currentScope = rhs;
	}
}

//------------------------------------------------------------------------------
// exitScope
// Function for exiting a scope if currentScope is equal to root this function
// does nothing.
void SymbolTable::exitScope() {
	if(!(currentScope == root)) {
		currentScope = currentScope->parent;
	}
}

//------------------------------------------------------------------------------
// insert
// Function for inserting a Symbol into the current obect. if root equals NULL
// this function does nothing.
bool SymbolTable::insert(Symbol *rhs) {
	if(root != NULL) {
		return currentScope->idents.insert(rhs);
	}
	else return false;
}

//------------------------------------------------------------------------------
// lookUp
// Function for looking up a Symbol in the current scope and all its parent's
// scopes. If the Symbol is not found this function returns NULL.
Symbol *SymbolTable::lookUp(const std::string &rhs) {
	// Create a Symbol for lookup.
	Symbol temp;
	temp.name = rhs;

	// See if the Symbol is a scope in the current scope if so return it.
	Scope *current = currentScope->child;
	while(current != NULL) {
		if(dynamic_cast<ProcFunc*>(current)->name == rhs) {
			return dynamic_cast<ProcFunc*>(current);
		}
		current = current->next;
	}

	// See if the Symbol is in the current scope if so return it.
	Symbol *pTemp = currentScope->idents.retrieve(temp);
	if(pTemp != NULL && pTemp->valid) {
		return pTemp;
	}
	// Otherwise look in its parents scopes.
	else {
		// Loop through all the scope of all parents.
		Scope *sco = currentScope->parent;
		while(sco != NULL) {
			// See if the Symbol is a scope in the current scope if so return it.
			Scope *current = sco->child;
			while(current != NULL) {
				if(dynamic_cast<ProcFunc*>(current)->name == rhs) {
					return dynamic_cast<ProcFunc*>(current);
				}
				current = current->next;
			}

			// See if the Symbol is in the scope of sco. If so return it.
			pTemp = sco->idents.retrieve(temp);
			if(pTemp != NULL && pTemp->valid) {
				return pTemp;
			}
			sco = sco->parent;
		}
	}

	// If the Symbol was not found return NULL.
	return NULL;
}

//------------------------------------------------------------------------------
// lookUpCS
// Function for looking up a Symbol in the current scope. If the Symbol is not
// found this function returns NULL.
Symbol *SymbolTable::lookUpCS(const std::string &rhs) {
	Symbol temp;
	temp.name = rhs;

	// See if the Symbol is a scope in the current scope if so return it.
	Scope *current = currentScope->child;
	while(current != NULL) {
		if(dynamic_cast<ProcFunc*>(current)->name == rhs) {
			return dynamic_cast<ProcFunc*>(current);
		}
		current = current->next;
	}

	ProcFunc *pTemp = dynamic_cast<ProcFunc*>(currentScope);
	if(pTemp != NULL && pTemp->name == rhs && pTemp->type != NULL) {
		return pTemp;
	}

	Symbol *toReturn = currentScope->idents.retrieve(temp);
	return (toReturn != NULL && toReturn->valid) ? toReturn : NULL;
}

//------------------------------------------------------------------------------
// cullBadPointers
// Function which invalidates bad pointers.
void SymbolTable::cullBadPointers() {
	currentScope->idents.cullBadPointers();
}

//------------------------------------------------------------------------------
// cullBadTypes
// Function which invalidates bad types.
void SymbolTable::cullBadTypes(bool &clean) {
	currentScope->idents.cullBadTypes(clean);
}

//------------------------------------------------------------------------------
// getScopeLevel
// Function for getting scope level.
int SymbolTable::getScopeLevel()const {
	if(currentScope != NULL) {
		return currentScope->level;
	}
	else {
		return 0;
	}
}

//------------------------------------------------------------------------------
// getScopeName
string SymbolTable::getScopeName()const {
	ProcFunc *temp = dynamic_cast<ProcFunc*>(currentScope);
	if(temp != NULL) {
		return temp->name;
	}
	else {
		return string("bad");
	}
}

//------------------------------------------------------------------------------
// printST
// Function for printing the current object.
void SymbolTable::printST() {
	printHelper(root);
}

//------------------------------------------------------------------------------
// printHelper
// Recursive helper function for printing the current object.
void SymbolTable::printHelper(Scope *rhs) {
	// Make sure rhs is not NULL.
	if(rhs != NULL) {
		// Output scope entry line.
		ProcFunc *func = dynamic_cast<ProcFunc*>(rhs);
		for(int i = 0; i < 75; i++) {
			cout << '-';
		}
		cout << endl << "enter " << func->name << endl;

		// Display the current scope's symbols.
		rhs->idents.display(rhs->level);

		// Call printHelper for all the current scope's children.
		Scope *current = rhs->child;
		while(current != NULL) {
			printHelper(current);
			current = current->next;
		}

		// Output scope exit line.
		for(int i = 0; i < 75; i++) {
			cout << '*';
		}
		cout << endl << "exit " << func->name << endl << endl;
	}
}