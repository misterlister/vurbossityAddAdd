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

   if (tokens[currPos].ttype == TokenType::StructDef) {
      cout << "\n";
   }

   while (tokens[currPos].ttype == TokenType::StructDef) {
      currPos = parseStructDef(tokens, currPos, size);
      cout << "\n";
      if (currPos == -1) {
         printSectionError("Struct Declaration");
         return -1;
      }

      currPos++;

      if (currPos >= size) return currPos;
   }

   if (tokens[currPos].ttype == TokenType::ProcDef) {
      cout << "\n";
   }

   while (tokens[currPos].ttype == TokenType::ProcDef) {
      currPos = parseProcedureDef(tokens, currPos, size);
      cout << "\n";
      if (currPos == -1) {
         printSectionError("Procedure Declaration");
         return -1;
      }

      currPos++;

      if (currPos >= size) return currPos;
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

   // If it is an array, parse it as an array
   if (tokens[currPos].ttype == TokenType::Array) {
      string content = "";
      currPos = parseArrayDef(tokens, currPos, size, content);
      if (currPos == -1) return -1;
      cout << content << ";\n";
      return currPos;
   // Otherwise, return an error if its not an identifier
   } else if (tokens[currPos].ttype != TokenType::Identifier) {
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
   string retType = "";

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

   while (isParameterType(tokens[currPos].ttype)) {

         if (params.length() != 0) {
            params += ", ";
         }

         if (tokens[currPos].ttype == TokenType::Array) {
            currPos++;
            if (currPos >= size) return -1;

            if (!isVariableType(tokens[currPos].ttype)) {
               printError(tokens[currPos], currPos, "Array data type");
               return -1;
            }

            params += tokenToCPPString(tokens[currPos].ttype) + " ";
            currPos ++;

            if (currPos >= size) return -1;

            if (tokens[currPos].ttype != TokenType::Identifier) {
               printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
               return -1;
            }

            params += tokens[currPos].content + "[]";
         } else if (tokens[currPos].ttype == TokenType::StructType) {
            currPos++;
            if (currPos >= size) return -1;

            if (tokens[currPos].ttype != TokenType::Identifier) {
               printError(tokens[currPos], currPos, "Struct data type");
               return -1;
            }

            params += tokens[currPos].content + " ";
            currPos ++;

            if (currPos >= size) return -1;

            if (tokens[currPos].ttype != TokenType::Identifier) {
               printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
               return -1;
            }

            params += "&" + tokens[currPos].content;
         } else {
            params += tokenToCPPString(tokens[currPos].ttype) + " ";
            currPos ++;

            if (currPos >= size) return -1;

            if (tokens[currPos].ttype != TokenType::Identifier) {
               printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
               return -1;
            }

            params += tokens[currPos].content;
         }

         currPos++;
         if (currPos >= size) return -1;
   }

   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Right) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Right));
      return -1;
   }

   if (currPos+1 >= size) return -1;

   // does this procedure have a return type?
   if (isReturnType(tokens[currPos+1].ttype)) {
      currPos++;
      retType = tokenToCPPString(tokens[currPos].ttype);
   } else {
      retType = tokenToCPPString(TokenType::VoidType);
   }

   cout << retType << " " << procname << "(" << params << ")\n";

   return parseBody(tokens, currPos+1, size, 0);
}


// parse a struct definition
int parseStructDef(token tokens[], int currPos, int size) {
   if (currPos >= size) return currPos;
   
   string structname = "";
   string elements = "";

   if (tokens[currPos].ttype != TokenType::StructDef) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::StructDef));
      return -1;
   }

   currPos++;
   if (currPos >= size) return -1;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
      return -1;
   }

   structname = tokens[currPos].content;
   currPos++;
   if (currPos >= size) return -1;

   if (tokens[currPos].ttype != TokenType::Begin) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Begin));
      return -1;
   }

   currPos++;
   if (currPos >= size) return -1;

   while (tokens[currPos].ttype == TokenType::Element) {
      currPos = parseStructElem(tokens, currPos, size, elements);
      if (currPos == -1) return -1;
      currPos++;
      if (currPos >= size) return -1;
   }

   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::End) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::End));
      return -1;
   }

   cout << "struct " << structname << "\n{\n" << elements << "};\n";

   return currPos;
}


