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

	keywordList[5].tokenName = "dispose";
	keywordList[5].tokenVal = ydispose;

	keywordList[6].tokenName = "div";
	keywordList[6].tokenVal = ydiv;

	keywordList[7].tokenName = "do";
	keywordList[7].tokenVal = ydo;

	keywordList[8].tokenName = "downto";
	keywordList[8].tokenVal = ydownto;

	keywordList[9].tokenName = "else";
	keywordList[9].tokenVal = yelse;

	keywordList[10].tokenName = "end";
	keywordList[10].tokenVal = yend;

	keywordList[11].tokenName = "false";
	keywordList[11].tokenVal = yfalse;

	keywordList[12].tokenName = "for";
	keywordList[12].tokenVal = yfor;

	keywordList[13].tokenName = "function";
	keywordList[13].tokenVal = yfunction;

	keywordList[14].tokenName = "if";
	keywordList[14].tokenVal = yif;

	keywordList[15].tokenName = "in";
	keywordList[15].tokenVal = yin;

	keywordList[16].tokenName = "mod";
	keywordList[16].tokenVal = ymod;

	keywordList[17].tokenName = "new";
	keywordList[17].tokenVal = ynew;

	keywordList[18].tokenName = "nil";
	keywordList[18].tokenVal = ynil;

	keywordList[19].tokenName = "not";
	keywordList[19].tokenVal = ynot;

	keywordList[20].tokenName = "of";
	keywordList[20].tokenVal = yof;

	keywordList[21].tokenName = "or";
	keywordList[21].tokenVal = yor;

	keywordList[22].tokenName = "procedure";
	keywordList[22].tokenVal = yprocedure;

	keywordList[23].tokenName = "program";
	keywordList[23].tokenVal = yprogram;

	keywordList[24].tokenName = "read";
	keywordList[24].tokenVal = yread;

	keywordList[25].tokenName = "readln";
	keywordList[25].tokenVal = yreadln;

	keywordList[26].tokenName = "record";
	keywordList[26].tokenVal = yrecord;

	keywordList[27].tokenName = "repeat";
	keywordList[27].tokenVal = yrepeat;

	keywordList[28].tokenName = "set";
	keywordList[28].tokenVal = yset;

	keywordList[29].tokenName = "then";
	keywordList[29].tokenVal = ythen;

	keywordList[30].tokenName = "to";
	keywordList[30].tokenVal = yto;

	keywordList[31].tokenName = "true";
	keywordList[31].tokenVal = ytrue;

	keywordList[32].tokenName = "type";
	keywordList[32].tokenVal = ytype;

	keywordList[33].tokenName = "until";
	keywordList[33].tokenVal = yuntil;

	keywordList[34].tokenName = "var";
	keywordList[34].tokenVal = yvar;

	keywordList[35].tokenName = "while";
	keywordList[35].tokenVal = ywhile;

	keywordList[36].tokenName = "write";
	keywordList[36].tokenVal = ywrite;

	keywordList[37].tokenName = "writeln";
	keywordList[37].tokenVal = ywriteln;
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