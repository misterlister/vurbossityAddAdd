#include "tokenizing.h"


// read each word from standard input,
//    displaying error messages for invalid tokens encountered,
//    filling in the corresponding token information in the tokens array for valid tokens,
// and returning the number of valid tokens read
int tokenize(token tokens[])
{
   cout << "Beginning Scanning:\n" << endl;
   int pos = 0;
   string word = "";

   while (cin >> word) {
      if (pos >= MaxTokens) {
         return pos;
      }
      TokenType newTok = matchTokens(word);
         
      if (newTok == TokenType::Invalid) {
         // Print an error message for invalid tokens
         cout << "Invalid token: " << word << " found after token " << pos << endl;
      } else if (newTok == TokenType::Comment) {
         // Discard all input until the end of the line
         skipToEndline();
      } else if (newTok == TokenType::StartText || newTok == TokenType::LoneQuote) {
         // Gather up the full content of the text string
         string textString = word;

         if (!getTextString(textString)) {
            cout << "Non-terminated string: " << textString << " found after token " << pos << endl;
            return pos;
         }

         tokens[pos].ttype = TokenType::TextLit;
         tokens[pos].content = textString;
         tokens[pos].pos = pos;
         pos++;
      } else if (newTok == TokenType::EndText) {
         // Print an error message for incorrectly formatted text strings
         cout << "Improperly formatted string: " << word << " found after token " << pos << endl;
      } else {
         // Store valid tokens in the tokens array
         tokens[pos].ttype = newTok;
         tokens[pos].content = word;
         tokens[pos].pos = pos;
         pos++;
      }
   }
   return pos;
}

// display the token information for each token in the array
void printTokens(token tokens[], int size)
{
   if (!tokens) {
      cout << "Invalid token array" << endl;
      return;
   }

   if (size <= 0) {
      cout << "Invalid array size" << endl;
      return;
   }

   cout << "\nTotal valid tokens: " << size << endl;
   
   for (int i = 0; i < size; i++) {
      string tokenName = tokenTypeToString(tokens[i].ttype);
      string content = !tokens[i].content.empty() ? tokens[i].content : "null";
      cout << "Token " << i << ": " << tokenName << ", content: " << content << endl;
   }
   cout << "\n---end of tokens---" << endl;
}

// match an input string with the regex that corresponds to a TokenType
// If no match is found, then return Invalid
TokenType matchTokens(string input) {
   for (size_t i = 0; i < TokenRegex.size(); i++) {
      if (regex_match(input, TokenRegex[i].second)) {
         return TokenRegex[i].first;
      }
   }
   return TokenType::Invalid;
}

// Gathers and returns a text string from input
// Returns false if the text wasn't terminated correctly
bool getTextString(string &textString) {
   string word;
   TokenType newTok;
   while (cin >> word) {
      newTok = matchTokens(word);

      textString += " " + word;

      if (newTok == TokenType::EndText || newTok == TokenType::LoneQuote) {
         return true;
      }

   }
   return false;
}

// Ignores input until the next endline
void skipToEndline() {
   cin.ignore(10000, '\n');
}

// Takes a tokentype and returns a string that describes it
string tokenTypeToString(TokenType type) {
    auto it = TokenName.find(type);
    return (it != TokenName.end()) ? it->second : "Unknown";
}
