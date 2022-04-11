#include<iostream>
#include<vector>
#include"types.hpp"
#include"common.hpp"
#include"Automata.cpp"
#include"AST.cpp"

main(int argc, char** argv) {
  Automata A(
    { 'a' },
    { 0, 1 },
    0,
    { 0 },
    { { 1 }, { 0 } }
  );

  char* str = (char*)malloc(1024 * sizeof(char));
  // std::cin >> str;
  // std::cout << (A.test((const char*)str) ? "1\n" : "0\n");
  // std::cout << A << std::endl;

  std::cin >> str;

  AST* ast = fromREGEX(str);

  printAST(ast);

  Automata* B = fromAST(ast);

  return 0;
}
