#include "parsing.h"

const int DebugMode = false; // set to false to turn off debugging messages

// parse the token sequence and rewrite as C++,
//    writing the results to standard output,
// with any error messages directed to standard error
void parse(token tokens[], int size)
{
   // Prints full token contents when DebugMode is enabled
   if (DebugMode) {
      printTokens(tokens, size);
   }

   printPreamble();
   int currPos = 0;
   currPos = parseGlobals(tokens, currPos, size);

   if (currPos >= size || currPos == -1) return;

   currPos = parseMain(tokens, currPos, size);

   if (currPos >= size || currPos == -1) return;

   currPos++;

   if (currPos != size) {
      cerr << "Error: invalid content found after main routine.\n";
      cerr << (size - currPos) << " additional tokens found\n";
   }
}


// print the C++ preamble, featuring include statements
// and namespace declaration
void printPreamble() {
   cout << "#include <iostream>\n";
   cout << "#include <string>\n";
   cout << "using namespace std;\n";
}


// print the C++ main routine title
int parseMain(token tokens[], int currPos, int size) {

   if (currPos >= size || currPos == -1) return currPos;

   if (tokens[currPos].ttype != TokenType::Main) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Main));
      return -1;
   }

   cout << tokenToCPPString(tokens[currPos].ttype);

   currPos++;

   return parseBody(tokens, currPos, size, 0);
}


// parse the global variable declarations
int parseGlobals(token tokens[], int currPos, int size) {

   if (currPos >= size) return currPos;

   if (tokens[currPos].ttype == TokenType::GlobalDef) {
      cout << "\n";
   }

   while (tokens[currPos].ttype == TokenType::GlobalDef) {
      currPos = parseGlobalVars(tokens, currPos, size);

      currPos++;

      if (currPos >= size) return currPos;
      if (currPos == -1) {
         printSectionError("Global Variable Declaration");
         return -1;
      }
   }

   if (tokens[currPos].ttype == TokenType::ProcDef) {
      cout << "\n";
   }

   while (tokens[currPos].ttype == TokenType::ProcDef) {
      currPos = parseProcedureDef(tokens, currPos, size);

      currPos++;

      if (currPos >= size) return currPos;
      if (currPos == -1) {
         printSectionError("Procedure Declaration");
         return -1;
      }
   }

   return currPos;
}


// parse a global variable definition
int parseGlobalVars(token tokens[], int currPos, int size) {

   if (currPos >= size) return currPos;

   string varname = "";

   if (tokens[currPos].ttype != TokenType::GlobalDef) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::GlobalDef));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
      return -1;
   }

   varname = tokens[currPos].content;
   currPos++;

   if (currPos >= size) return size;

   if (!isVariableType(tokens[currPos].ttype)) {
         printError(tokens[currPos], currPos, "Type Specifier");
         return -1;
   }

   cout << tokenToCPPString(tokens[currPos].ttype);
   cout << " " << varname << ";\n";
   return currPos;
}


// parse a procedure definition
int parseProcedureDef(token tokens[], int currPos, int size) {

   if (currPos >= size) return currPos;
   
   string procname = "";
   string params = "";

   if (tokens[currPos].ttype != TokenType::ProcDef) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::ProcDef));
      return -1;
   }

   currPos++;
   if (currPos >= size) return -1;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
      return -1;
   }

   procname = tokens[currPos].content;
   currPos++;
   if (currPos >= size) return -1;

   if (tokens[currPos].ttype != TokenType::Left) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Left));
      return -1;
   }

   currPos++;
   if (currPos >= size) return -1;

   while (isVariableType(tokens[currPos].ttype)) {

         if (params.length() != 0) {
            params += ", ";
         }

         params += tokenToCPPString(tokens[currPos].ttype) + " ";
         currPos ++;

         if (currPos >= size) return -1;

         if (tokens[currPos].ttype != TokenType::Identifier) {
            printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
            return -1;
         }

         params += tokens[currPos].content;
         currPos++;

         if (currPos >= size) return -1;
   }

   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Right) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Right));
      return -1;
   }

   cout << "void " << procname << "(" << params << ")\n";

   return parseBody(tokens, currPos+1, size, 0);
}


