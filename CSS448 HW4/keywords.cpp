// File: keywords.cpp
// Function definitions for the Keywords class.

#include "keywords.h"
#include "y.tab.h"

//------------------------------------------------------------------------------
// Constructor
// Function which builds a new Keywords object.
Keywords::Keywords() {
	// Initialize all elements of keywordList with Pascal keywords.
	keywordList[0].tokenName = "and";
	keywordList[0].tokenVal = yand;

	keywordList[1].tokenName = "array";
	keywordList[1].tokenVal = yarray;

	keywordList[2].tokenName = "begin";
	keywordList[2].tokenVal = ybegin;

	keywordList[3].tokenName = "case";
	keywordList[3].tokenVal = ycase;

	keywordList[4].tokenName = "const";
	keywordList[4].tokenVal = yconst;

	keywordList[5].tokenName = "div";
	keywordList[5].tokenVal = ydiv;

	keywordList[6].tokenName = "do";
	keywordList[6].tokenVal = ydo;

	keywordList[7].tokenName = "downto";
	keywordList[7].tokenVal = ydownto;

	keywordList[8].tokenName = "else";
	keywordList[8].tokenVal = yelse;

	keywordList[9].tokenName = "end";
	keywordList[9].tokenVal = yend;

	keywordList[10].tokenName = "false";
	keywordList[10].tokenVal = yfalse;

	keywordList[11].tokenName = "for";
	keywordList[11].tokenVal = yfor;

	keywordList[12].tokenName = "function";
	keywordList[12].tokenVal = yfunction;

	keywordList[13].tokenName = "if";
	keywordList[13].tokenVal = yif;

	keywordList[14].tokenName = "in";
	keywordList[14].tokenVal = yin;

	keywordList[15].tokenName = "mod";
	keywordList[15].tokenVal = ymod;

	keywordList[16].tokenName = "nil";
	keywordList[16].tokenVal = ynil;

	keywordList[17].tokenName = "not";
	keywordList[17].tokenVal = ynot;

	keywordList[18].tokenName = "of";
	keywordList[18].tokenVal = yof;

	keywordList[19].tokenName = "or";
	keywordList[19].tokenVal = yor;

	keywordList[20].tokenName = "procedure";
	keywordList[20].tokenVal = yprocedure;

	keywordList[21].tokenName = "program";
	keywordList[21].tokenVal = yprogram;

	keywordList[22].tokenName = "record";
	keywordList[22].tokenVal = yrecord;

	keywordList[23].tokenName = "repeat";
	keywordList[23].tokenVal = yrepeat;

	keywordList[24].tokenName = "set";
	keywordList[24].tokenVal = yset;

	keywordList[25].tokenName = "then";
	keywordList[25].tokenVal = ythen;

	keywordList[26].tokenName = "to";
	keywordList[26].tokenVal = yto;

	keywordList[27].tokenName = "true";
	keywordList[27].tokenVal = ytrue;

	keywordList[28].tokenName = "type";
	keywordList[28].tokenVal = ytype;

	keywordList[29].tokenName = "until";
	keywordList[29].tokenVal = yuntil;

	keywordList[30].tokenName = "var";
	keywordList[30].tokenVal = yvar;

	keywordList[31].tokenName = "while";
	keywordList[31].tokenVal = ywhile;
}

//------------------------------------------------------------------------------
// Destructor
// Function for destroying a Keywords object.
Keywords::~Keywords() {
}

//------------------------------------------------------------------------------
// isKeyword
// Function which determines if a string is a Pascal Keyword. Takes a constant 
// string object (rhs) as a parameter and uses a binary search to search for a 
// match in the keywordList. Returns true if the keyword is found in the Pascal
// keyword list and false otherwise.
bool Keywords::isKeyword(const string rhs)const {
	// Variables for the binary search.
	int low = 0;
	int high = NUM_KEYWORDS - 1;
	int mid;

	// Perform the binary search.
	while(low <= high) {
		mid = (low + high) / 2;

		if(keywordList[mid].tokenName < rhs) {
			low = mid + 1;
		}
		else if(keywordList[mid].tokenName > rhs) {
			high = mid - 1;
		}
		else {
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
// getTokenVal
// Function which returns the token of a Pascal Keyword given its string. Takes 
// a constant string object (rhs) as a parameter and uses a binary search to 
// search for a match in the keywordList. Returns the token of the keyword if it 
// is found in the Pascal keyword list and INVALID_KEYWOD otherwise.
int Keywords::getTokenVal(const string rhs)const {
	// Variables for the binary search.
	int low = 0;
	int high = NUM_KEYWORDS - 1;
	int mid;

	// Perform the binary search.
	while(low <= high) {
		mid = (low + high) / 2;

		if(keywordList[mid].tokenName < rhs) {
			low = mid + 1;
		}
		else if(keywordList[mid].tokenName > rhs) {
			high = mid - 1;
		}
		else {
			return keywordList[mid].tokenVal;
		}
	}

	return INVALID_KEYWORD;
}