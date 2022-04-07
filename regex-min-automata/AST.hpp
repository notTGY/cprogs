#include<iostream>
#include<vector>
#include<string.h>

// https://www.geeksforgeeks.org/regular-expression-to-dfa/
enum ASTNodeType {
  cat_node, or_node, star_node, leaf_node, hash_node,
};

// https://github.com/ckshitij/RE_TO_DFA/blob/master/RE_TO_DFA.cpp#L14
vector<int> concat(vector<int> a, vector<int> b);

typedef struct AST {
  ASTNodeType type;
  int value;
  struct AST *left, *right;
  vector<int> fpos, lpos;
  bool nullable;
} AST;

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

