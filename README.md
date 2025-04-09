# vurbossityAddAdd

A translator which converts the VurbossityAddAdd language to C++

## The Vurbossity Language

### Description

Vurbossity is a text-heavy language, generally favouring the use of words over symbols.
   (The only characters used in the language aside from whitespace are [a-z], [A-Z], [0-9],
    the decimal point [.] and the double-quote ["].)

It is strictly typed: every literal, variable, and parameter has a fixed type
   (integer, real, text, or boolean) with strict rules on mixed-type operations.
VurbossityAddAdd expands on the base Vurbossity language by adding the composite types: arrays and structures.

The expression syntax generally follows a fully-parenthesized prefix style
    (operation type then the operands, all within the left/right bracket keywords).

The language supports global and local variables but has no support for constants.

In addition to a (required) 'main' routine, the language supports procedures, which can take parameters, and return a primitive-typed value.

The language currently does not support any form of forward declaration for procedures.

The language supports only one kind of loop, the if loop, which is discussed in greater
    detail in the syntax and examples sections below

Comments begin with the COM keyword and continue to the end of the current line.
Language tokens (all case sensitive)

### Keywords

   begin end (used to mark the beginning and end of most blocks)
   pdef gdef sdef vdef (used to denote definitions of procedures, global vars, structs, local vars)
   if else (used for if/else loops)
   integer boolean real text void (type specifiers)
   add sub mul div rem (math operators)
   lt gt le ge ne eq (comparison operators)
   addadd subsub addaddpre subsubpre (increment operators)
   set (assignment operator)
   and or not (boolean operators)
   left right (used to bracket expressions)
   call (used to call a procedure)
   return (used to return a value from a procedure)
   arraydef arrayset arrayaccess array (used with arrays)
   structtype structbuild structelemset structindirelemset structelemaccess structindirelemaccess (used with structs)

### Literal values

   integer literals consist of one or more digits (0-9)
   real literals consist of one or more digits then a decimal point then one or more digits
   text literals begin and end with " and may have any other characters enclosed other than a "
   boolean literals consist of either "true" or "false

Identifiers:
   an alphabetic character followed by any sequence of alphabetic, numeric, or underscore characters (that are not a keyword)

### Syntax rules

#### Overall program structure

   The program structure has a fixed order for global components:
      1. zero or more global variable declarations
      2. zero or more procedure declarations
      3. exactly one main routine

#### Global variable declarations

   These begin with the "gdef" keyword followed by an identifier for the variable name
   (no two global variables can have the same name), followed by the type specifier
   (currently "text", "real", or "integer").


#### Procedure declarations

   These begin with the "pdef" keyword followed by an identifier for the procedure name
   (no two procedures can have the same name), followed by the formal parameter list and then
   a body block.
   If the parameter list includes a return type, then the body block must end with a return statement (The keyword "return" followed by an expression)


#### Formal parameter lists

   These begin with the "left" keyword, followed by zero or more pairs of data type and
   identifier (thus providing the data type of each parameter), ending with the "right" keyword.
   After this, a type specifier (integer, text, real, or boolean) may be added to indicate the return type
   If no return type is stated, then the procedure returns no value


#### Main routine

   This begins with the "main" token and is followed by a body block.


#### Body blocks (for procedures, main, and loops)

   These blocks start with the "begin" keyword and finish with the "end" keyword and
   may contain zero or more local statements.


### Local statement types

#### Local variable declarations

      These begin with the "vdef" keyword followed by an identifier for the variable name
      (no two variables can be declared in the same scope with the same name), followed by
      the type specifier ("text", "integer", "real", or "boolean").

#### Assignment statements

      These begin with the "set" keyword followed by an identifier naming the variable
      whose value is being set (the variable must have been previously declared in an
      accessible scope), then the expression specifying the value to be assigned.
      (See the "additional rules" section for type compatibility/casting.)

#### Output statements

      These begin with the "write" keyword followed by an expression whose evaluated
      result is the value is to be output.  (Note that expressions include things like
      stand-alone literals and identifiers.)

#### Input statements

      These begin with the "read" keyword followed by an identifier naming which variable
      is to be used as the storage location for the value read.  The variable must have
      been previously declared in an accessible scope.
      (During execution the user must enter a type-compatible value or the program will crash.)

#### Procedure calls

      These begin with the "call" keyword followed by an identifier naming the procedure
      to be called followed by the list of passed arguments (discussed below).  The number
      of values passed must exactly match the number of arguments the procedure is expecting,
      and the passed data types must be compatible with the declared parameter types (see the
      "Additional rules" below for type compatibility rules).

#### Parameter lists

      Parameter lists consist of the "left" keyword followed by zero or more expressions
      followed by the "right" keyword.

#### Expressions can be any one of the following

      A single literal value, or
      a single identifier (which must be accessible in the current scope), or
      a "left" keyword followed by an operator followed by one or two expressions
        (one expression for unary operators, two expressions for binary operators),
        followed by a "right" keyword
      Conditional expressions use the conditional operators (and, or, not, lt, gt, ge, le, ne, eq)
         while computational expressions use the sub, mul, div, add, and rem operators.
      Note that, since expressions are fully parenthesized, associativity and precedence
        rules are not required.

#### Binary operators for computation expressions

      "add" "sub" "div" "mul" "rem" (remainder, integer division)

#### Binary operators for condition expressions

      "eq" "ne" "lt" "le" "gt" "ge" (equals, not-equals, etc)
      "and", "or"

#### Unary operators for computation expressions

      neg     (e.g. left NEG x right to get -x)
      addadd (acts like the c++ '++' increment operator)
      subsub (acts like the c++ '--' decrement operator)
      addaddpre (acts like the c++ '++'  prefix increment operator)
      subsubpre (acts like the c++ '--' prefix decrement operator)


#### Unary operators for condition expressions

      not

   If loops:
      These consist of
       - the "if" keyword
       - then a condition expression
       - then a body
       - then the "else" keyword
       - then a body
   Note:
      The first if/condition/body operates like a conventional top-tested while loop,
         repeating as long as the condition is true.
      The else/body operates like a final/last clause: it is always executed exactly once,
         after the while test condition fails.

### Additional rules

#### Scoping rules

 - variables can be either global or local to body (a procedure's body or an if loop's body)
 - global variables are usable anywhere after their point of declaration
 - local variables are usable anywhere within the same body block after the point of their declaration
 - note that global variables can only be declared in the global space, locals only in body blocks

#### Type rules

 - every variable, parameter, and literal has a fixed type (integer, real, text, or bool)
 - expressions and assignment (set) statements that contain a mix of integer and real types are permitted, with the integer values automatically cast to reals, but no other type mixes are permitted
 - when assigning values the types of the left and right hand sides must match, with the exception that integer values will automatically be cast to reals if assigned to a real variable
 - similarly values passed to parameters must be of matching types with the exception of passing integers to reals (where again the integer value is automatically cast to real)

### Syntax

#### AddAdd

left addadd *identifier* right

left addaddpre *identifier* right

#### SubSub

left subsub *identifier* right

left subsubpre *identifier* right

#### Declare Return Type

pdef *identifier* left *paramtype* *paramidentifier* right *returntype*
begin
    return *value*
end

#### Use Return from Function

set *variableident* call *ident* left *params* right

### Data Structures

#### Structure Definition

sdef *identifier*
begin
    element *identifier* *type*
    element *identifier* *type*
end

#### Structure Initialization

structbuild *structIdentifier* *instanceIdentifier*

#### Structure Modify Element

structelemset *structIdentifier* *elementIdentifier* *value*

#### Structure Modify Element Via Pointer

structindirelemset *structIdentifier* *elementIdentifier* *value*

#### Structure Access Element

structelemaccess *structIdentifier* *elementIdentifier*

#### Structure Access Element Via Pointer

structindirelemaccess *structIdentifier* *elementIdentifier*

#### Structure As Procedure Parameter

pdef *identifier* left structtype *structidentifier* right *returntype*

#### Array Definition

*deftype* array *identifier* *type* *size*

#### Array Modify Element

arrayset *identifier* *index* *value*

#### Array Access Element

arrayaccess *identifier* *index*

#### Array As Procedure Parameter

pdef *identifier* left array *arraytype* *arrayidentifier* right *returntype*
