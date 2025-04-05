#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <regex>
#include <vector>
#include <utility>

using namespace std;


// enumeration of all the token types
enum TokenType {
   Comment = -5, LoneQuote = -4, EndText = -3, StartText = -2, Invalid = -1,
   Begin, End, Main, IntLit, RealLit, TextLit, BoolLit, Identifier,
   GlobalDef, ProcDef, VarDef, Set, Call, Read, Write,
   Left, Right, If, Else, Return, Struct, Array,
   LTOp, GTOp, LEOp, GEOp, EQOp, NEOp, AndOp, OrOp, NotOp, Negate,
   Add, Sub, Mul, Div, Rem, AddAdd, SubSub, AddAddPre, SubSubPre,
   IntType, RealType, TextType, BoolType, VoidType,
};

const unordered_map<TokenType, string> TokenName = {
   {Begin, "Begin"},
   {End, "End"},
   {Main, "Main"},
   {IntLit, "IntegerLiteral"},
   {RealLit, "RealLiteral"},
   {TextLit, "TextLiteral"},
   {Identifier, "Identifier"},
   {BoolLit, "BooleanLiteral"},
   {GlobalDef, "GlobalVariableDef"},
   {ProcDef, "ProcedureDef"},
   {VarDef, "LocalVariableDef"},
   {AndOp, "AndOperator"},
   {OrOp, "OrOperator"},
   {NotOp, "NotOperator"},
   {Negate, "Negation"},
   {Call, "ProcedureCall"},
   {Read, "Read"},
   {Write, "Write"},
   {LTOp, "LessThanOp"},
   {GTOp, "GreaterThanOp"},
   {LEOp, "LessOrEqualOp"},
   {GEOp, "GreaterOrEqualOp"},
   {EQOp, "EqualOp"},
   {NEOp, "NotEqualOp"},
   {Left, "LeftBracket"},
   {Right, "RightBracket"},
   {If, "If"},
   {Else, "Else"},
   {Add, "AddOp"},
   {Sub, "SubtractOp"},
   {Mul, "MultiplyOp"},
   {Div, "DivideOp"},
   {Rem, "RemainderOp"},
   {AddAdd, "IncrementOp"},
   {SubSub, "DecrementOp"},
   {AddAddPre, "IncrementOpPrefix"},
   {SubSubPre, "DecrementOpPrefix"},
   {Set, "Set"},
   {IntType, "IntType"},
   {RealType, "RealType"},
   {TextType, "TextType"},
   {BoolType, "BoolType"},
   {VoidType, "VoidType"},
   {Struct, "Struct"},
   {Array, "Array"},
   {Return, "Return"},
};

const vector<pair<TokenType, regex>> TokenRegex = {
   {Begin, regex("^begin")},
   {End, regex("^end")},
   {Main, regex("^main")},
   {GlobalDef, regex("^gdef")},
   {ProcDef, regex("^pdef")},
   {VarDef, regex("^vdef")},
   {AndOp, regex("^and")},
   {OrOp, regex("^or")},
   {NotOp, regex("^not")},
   {Negate, regex("^neg")},
   {Call, regex("^call")},
   {Read, regex("^read")},
   {Write, regex("^write")},
   {LTOp, regex("^lt")},
   {GTOp, regex("^gt")},
   {LEOp, regex("^le")},
   {GEOp, regex("^ge")},
   {EQOp, regex("^eq")},
   {NEOp, regex("^ne")},
   {Left, regex("^left")},
   {Right, regex("^right")},
   {If, regex("^if")},
   {Else, regex("^else")},
   {Add, regex("^add")},
   {Sub, regex("^sub")},
   {Mul, regex("^mul")},
   {Div, regex("^div")},
   {Rem, regex("^rem")},
   {AddAdd, regex("^addadd")},
   {SubSub, regex("^subsub")},
   {AddAddPre, regex("^addaddpre")},
   {SubSubPre, regex("^subsubpre")},
   {Set, regex("^set")},
   {IntType, regex("^integer")},
   {RealType, regex("^real")},
   {TextType, regex("^text")},
   {BoolType, regex("^boolean")},
   {VoidType, regex("^void")},
   {Struct, regex("^struct")},
   {Array, regex("^array")},
   {Return, regex("^return")},
   {RealLit, regex("^[0-9]+[.][0-9]+")},
   {IntLit, regex("^[0-9]+")},
   {BoolLit, regex("^true|false")},
   {TextLit, regex(R"(^"[^"]*")")},
   {LoneQuote, regex(R"(^"$)")},
   {StartText, regex(R"(^"[^"]*)")},
   {EndText, regex(R"(^[^"]*")")},
   {Comment, regex("^COM")},
   {Identifier, regex("[a-zA-Z]+")},
};


// upper limit on the number of tokens the program can process
const int MaxTokens = 1024;


// each token has a type (from the TokenTypes enum),
//    the associated token text content, and
//    its position in the sequence of valid tokens
struct token {
   TokenType ttype;
   string content;
   int pos;
};


// read each word from standard input,
//    displaying error messages for invalid tokens encountered,
//    filling in the corresponding token information in the tokens array for valid tokens,
// and returning the number of valid tokens read
int tokenize(token tokens[]);


// match an input string with the regex that corresponds to a TokenType
// If no match is found, then return Invalid
TokenType matchTokens(string input);


// display the token information for each token in the array
void printTokens(token tokens[], int size);


// Gathers and returns a text string from input
// Returns false if the text wasn't terminated correctly
bool getTextString(string &textString);


// Ignores input until the next endline
void skipToEndline();


// Takes a tokentype and returns a string that describes it
string tokenTypeToString(TokenType type);
