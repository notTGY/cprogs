#include<vector>

// https://www.geeksforgeeks.org/regular-expression-to-dfa/
enum ASTNodeType {
  cat_node, or_node, star_node, leaf_node, hash_node,
};

// https://github.com/ckshitij/RE_TO_DFA/blob/master/RE_TO_DFA.cpp#L14
std::vector<int> concat(
  std::vector<int> a,
  std::vector<int> b
);

typedef struct AST {
  ASTNodeType type;
  int value;
  struct AST *left, *right;
  std::vector<int> fpos, lpos;
  bool nullable;
} AST;