// parse a body of code
int parseBody(token tokens[], int currPos, int size, int indent) {

   if (currPos >= size) return currPos;

   if (tokens[currPos].ttype != TokenType::Begin) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Begin));
      return -1;
   }

   printIndent(indent);

   cout << tokenToCPPString(tokens[currPos].ttype) << "\n";

   currPos++;
   if (currPos >= size) return -1;

   while (tokens[currPos].ttype != TokenType::End) {
      switch (tokens[currPos].ttype) {
         case Call:
            currPos = parseProcedureCall(tokens, currPos, size, indent + 1);
            break;
         case Set:
            currPos = parseSetStmt(tokens, currPos, size, indent + 1);
            break;
         case Write:
            currPos = parseOutput(tokens, currPos, size, indent + 1);
            break;
         case Read:
            currPos = parseInput(tokens, currPos, size, indent + 1);
            break;
         case VarDef:
            currPos = parseLocalVarDef(tokens, currPos, size, indent + 1);
            break;
         case If:
            currPos = parseIfLoop(tokens, currPos, size, indent + 1);
            break;
         default:
            printError(tokens[currPos], currPos, "valid expression");
            return -1;
      }

      if (currPos == -1) return -1;
      currPos++;
      if (currPos >= size) return -1;
   }

   if (tokens[currPos].ttype != TokenType::End) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::End));
      return -1;
   }

   printIndent(indent);
   cout << tokenToCPPString(tokens[currPos].ttype) << "\n";

   return currPos;
}


// parse a local variable definition
int parseLocalVarDef(token tokens[], int currPos, int size, int indent) {
   
   if (currPos >= size) return currPos;

   string varname = "";

   if (tokens[currPos].ttype != TokenType::VarDef) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::VarDef));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
      return -1;
   }

   varname = tokens[currPos].content;
   currPos++;

   if (currPos >= size) return size;

   if (!isVariableType(tokens[currPos].ttype)) {
         printError(tokens[currPos], currPos, "Type Specifier");
         return -1;
   }

   printIndent(indent);
   cout << tokenToCPPString(tokens[currPos].ttype);
   cout << " " << varname << ";\n";
   return currPos;
}


// parse a set variable statement
int parseSetStmt(token tokens[], int currPos, int size, int indent) {
      
   if (currPos >= size) return currPos;

   string varname = "";
   string assignValue = "";

   if (tokens[currPos].ttype != TokenType::Set) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Set));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
      return -1;
   }

   varname = tokens[currPos].content;
   currPos++;
   if (currPos >= size) return size;

   if (isLiteralValue(tokens[currPos].ttype)
      || (tokens[currPos].ttype == TokenType::Identifier)) {
         assignValue = tokens[currPos].content;
   } else if (tokens[currPos].ttype == TokenType::Left) {
      currPos = parseExpression(tokens, currPos, size, assignValue);
      if (currPos == -1) return -1;
   } else {
      printError(tokens[currPos], currPos, "Variable, literal value, or expression");
   }

   printIndent(indent);
   cout << varname << " = " << assignValue << ";\n";
   return currPos;
}


// parse an output statement
int parseOutput(token tokens[], int currPos, int size, int indent) {

   if (currPos >= size) return currPos;

   string content = "";

   if (tokens[currPos].ttype != TokenType::Write) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Write));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   currPos = parseExpression(tokens, currPos, size, content);

   if (currPos == -1) return -1;

   printIndent(indent);
   cout << "cout << " << content << " << endl;\n";
   return currPos;
}


// parse an input statement
int parseInput(token tokens[], int currPos, int size, int indent) {

   if (currPos >= size) return currPos;

   if (tokens[currPos].ttype != TokenType::Read) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Read));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (!(tokens[currPos].ttype == TokenType::Identifier)) {
         printError(tokens[currPos], currPos, "Variable name");
         return -1;
   }

   printIndent(indent);
   cout << "cin >> " << tokens[currPos].content << ";\n";
   return currPos;
}


// parse an if loop
int parseIfLoop(token tokens[], int currPos, int size, int indent) {

   if (currPos >= size) return currPos;

   string condStmt = "";

   if (tokens[currPos].ttype != TokenType::If) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::If));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   currPos = parseCondExpression(tokens, currPos, size, condStmt);

   if (currPos == -1) return -1;
   currPos++;
   if (currPos >= size) return size;

   printIndent(indent);
   cout << tokenToCPPString(TokenType::If) << "(" << condStmt << ");\n";

   // parse the if loop body
   currPos = parseBody(tokens, currPos, size, indent);

   if (currPos == -1) return -1;
   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Else) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Else));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   // parse the else loop body
   currPos = parseBody(tokens, currPos, size, indent);

   return currPos;
}


