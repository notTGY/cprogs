#include<algorithm>
#include<iostream>
#include<cstring>
#include"AST.hpp"

/**
 * Global variable used for leaf numbers
 */
int pos;

std::vector<int> concat(
  std::vector<int> a, std::vector<int> b
) {
  if (a.empty()) return b; 
  if (b.empty()) return a; 
  a.insert(a.end(), b.begin(), b.end());
  // required for making unique element
  std::sort(a.begin(), a.end());
  //for finding unique element within vector
  a.erase(std::unique(a.begin(), a.end()), a.end());
  return a;
}

AST* createLeaf(int val) {
  AST* leaf = new AST;
  std::vector<int>fpos { pos };
  std::vector<int>lpos { pos };

  leaf->type = leaf_node;
  leaf->value = val;
  leaf->fpos = fpos;
  leaf->lpos = lpos;
  leaf->nullable = false;

  pos++;
  return leaf;
}

AST* createHash() {
  AST* hash = new AST;
  std::vector<int>fpos { pos };
  std::vector<int>lpos { pos };

  hash->type = hash_node;
  hash->fpos = fpos;
  hash->lpos = lpos;
  hash->nullable = false;

  pos++;
  return hash;
}

AST* createCat(
  const char* left,
  const char* right,
  bool isHash
) {
  AST* node = new AST;
  node->type = cat_node;

  AST* leftChild = fromString(left);
  AST* rightChild = (
    isHash
      ? createHash()
      : fromString(right)
  );

  node->left = leftChild;
  node->right = rightChild;
  
  node->fpos = (
    leftChild->nullable
      ? concat(leftChild->fpos, rightChild->fpos)
      : leftChild->fpos
  );
  node->lpos = (
    rightChild->nullable
      ? concat(leftChild->lpos, rightChild->lpos)
      : rightChild->lpos
  );
  node->nullable = (
    leftChild->nullable && rightChild->nullable
  );

  return node;
}

AST* createOr(const char* left, const char* right) {
  AST* node = new AST;
  node->type = or_node;

  AST* leftChild = fromString(left);
  AST* rightChild = fromString(right);

  node->left = leftChild;
  node->right = rightChild;
  
  node->fpos = concat(
    leftChild->fpos, rightChild->fpos
  );
  node->lpos = concat(
    leftChild->lpos, rightChild->lpos
  );
  node->nullable = leftChild->nullable
    || rightChild->nullable;

  return node;
}

AST* createStar(const char* content) {
  AST* node = new AST;
  node->type = star_node;

  AST* child = fromString(content);

  node->left = child;
  
  node->fpos = child->fpos;
  node->lpos = child->lpos;
  node->nullable = true;

  return node;
}

AST* fromString(const char* str) {
  int len = strlen(str);

  if (len < 1) {
    std::cerr << "AST children can't have <1 length\n";
    return nullptr;
  }

  if (len == 1) { return createLeaf(str[0]); }

  char* left = new char[len];
  char right[2] = { str[len - 1], '\0' };
  memcpy((void *)left, (const void *)str, len - 1);
  left[len - 1] = '\0';
  return createCat(left, right, false);
}

AST* fromREGEX(const char* regex) {
  pos = 0;
  return createCat(regex, "#", true);
}

void printAST(AST* ast) {
  std::cout << "type: " << ast->type;
  if (ast->type == leaf_node) std::cout << " value: " << ast->value;
  std::cout << " fpos: ";
  for (int i = 0; i < ast->fpos.size(); i++) std::cout << ast->fpos[i] << " ";
  std::cout << " lpos: ";
  for (int i = 0; i < ast->lpos.size(); i++) std::cout << ast->lpos[i] << " ";
  std::cout << std::endl;
  if (ast->type == cat_node) {
    printAST(ast->left);
    printAST(ast->right);
  }
}

std::vector<int> alphabet(AST* ast) {
  if (ast->type == leaf_node) {
    std::vector<int> ret { ast->value };
    return ret;
  }

  if (ast->left && ast->right)
    return concat(
      alphabet(ast->left),
      alphabet(ast->right)
    );
  if (ast->left) return alphabet(ast->left);

  std::vector<int> empty;
  return empty;
}

void putinmap(std::vector<int> *map, AST* ast) {
  if (ast->type == leaf_node) {
    (*map)[ast->fpos[0]] = ast->value;
  }
  if (ast->left) putinmap(map, ast->left);
  if (ast->right) putinmap(map, ast->right);
  return;
}

std::vector<int> CharValueMap(AST* ast) {
  std::vector<int> map;
  map.resize(ast->lpos[0]);
  putinmap(&map, ast);
  return map;
}

void computeFpos(
  std::vector<std::vector<int>> *followpos, AST* ast
) {
  auto left = ast->left;
  auto right = ast->right;
  if (ast->type == cat_node) {
    for (int i = 0; i < left->lpos.size(); i++) {
      int j = left->lpos[i];
      (*followpos)[j] = concat(
        (*followpos)[j], right->fpos
      );
    }
    if (left) computeFpos(followpos, left);
    if (right) computeFpos(followpos, right);
  }
  return;
}

std::vector<std::vector<int>> FollowPos(AST* ast) {
  std::vector<std::vector<int>> fpos;
  fpos.resize(ast->lpos[0]);
  for (int i = 0; i < fpos.size(); i++) fpos[i] = {};
  computeFpos(&fpos, ast);
  return fpos;
}

