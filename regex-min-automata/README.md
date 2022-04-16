# Minimal DFA from regex

## how to use

Compile code into executable like so:
```
g++ main.c
```

Then run program. Input regular expression (cin).
The output is json-compatible form of minimal dfa
resolving the regular language given.

## Project structure

### common
Common functions for both AST parsing and Automata
creation/execution are declared in `common.hpp` and
defined in `common.cpp`.
Those are just helper functions, which do not have any significant practical implications.

### AST
Ast parsing of regular expression is done in `AST.cpp`
(definitions in `AST.hpp`).
You can see there constructor functions like:
```
AST* createLeaf(int val);
AST* createHash();
AST* createStar(const char* content);
AST* createOr(const char* left, const char* right);
AST* createCat(
  const char* left,
  const char* right,
  bool isHash
);
AST* fromString(const char* str);

AST* fromREGEX(const char* regex);
```

Those functions recursively call each other.
`createString` determines sequence of nodes and
passes substrings into node-specific constructors.

There are also helper functions defined
(like followpos calculation) as well as `printAST`
used for debugging.

### Automata
In order to build and test DFA there are
`Automata.hpp` with definitions and `Automata.cpp`
with realisation of the Automata class.
It has method `test` to check whether
word fits in the language or not.
There is also `match` method that can be used in
real-world applications to search for text (as an 
example).

### Minimization
Minimization is done in `minimize.hpp` and `minimize.cpp`.
There is also function `minFromREGEX` to surpass whole process
of permutations and get minimal DFA straight after
passing in `const char*`.

## Scientific reference

* converting regex to DFA - https://www.geeksforgeeks.org/regular-expression-to-dfa/
* minimization algorithm - https://geeksforgeeks.org/minimization-of-dfa
* vector concatenation function - https://github.com/ckshitij/RE_TO_DFA/blob/master/RE_TO_DFA.cpp#L14