// parse a procedure call
int parseProcedureCall(token tokens[], int currPos, int size, int indent) {

   if (currPos >= size) return currPos;

   string procName = "";
   string args = "";

   if (tokens[currPos].ttype != TokenType::Call) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Call));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (!(tokens[currPos].ttype == TokenType::Identifier)) {
         printError(tokens[currPos], currPos, "Procedure name");
         return -1;
   }

   procName = tokens[currPos].content;

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Left) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Left));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   while((tokens[currPos].ttype != TokenType::Right)) {
      if (args.length() != 0) {
         args += ", ";
      }

      currPos = parseExpression(tokens, currPos, size, args);

      if (currPos == -1) return -1;

      currPos ++;
      if (currPos >= size) return -1;
   }

   if (tokens[currPos].ttype != TokenType::Right) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Right));
      return -1;
   }

   printIndent(indent);
   cout << procName << "(" << args << ");\n";

   return currPos;
}


// parse an expression
int parseExpression(token tokens[], int currPos, int size, string &content) {

   if (currPos >= size) return -1;

   if ((tokens[currPos].ttype == TokenType::Identifier)
      || isLiteralValue(tokens[currPos].ttype)) {
         content += tokens[currPos].content;
         return currPos;
   } else if (tokens[currPos].ttype == TokenType::Left) {
      content += "(";
      currPos ++;
      if (currPos >= size) return -1;

      if (isBinaryOperator(tokens[currPos].ttype)) {
         bool isCondExpOp = isCondExpOperator(tokens[currPos].ttype);
         string binaryOp = tokenToCPPString(tokens[currPos].ttype);

         if (isCondExpOp) {
            currPos = parseCondExpression(tokens, currPos + 1, size, content);
         } else {
            currPos = parseExpression(tokens, currPos + 1, size, content);
         }

         if (currPos == -1) return -1;
         content += " " + binaryOp + " ";
         
         if (isCondExpOp) {
            currPos = parseCondExpression(tokens, currPos + 1, size, content);
         } else {
            currPos = parseExpression(tokens, currPos + 1, size, content);
         }

         if (currPos == -1) return -1;
      } else if (isUnaryOperator(tokens[currPos].ttype)) {
         bool isCondExpOp = isCondExpOperator(tokens[currPos].ttype);
         content += tokenToCPPString(tokens[currPos].ttype);
         
         if (isCondExpOp) {
            currPos = parseCondExpression(tokens, currPos + 1, size, content);
         } else {
            currPos = parseExpression(tokens, currPos + 1, size, content);
         }

         if (currPos == -1) return -1;
      } else {
         printError(tokens[currPos], currPos, "Expression operator");
         return -1;
      }

      currPos ++;
      if (currPos >= size) return -1;

      if (tokens[currPos].ttype != TokenType::Right) {
         printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Right));
         return -1;
      }
      
      content += ")";
      return currPos;
   }

   printError(tokens[currPos], currPos, "Variable name, literal value, or expression");
   return -1;
}

// parse an expression
int parseCondExpression(token tokens[], int currPos, int size, string &content) {

   if (currPos >= size) return -1;

   if (tokens[currPos].ttype != TokenType::Left) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Left));
      return -1;
   }

   content += "(";
   currPos ++;
   if (currPos >= size) return -1;

   if (isBinaryOperator(tokens[currPos].ttype)) {
      if (!isCondOperator(tokens[currPos].ttype)) {
         printCondOpError(currPos);
         return -1;
      }
      
      bool isCondExpOp = isCondExpOperator(tokens[currPos].ttype);
      string binaryOp = tokenToCPPString(tokens[currPos].ttype);

      if (isCondExpOp) {
         currPos = parseCondExpression(tokens, currPos + 1, size, content);
      } else {
         currPos = parseExpression(tokens, currPos + 1, size, content);
      }

      if (currPos == -1) return -1;
      content += " " + binaryOp + " ";

      if (isCondExpOp) {
         currPos = parseCondExpression(tokens, currPos + 1, size, content);
      } else {
         currPos = parseExpression(tokens, currPos + 1, size, content);
      }

      if (currPos == -1) return -1;
   } else if (isUnaryOperator(tokens[currPos].ttype)) {
      if (!isCondOperator(tokens[currPos].ttype)) {
         printCondOpError(currPos);
         return -1;
      }
      bool isCondExpOp = isCondExpOperator(tokens[currPos].ttype);
      content += tokenToCPPString(tokens[currPos].ttype);

      if (isCondExpOp) {
         currPos = parseCondExpression(tokens, currPos + 1, size, content);
      } else {
         currPos = parseExpression(tokens, currPos + 1, size, content);
      }

      if (currPos == -1) return -1;
   } else {
      printError(tokens[currPos], currPos, "Conditional operator");
      return -1;
   }

   currPos ++;
   if (currPos >= size) return -1;

   if (tokens[currPos].ttype != TokenType::Right) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Right));
      return -1;
   }
   
   content += ")";
   return currPos;
}