// parse a struct element
int parseStructElem(token tokens[], int currPos, int size, string &content) {
   if (tokens[currPos].ttype != TokenType::Element) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Element));
      return -1;
   }

   content += INDENT;

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype == TokenType::ArraySet) {
      currPos = parseArraySet(tokens, currPos, size, content);
   } else if (tokens[currPos].ttype == TokenType::Identifier) {
      string elementName = tokens[currPos].content;
      currPos ++;

      if (currPos >= size) return -1;

      if (!isVariableType(tokens[currPos].ttype)) {
         printError(tokens[currPos], currPos, "Valid struct element type");
         return -1;
      }

      content += tokenToCPPString(tokens[currPos].ttype) + " " + elementName + ";\n";

   } else {
      printError(tokens[currPos], currPos, "Valid struct element");
      return -1;
   }
   return currPos;
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
      string content = "";
      switch (tokens[currPos].ttype) {
         case Call:
            printIndent(indent + 1);
            currPos = parseProcedureCall(tokens, currPos, size, content);
            cout << content << ";\n";
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
         case Left:
            currPos = parseStandaloneStmt(tokens, currPos, size, indent + 1);
            break;
         case Return:
            currPos = parseReturnStmt(tokens, currPos, size, indent + 1);
            break;
         case ArraySet:
            printIndent(indent + 1);
            currPos = parseArraySet(tokens, currPos, size, content);
            cout << content << ";\n";
            break;
         case StructBuild:
            currPos = parseStructBuild(tokens, currPos, size, indent + 1);
            break;
         case StructElemSet:
         case StructIndirElemSet:
            currPos = parseStructSet(tokens, currPos, size, indent + 1);
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

      // If it is an array, parse it as an array
   if (tokens[currPos].ttype == TokenType::Array) {
      string content = "";
      currPos = parseArrayDef(tokens, currPos, size, content);
      if (currPos == -1) return -1;
      cout << content << ";\n";
      return currPos;
   // Otherwise, return an error if its not an identifier
   } else if (tokens[currPos].ttype != TokenType::Identifier) {
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

   currPos = parseExpression(tokens, currPos, size, assignValue);
   if (currPos == -1) return -1;

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


// parse a standalone statement
int parseStandaloneStmt(token tokens[], int currPos, int size, int indent) {
   
   string content = "";
   currPos = parseIncrement(tokens, currPos, size, content);

   if (currPos == -1) {
      return -1;
   }

   printIndent(indent);
   cout << content << ";\n";

   return currPos;
}


// parse a return statement
int parseReturnStmt(token tokens[], int currPos, int size, int indent) {

   if (tokens[currPos].ttype != TokenType::Return) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Return));
      return -1;
   }

   currPos ++;
   if (currPos >= size) return -1;

   string content = "return ";

   currPos = parseExpression(tokens, currPos, size, content);

   if (currPos >= size || currPos == -1) return -1;

   printIndent(indent);
   cout << content << ";\n";

   return currPos;
}


// parse an increment/decrement statement
int parseIncrement(token tokens[], int currPos, int size, string &content) {

   if (currPos >= size) return -1;

   if (tokens[currPos].ttype != TokenType::Left) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Left));
      return -1;
   }

   currPos ++;
   if (currPos >= size) return -1;

   if (isPreIncrementOperator(tokens[currPos].ttype)) {
      content += tokenToCPPString(tokens[currPos].ttype);

      currPos++;
      if (currPos >= size) return -1;

      if (tokens[currPos].ttype != TokenType::Identifier) {
         printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
         return -1;
      }

      content += tokens[currPos].content;

      currPos ++;
      if (currPos >= size) return -1;

      if (tokens[currPos].ttype != TokenType::Right) {
         printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Right));
         return -1;
      }

      return currPos;

   } else if (isPostIncrementOperator(tokens[currPos].ttype)) {
      string op = tokenToCPPString(tokens[currPos].ttype);

      currPos ++;
      if (currPos >= size) return -1;

      if (tokens[currPos].ttype != TokenType::Identifier) {
         printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
         return -1;
      }

      content += tokens[currPos].content + op;

      currPos++;
      if (currPos >= size) return -1;

      if (tokens[currPos].ttype != TokenType::Right) {
         printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Right));
         return -1;
      }

      return currPos;
   }

   printError(tokens[currPos], currPos, "Increment operation");
   return -1;
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
   cout << tokenToCPPString(TokenType::If) << "(" << condStmt << ")\n";

   // parse the if loop body
   currPos = parseBody(tokens, currPos, size, indent);

   if (currPos == -1) return -1;
   if (currPos + 1 >= size) return size;

   if (tokens[currPos + 1].ttype != TokenType::Else) {
      return currPos;
   }

   currPos++;

   // parse the else loop body
   currPos = parseBody(tokens, currPos + 1, size, indent);

   return currPos;
}


