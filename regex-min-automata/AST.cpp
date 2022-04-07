#include"AST.hpp"

/**
 * Global variable used for leaf numbers
 */
int pos;

vector<int> concat(vector<int> a, vector<int> b) {
  a.insert(a.end(), b.begin(), b.end());
  // required for making unique element
  sort(a.begin(), a.end());
  //for finding unique element within vector
  a.erase(unique(a.begin(), a.end()), a.end());
  return a;
}

AST* createLeaf(int val) {
  AST* leaf = new AST;
  leaf->type = leaf_node;
  leaf->value = val;
  leaf->fpos = { pos };
  leaf->lpos = { pos };
  leaf->nullable = false;
  pos++;
  return leaf;
}

AST* createHash() {
  AST* hash = new AST;
  hash->type = hash_node;
  hash->fpos = { pos };
  hash->lpos = { pos };
  hash->nullable = false;
  pos++;
  return hash;
}

AST* fromString(const char* str) {
  int len = strlen(str);
  if (len < 1) {
    cerr << "AST children can't have <1 lenght\n";
    return nullptr;
  } else if (len == 1) {
    return createLeaf(str[0]);
  } else if (str[len - 1] == '*') {
    if (len == 2) {
      char content[2] = { str[0], '\0' };
      return createStar(content);
    }
  }

  cerr << "AST unsupported format\n";
  return nullptr;
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

AST* fromREGEX(const char* regex) {
  pos = 0;
  return createCat(regex, "#", true);
}

