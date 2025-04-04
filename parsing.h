#pragma once

#include "tokenizing.h"
#include <string>

using std::string;

const string INDENT = "   ";

// parse the token sequence and rewrite as C++,
//    writing the results to standard output,
// with any error messages directed to standard error
void parse(token tokens[], int size);


// print the C++ preamble, featuring include statements
// and namespace declaration
void printPreamble();


// parse the main routine
int parseMain(token tokens[], int currPos, int size);


// parse the global variable declarations
int parseGlobals(token tokens[], int currPos, int size);


// parse a global variable definition
int parseGlobalVars(token tokens[], int currPos, int size);


// parse a procedure definition
int parseProcedureDef(token tokens[], int currPos, int size);


// parse a body of code
int parseBody(token tokens[], int currPos, int size, int indent);


// parse a local variable definition
int parseLocalVarDef(token tokens[], int currPos, int size, int indent);


// parse a set variable statement
int parseSetStmt(token tokens[], int currPos, int size, int indent);


// parse an output statement
int parseOutput(token tokens[], int currPos, int size, int indent);


// parse an input statement
int parseInput(token tokens[], int currPos, int size, int indent);


// parse an if loop
int parseIfLoop(token tokens[], int currPos, int size, int indent);


// parse a procedure call
int parseProcedureCall(token tokens[], int currPos, int size, int indent);


// parse an expression
int parseExpression(token tokens[], int currPos, int size, string &content);


// parse a conditional expression
int parseCondExpression(token tokens[], int currPos, int size, string &content);


// prints an error message to cerr indicating the type and position
// of an invalid token
void printError(token tok, int pos, string expected);


// prints an error indicating the section where an error was found
void printSectionError(string sectionName);


// prints an error indicating that an arithmetic expression was used
// when a conditional operation was required
void printCondOpError(int currPos);


// takes a tokenType representing a variable type, and returns
// the equivalent type string in C++
string varTypeTokenToString(TokenType type);


// takes a tokenType and returns the string that represents
// the equivalent feature in C++
string tokenToCPPString(TokenType type);


// print the specified degree of indentation on this line
void printIndent(int indent);


// returns true if token is a valid type for a variable
bool isVariableType(TokenType token);


// returns true if the token is a literal value
bool isLiteralValue(TokenType token);


// returns true if the token is a binary operator
bool isBinaryOperator(TokenType token);


// returns true if the token is a unary operator
bool isUnaryOperator(TokenType token);


// returns true if the token is a conditional operator
bool isCondOperator(TokenType token) ;


// returns true if the token is an operator that only works on
// other conditional operators
bool isCondExpOperator(TokenType token);