// parse a procedure call
int parseProcedureCall(token tokens[], int currPos, int size, string &content) {

   if (currPos >= size) return currPos;

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

   content += tokens[currPos].content;

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

   content += "(" + args + ")";

   return currPos;
}


// parse an expression
int parseExpression(token tokens[], int currPos, int size, string &content) {

   if (currPos >= size) return -1;

   if ((tokens[currPos].ttype == TokenType::Identifier)
      || isLiteralValue(tokens[currPos].ttype)) {
         content += tokens[currPos].content;
         return currPos;
   } else if (tokens[currPos].ttype == TokenType::ArrayAccess) {
      return parseArrayAccess(tokens, currPos, size, content);
   } else if (tokens[currPos].ttype == TokenType::StructElemAccess
      || tokens[currPos].ttype == TokenType::StructIndirElemAccess) {
      return parseStructAccess(tokens, currPos, size, content);
   } else if (tokens[currPos].ttype == TokenType::Call) {
      return parseProcedureCall(tokens, currPos, size, content);
   } else if (tokens[currPos].ttype == TokenType::Left) {
      if ((currPos + 1) >= size) return -1;
      if (isIncrementOperator(tokens[currPos + 1].ttype)) {
         return parseIncrement(tokens, currPos, size, content);
      }

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


// parse a conditional expression
int parseCondExpression(token tokens[], int currPos, int size, string &content) {

   if (currPos >= size) return -1;

   if (tokens[currPos].ttype != TokenType::Left) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Left));
      return -1;
   }

   content += "(";
   currPos ++;
   if (currPos >= size) return -1;

   // expression is a boolean literal or identifier
   if (tokens[currPos].ttype == TokenType::BoolLit
   || tokens[currPos].ttype == TokenType::Identifier) {
      content += tokens[currPos].content;
   }
   // expression uses a binary operator
   else if (isBinaryOperator(tokens[currPos].ttype)) {
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
   // expression uses a unary operator
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
   // not a valid conditional expression
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


// parse an implementation of an array
int parseArrayDef(token tokens[], int currPos, int size, string &content) {
   
   if (currPos >= size) return currPos;

   string varname = "";
   string arrayType = "";
   string arraySize = "";

   if (tokens[currPos].ttype != TokenType::Array) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Array));
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

   arrayType = tokenToCPPString(tokens[currPos].ttype);

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::IntLit) {
      printError(tokens[currPos], currPos, "Valid array Size");
      return -1;
   }

   arraySize = tokens[currPos].content;

   content += arrayType + " " + varname + "[" + arraySize + "]";
   return currPos;
}


// parse an array set statement
int parseArraySet(token tokens[], int currPos, int size, string &content) {
   if (currPos >= size) return currPos;

   string varname = "";
   string index = "";
   string value = "";

   if (tokens[currPos].ttype != TokenType::ArraySet) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::ArraySet));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
      return -1;
   }

   varname += tokens[currPos].content;

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::IntLit
      && tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, "Valid array index");
      return -1;
   }

   index += tokens[currPos].content;

   currPos++;
   if (currPos >= size) return size;

   currPos = parseExpression(tokens, currPos, size, value);

   if (currPos == -1) return -1;

   content += varname + "[" + index + "] = " + value;

   return currPos;
}


// parse an array access statement
int parseArrayAccess(token tokens[], int currPos, int size, string &content) {
   if (currPos >= size) return currPos;

   string varname = "";
   string index = "";

   if (tokens[currPos].ttype != TokenType::ArrayAccess) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::ArrayAccess));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
      return -1;
   }

   varname += tokens[currPos].content;

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::IntLit
      && tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, "Valid array index");
      return -1;
   }

   index += tokens[currPos].content;

   content += varname + "[" + index + "]";

   return currPos;
}


