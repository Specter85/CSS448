// File: stdidents.cpp

#include "stdidents.h"
#include "y.tab.h"
#include "procfunc.h"
#include "simpletype.h"

//------------------------------------------------------------------------------
// Constructor
// Function which builds a new StdIdents object.
StdIdents::StdIdents() {
	// Initialize all elements of stdIdentList with Pascal stdIdents.
	stdIdentList[0].sym = new ProcFunc(0);
	stdIdentList[0].sym->name = "abs";

	stdIdentList[1].sym = new ProcFunc(0);
	stdIdentList[1].sym->name = "arctan";

	stdIdentList[2].sym = new SimpleType();
	stdIdentList[2].sym->name = "boolean";

	stdIdentList[3].sym = new SimpleType();
	stdIdentList[3].sym->name = "char";

	stdIdentList[4].sym = new ProcFunc(0);
	stdIdentList[4].sym->name = "chr";

	stdIdentList[5].sym = new ProcFunc(0);
	stdIdentList[5].sym->name = "cos";

	stdIdentList[6].sym = new ProcFunc(0);
	stdIdentList[6].sym->name = "dispose";

	stdIdentList[7].sym = new ProcFunc(0);
	stdIdentList[7].sym->name = "eof";

	stdIdentList[8].sym = new ProcFunc(0);
	stdIdentList[8].sym->name = "eoln";

	stdIdentList[9].sym = new ProcFunc(0);
	stdIdentList[9].sym->name = "exp";

	stdIdentList[10].sym = new ProcFunc(0);
	stdIdentList[10].sym->name = "get";

	stdIdentList[11].sym = new SimpleType();
	stdIdentList[11].sym->name = "integer";

	stdIdentList[12].sym = new ProcFunc(0);
	stdIdentList[12].sym->name = "ln";

	stdIdentList[13].sym = new ProcFunc(0);
	stdIdentList[13].sym->name = "new";

	stdIdentList[14].sym = new ProcFunc(0);
	stdIdentList[14].sym->name = "odd";

	stdIdentList[15].sym = new ProcFunc(0);
	stdIdentList[15].sym->name = "ord";

	stdIdentList[16].sym = new ProcFunc(0);
	stdIdentList[16].sym->name = "pack";

	stdIdentList[17].sym = new ProcFunc(0);
	stdIdentList[17].sym->name = "page";

	stdIdentList[18].sym = new ProcFunc(0);
	stdIdentList[18].sym->name = "pred";

	stdIdentList[19].sym = new ProcFunc(0);
	stdIdentList[19].sym->name = "put";

	stdIdentList[20].sym = new ProcFunc(0);
	stdIdentList[20].sym->name = "read";

	stdIdentList[21].sym = new ProcFunc(0);
	stdIdentList[21].sym->name = "readln";

	stdIdentList[22].sym = new SimpleType();
	stdIdentList[22].sym->name = "real";

	stdIdentList[23].sym = new ProcFunc(0);
	stdIdentList[23].sym->name = "reset";

	stdIdentList[24].sym = new ProcFunc(0);
	stdIdentList[24].sym->name = "rewrite";

	stdIdentList[25].sym = new ProcFunc(0);
	stdIdentList[25].sym->name = "round";

	stdIdentList[26].sym = new ProcFunc(0);
	stdIdentList[26].sym->name = "sin";

	stdIdentList[27].sym = new ProcFunc(0);
	stdIdentList[27].sym->name = "sqr";

	stdIdentList[28].sym = new ProcFunc(0);
	stdIdentList[28].sym->name = "sqrt";

	stdIdentList[29].sym = new ProcFunc(0);
	stdIdentList[29].sym->name = "succ";

	stdIdentList[30].sym = new ProcFunc(0);
	stdIdentList[30].sym->name = "trunc";

	stdIdentList[31].sym = new ProcFunc(0);
	stdIdentList[31].sym->name = "unpack";

	stdIdentList[32].sym = new ProcFunc(0);
	stdIdentList[32].sym->name = "write";

	stdIdentList[33].sym = new ProcFunc(0);
	stdIdentList[33].sym->name = "writeln";
}

//------------------------------------------------------------------------------
// Destructor
// Function for destroying a StdIdents object.
StdIdents::~StdIdents() {
	for(int i = 0; i < NUM_STD_IDENTS; i++) {
		delete stdIdentList[i].sym;
	}
}

//------------------------------------------------------------------------------
// isStdIdent
// Function which determines if a string is a Pascal StdIdent. Takes a constant 
// string object (rhs) as a parameter and uses a binary search to search for a 
// match in the stdIdentList. Returns true if the stdIdent is found in the Pascal
// stdIdent list and false otherwise.
bool StdIdents::isStdIdent(const string rhs)const {
	// Variables for the binary search.
	int low = 0;
	int high = NUM_STD_IDENTS - 1;
	int mid;

	// Perform the binary search.
	while(low <= high) {
		mid = (low + high) / 2;

		if(stdIdentList[mid].sym->name < rhs) {
			low = mid + 1;
		}
		else if(stdIdentList[mid].sym->name > rhs) {
			high = mid - 1;
		}
		else {
			return true;
		}
	}

	return false;
}


//------------------------------------------------------------------------------
// getSymbol
// Function which retrieves/returns a pointer to a StdIdent Symbol. Takes a
// constant string object (rhs) as a parameter and uses a binary search to
// search for a match in the stdIdentList. Returns a pointer to the symbol if
// the stdIdent is found in the Pascal stdIdent list and NULL otherwise.
Symbol *StdIdents::getSymbol(const string rhs)const {
	// Variables for the binary search.
	int low = 0;
	int high = NUM_STD_IDENTS - 1;
	int mid;

	// Perform the binary search.
	while(low <= high) {
		mid = (low + high) / 2;

		if(stdIdentList[mid].sym->name < rhs) {
			low = mid + 1;
		}
		else if(stdIdentList[mid].sym->name > rhs) {
			high = mid - 1;
		}
		else {
			return stdIdentList[mid].sym;
		}
	}

	return NULL;
}
