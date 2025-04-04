#include "tokenizing.h"
#include "parsing.h"

int main()
{
   token tokens[MaxTokens];
   int numTokens;

   numTokens = tokenize(tokens);
   parse(tokens, numTokens);
}