// parse a struct build statement
int parseStructBuild(token tokens[], int currPos, int size, int indent) {
   if (currPos >= size) return currPos;

   string structtype = "";
   string structname = "";

   if (tokens[currPos].ttype != TokenType::StructBuild) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::StructBuild));
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, "Valid struct type");
      return -1;
   }

   structtype = tokens[currPos].content;

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, tokenTypeToString(TokenType::Identifier));
      return -1;
   }

   structname = tokens[currPos].content;

   printIndent(indent);
   cout << structtype << " " << structname << ";\n";
   return currPos;
}


// parse a struct set statement
int parseStructSet(token tokens[], int currPos, int size, int indent) {
   if (currPos >= size) return currPos;

   string structname = "";
   string element = "";
   string value = "";
   string op = "";

   if (tokens[currPos].ttype == TokenType::StructElemSet
      || tokens[currPos].ttype == TokenType::StructIndirElemSet) {
      op = tokenToCPPString(tokens[currPos].ttype);
   } else {
      printError(tokens[currPos], currPos, "Struct set operator");
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, "Struct identifier");
      return -1;
   }

   structname += tokens[currPos].content;

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, "Struct element identifier");
      return -1;
   }

   element += tokens[currPos].content;

   currPos++;
   if (currPos >= size) return size;

   currPos = parseExpression(tokens, currPos, size, value);

   if (currPos == -1) return -1;

   printIndent(indent);
   cout << structname << op << element << " = " << value << ";\n";

   return currPos;
}


// parse a struct access statement
int parseStructAccess(token tokens[], int currPos, int size, string &content) {
   if (currPos >= size) return currPos;

   string structname = "";
   string element = "";
   string op = "";

   if (tokens[currPos].ttype == TokenType::StructElemAccess
      || tokens[currPos].ttype == TokenType::StructIndirElemAccess) {
      op = tokenToCPPString(tokens[currPos].ttype);
   } else {
      printError(tokens[currPos], currPos, "Struct access operator");
      return -1;
   }

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, "Struct identifier");
      return -1;
   }

   structname += tokens[currPos].content;

   currPos++;
   if (currPos >= size) return size;

   if (tokens[currPos].ttype != TokenType::Identifier) {
      printError(tokens[currPos], currPos, "Struct element identifier");
      return -1;
   }

   element += tokens[currPos].content;

   content += structname + op + element;

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
      case VoidType:
         return "void";
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
      case AddAdd:
         return "++";
      case SubSub:
         return "--";
      case AddAddPre:
         return "++";
      case SubSubPre:
         return "--";
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
      case StructIndirElemAccess:
      case StructIndirElemSet:
      case StructElemAccess:
      case StructElemSet:
         return ".";
      case Main:
         return "\nint main()\n";
      case If:
         return "while";
      case Return:
         return "return";
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


// returns true if token is a valid type for a parameter
bool isParameterType(TokenType token) {
   return (token == TokenType::IntType
      || token == TokenType::RealType
      || token == TokenType::TextType
      || token == TokenType::BoolType
      || token == TokenType::Array
      || token == TokenType::StructType);
}


// returns true if the token is a valid type for a procedure return
bool isReturnType(TokenType token) {
   return (token == TokenType::IntType
      || token == TokenType::RealType
      || token == TokenType::TextType
      || token == TokenType::BoolType
      || token == TokenType::VoidType);
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


// returns true if the token is an increment operator
bool isIncrementOperator(TokenType token) {
   return (token == TokenType::AddAdd
      || token == TokenType::SubSub
      || token == TokenType::AddAddPre
      || token == TokenType::SubSubPre);
}


// returns true if the token is a postfix increment operator
bool isPostIncrementOperator(TokenType token) {
   return (token == TokenType::AddAdd
      || token == TokenType::SubSub);
}


// returns true if the token is a prefix increment operator
bool isPreIncrementOperator(TokenType token) {
   return (token == TokenType::AddAddPre
      || token == TokenType::SubSubPre);
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