// prints an error message to cerr indicating the type and position
// of an invalid token
void printError(token tok, int pos, string expected) {
   cerr << "Error: " << tokenTypeToString(tok.ttype);
   cerr << " with value '" << tok.content;
   cerr << "' found in position " << pos << ". ";
   cerr << "Expected to find " << expected << endl;
}


// prints an error indicating the section where an error was found
void printSectionError(string sectionName) {
   cerr << "Error: Malformed content in " << sectionName << " section.\n";
}


// prints an error indicating that an arithmetic expression was used
// when a conditional operation was required
void printCondOpError(int currPos){
   cerr << "Error: Arithmetic operation used in place of conditional operation";
   cerr << " in position " << currPos << endl;
}


// takes a tokenType and returns the string that represents
// the equivalent feature in C++
string tokenToCPPString(TokenType type) {
   switch (type) {
      case IntType:
         return "long";
      case RealType:
         return "double";
      case TextType:
         return "string";
      case BoolType:
         return "bool";
      case Begin:
         return "{";
      case End:
         return "}";
      case Add:
         return "+";
      case Sub:
         return "-";
      case Mul:
         return "*";
      case Div:
         return "/";
      case Rem:
         return "%";
      case EQOp:
         return "==";
      case NEOp:
         return "!=";
      case LTOp:
         return "<";
      case LEOp:
         return "<=";
      case GTOp:
         return ">";
      case GEOp:
         return ">=";
      case AndOp:
         return "&&";
      case OrOp:
         return "||";
      case Negate:
         return "-";
      case NotOp:
         return "!";
      case Main:
         return "\nint main()\n";
      case If:
         return "while";
      default:
         cerr << "Error: Invalid token type: " << tokenTypeToString(type) << endl;
         return "";
   }
}


// print the specified degree of indentation on this line
void printIndent(int indent) {
   for (int i = 0; i < indent; i++) {
      cout << INDENT;
   }
}


// returns true if the token is a valid type for a variable
bool isVariableType(TokenType token) {
   return (token == TokenType::IntType
      || token == TokenType::RealType
      || token == TokenType::TextType
      || token == TokenType::BoolType);
}


// returns true if the token is a literal value
bool isLiteralValue(TokenType token) {
   return (token == TokenType::IntLit
      || token == TokenType::RealLit
      || token == TokenType::TextLit
      || token == TokenType::BoolLit);
}


// returns true if the token is a binary operator
bool isBinaryOperator(TokenType token) {
   return (token == TokenType::Add
      || token == TokenType::Sub
      || token == TokenType::Mul
      || token == TokenType::Div
      || token == TokenType::Rem
      || token == TokenType::EQOp
      || token == TokenType::NEOp
      || token == TokenType::LTOp
      || token == TokenType::LEOp
      || token == TokenType::GTOp
      || token == TokenType::GEOp
      || token == TokenType::AndOp
      || token == TokenType::OrOp);
}


// returns true if the token is a unary operator
bool isUnaryOperator(TokenType token) {
return (token == TokenType::Negate
      || token == TokenType::NotOp);
}


// returns true if the token is a conditional operator
bool isCondOperator(TokenType token) {
return (token == TokenType::NotOp
      || token == TokenType::EQOp
      || token == TokenType::NEOp
      || token == TokenType::LTOp
      || token == TokenType::LEOp
      || token == TokenType::GTOp
      || token == TokenType::GEOp
      || token == TokenType::AndOp
      || token == TokenType::OrOp);
}


// returns true if the token is an operator that only works on
// other conditional operators
bool isCondExpOperator(TokenType token) {
return (token == TokenType::NotOp
      || token == TokenType::AndOp
      || token == TokenType::OrOp);
}
