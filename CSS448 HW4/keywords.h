// File: keywords.h
// Header for the Keywords class.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef KEYWORDS_H
#define KEYWORDS_H
#include <string>
using namespace std;

//------------------------------------------------------------------------------
// Keywords class: A class which stores Pascal's keywords and allows one to
// determine if a string is a Pascal keyword.
//
// ADT Keyword class: A class which maintains the keywords of a language and
// supports determining if a string is a keyword and returning the token
// associated with a keyword given its string.
//------------------------------------------------------------------------------
class Keywords {
private:

	// Struct for a Keyword.
	struct Keyword {
		string tokenName;		// The string representation of a Keyword.
		int tokenVal;			// The int representation of a Keyword.

		~Keyword() {}			// Destructor for strings.
	};

	// Const for the number of keywords in the class.
	static const int NUM_KEYWORDS = 32;
	// Const for invalid strings.
	static const int INVALID_KEYWORD = -1;
	// The list of keywords in the class.
	Keyword keywordList[NUM_KEYWORDS];

public:

	Keywords();					// Constructor
	~Keywords();				// Destructor

	// Function which determines if a string is a Pascal Keyword.
	bool isKeyword(const string)const;
	// Function which returns the token of a Pascal Keyword given its
	// string.
	int getTokenVal(const string)const;

};

#endif