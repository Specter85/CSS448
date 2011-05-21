// File: yylex.cpp
// Function definitions for functions defined in yylex.h.
// Programers Mark Zachacz, Ilya Novichenko

#include "yylex.h"
#include "y.tab.h"
#include "keywords.h"
#include <ctype.h>
#include <algorithm>
#include <iostream>

//extern YYSTYPE yylval;

// Variable for the value of tokens which have values.
string value("");

// Variables which hold chars that had to be read ahead to determine a token.
char *last = NULL;
char *last2 = NULL;

//------------------------------------------------------------------------------
// yylex
// Function which returns tokens based on input from cin. These tokens will be
// based on the Pascal language.
int yylex() {
	for(;;) {
		char curC[2];					// char array for next read character
		curC[1] = '\0';					// make append func. work
		char &current = curC[0];		// refernce the actual char to use
		
		// If an extra char had to be read during the last call to yylex get it.
		if(last != NULL) {
			current = *last;
			delete last;
			last = NULL;
		}
		// If a second extra char had to be read and the first has already been
		// used get the second one.
		else if(last2 != NULL) {
			current = *last2;
			delete last2;
			last2 = NULL;
		}
		else {
			current = cin.get();		// get next character in the cin stream
		}

		// If we are at the end of the file return a special int.
		if(cin.eof()) {
			return END_OF_FILE;
		}
		// Disgard spaces.
		else if(current == ' ' || current == '\t') {
			continue;
		}
		// If we see a newline display it then skip it.
		else if(current == '\n') {
			cout << endl;
			continue;
		}
		// See if the next token is a keyword or identifier.
		else if(isLetter(current)) {
			// Object which knows Pascal's keywords.
			Keywords possibleKeywords;
			string currentToken;					// Token being built.
			currentToken.append(curC);

			// If there is a value in last2 take it into consideration.
			if(last2 != NULL) {
				// If last2 is valid for the body of a identifier add it to
				// to the current token.
				if(isIdentChar(*last2)) {
					char temp[2];
					temp[0] = *last2;
					temp[1] = '\0';
					currentToken.append(temp);
					delete last2;
					last2 = NULL;
				}
				// Otherwise return the token as it is as an ident since there
				// are no keywords with only one char in them.
				else {
					value = currentToken;
					//yylval.str = value.c_str();
					return yident;
				}
			}

			// Get all chars in the current token and append them to 
			// currentToken.
			for(;;) {
				current = cin.peek();
				if(isIdentChar(current)) {
					cin.get();
					currentToken.append(curC);
				}
				else {
					break;
				}
			}

			// Because pascal is not case sensitive make all caracters of the
			// current token lowercase.
			transform(currentToken.begin(), currentToken.end(), 
				currentToken.begin(), static_cast<int (*)(int)>(tolower));

			// Determine if currentToken is a Pascal keyword if so return 
			// its yval.
			if(possibleKeywords.isKeyword(currentToken)) {
				return possibleKeywords.getTokenVal(currentToken);
			}
			// Otherwise return the current token as an identifier.
			else {
				value = currentToken;
				//yylval.str = value.c_str();
				return yident;
			}

		}
		// See if the next token is a number.
		else if(isDigit(current)) {
			string currentToken;					// Token being built.
			currentToken.append(curC);
			bool hasDecimal = false;
			bool hasExponent = false;

			// Loop to get all digits in the number.
			for(;;) {
				// Get the next char.
				current = cin.peek();
				// If the next char is a number just append it.
				if(isDigit(current)) {
					cin.get();
					currentToken.append(curC);
				}
				// If the next char is a dot and the current token does
				// not have a decimal check to see if the char after it 
				// in the stream is a number if so append the dot otherwise
				// return the number as it is and save the dot.
				else if(current == '.' && !hasDecimal) {
					cin.get();
					if(isDigit(cin.peek())) {
						currentToken.append(curC);
						hasDecimal = true;
					}
					else {
						last = new char(current);
						break;
					}
				}
				// If the next char is an e and the current token does
				// not have an exponent check to see if the char after it
				// in the stream is a number or a - or + if it is a number
				// append the e to the token if it is a - or + check to see
				// if the next char in the stream is a number if so append
				// the - or + to the stream other wise return the number
				// as is and save the e and possible - or +.
				else if((current == 'e' || current == 'E') && !hasExponent) {
					cin.get();
					if(isDigit(cin.peek())) {
						currentToken.append("e");
						hasDecimal = true;
						hasExponent = true;
					}
					else if(cin.peek() == '-' || cin.peek() == '+') {
						current = cin.get();
						if(isDigit(cin.peek())) {
							currentToken.append("e");
							currentToken.append(curC);
							hasDecimal = true;
							hasExponent = true;
						}
						else {
							last = new char('e');
							last2 = new char(current);
							break;
						}
					}		
					else {
						last = new char(current);
						break;
					}
				}
				else {
					break;
				}
			}

			// If done reading the number return ynumber and save
			// the value in value.
			value = currentToken;
			//yylval.str = value.c_str();
			return ynumber;
		}
		// Take care of Pascal operators delimiter and comments.
		else {
			switch(current) {
				case ':':
					// If there is a = after the : return yassign.
					if(cin.peek() == '=') {
						cin.get();
						return yassign;
					}
					// Otherwise return ycolon.
					else {
						return ycolon;
					}
				case '+':
					return yplus;
				case '-':
					return yminus;
				case '*':
					return ymultiply;
				case '/':
					return ydivide;
				case '=':
					return yequal;
				case '<':
					// If there is a > after the < return ynotequal to.
					if(cin.peek() == '>') {
						cin.get();
						return ynotequal;
					}
					// If there is a = after the < return ylessequal.
					else if(cin.peek() == '=') {
						cin.get();
						return ylessequal;
					}
					// Otherwise return yless.
					else {
						return yless;
					}
				case '>':
					// If there is a = after the > return ygreaterequal.
					if(cin.peek() == '=') {
						cin.get();
						return ygreaterequal;
					}
					// Otherwise return ygreater.
					else {
						return ygreater;
					}
				case '(':
					// If there is a * after the ( check for a comment.
					if(cin.peek() == '*') {
						cin.get();

						// Output the comment and continue.
						cout << "/*";
						for(;;) {
							char temp;
							temp = cin.get();
							if(cin.eof()) {
								return END_OF_FILE;
							}

							if(temp == '*' && cin.peek() == ')') {
								cin.get();
								cout << "*/";
								break;
							}
							else {
								cout << temp;
							}
						}
						continue;
					}
					// Otherwise return yleftparen.
					else {
						return yleftparen;
					}
				case ')':
					return yrightparen;
				case '[':
					return yleftbracket;
				case '{':
					// Output the comment.
					cout << "/*";
					for(;;) {
						char temp;
						temp = cin.get();
						if(cin.eof()) {
							return END_OF_FILE;
						}

						if(temp == '}') {
							cout << "*/";
							break;
						}
						else {
							cout << temp;
						}
					}
					continue;
				case ']':
					return yrightbracket;
				case ',':
					return ycomma;
				case '.':
					// If there is a . after the . return ydotdot.
					if(cin.peek() == '.') {
						cin.get();
						return ydotdot;
					}
					// Otherwise return ydot.
					else {
						return ydot;
					}
				case ';':
					return ysemicolon;
				case '^':
					return ycaret;
				case '"':
					{
						string currentString;
						// Loop till the end of string token is found
						// appending the chars to currentString if eof
						// is found return END_OF_FILE.
						for(;;) {
							current = cin.get();
							if(cin.eof()) {
								return END_OF_FILE;
							}

							if(current == '"') {
								value = currentString;
								return ystring;
							}
							else {
								currentString.append(curC);
							}
						}
					}
				case '\'':
					{
						string currentString;
						// Loop till the end of string token is found
						// appending the chars to currentString if eof
						// is found return END_OF_FILE.
						for(;;) {
							current = cin.get();
							if(cin.eof()) {
								return END_OF_FILE;
							}

							if(current == '\'') {
								value = currentString;
								//yylval.str = value.c_str();
								return ystring;
							}
							else {
								currentString.append(curC);
							}
						}
					}
				default:
					value.assign(curC);
					return yunknown;
			}
		}
	}
}

//------------------------------------------------------------------------------
// isLetter
// Function for determining if a char is a letter.
bool isLetter(char rhs) {
	return (rhs >= 'a' && rhs <= 'z') ||
		(rhs >= 'A' && rhs <= 'Z');
}

//------------------------------------------------------------------------------
// isIdentChar
// Function for determinging if a char is valid for the body of an
// identifier.
bool isIdentChar(char rhs) {
	return (rhs >= 'a' && rhs <= 'z') ||
		(rhs >= 'A' && rhs <= 'Z') ||
		(rhs >= '0' && rhs <= '9');
}

//------------------------------------------------------------------------------
// isDigit
// Function for determining if a char is a number between 0 and 9.
bool isDigit(char rhs) {
	return (rhs >= '0' && rhs <= '9');
}