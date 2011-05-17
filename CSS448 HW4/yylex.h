// File: yylex.h
// Function prototypes for yylex and helper functions.
// Programers Mark Zachacz, Ilya Novichenko

#ifndef YYLEX_H
#define YYLEX_H
#include <string>
using namespace std;

//------------------------------------------------------------------------------
// yylex: function which takes input from cin and returns integer tokens. This
// function makes up the lexical analyzer for a compiler.
//
// Assumptions:  
//		-- Input files are given by redirecting standard in with the < operator.
//		-- All identifiers are made lowercase because Pascal is not case 
//		   sensitive.
//		-- The computer will not run out of memory.
//------------------------------------------------------------------------------

// Variable which holds the value of tokens such as numbers and
// strings.
extern string value;

// Int value which signifies that yylex has reached the end of a file.
const int END_OF_FILE = -1;

// Function prototype for the lexical analyzer.
int yylex();

// Function for determining if a char is a letter.
bool isLetter(char);
// Function for determinging if a char is valid for the body of an
// identifier.
bool isIdentChar(char);
// Function for determining is a char is a number between 0 and 9.
bool isDigit(char);

#endif