#include<iostream>
#include<vector>
#include"Automata.cpp"
#include"AST.cpp"
using namespace std;

main(int argc, char** argv) {
  Automata A(
    { 'a' },
    { 0, 1 },
    0,
    { 0 },
    { { 1 }, { 0 } }
  );

  char* str = (char*)malloc(1024 * sizeof(char));
  cin >> str;
  cout << (A.test((const char*)str) ? "1\n" : "0\n");

  cout << A;

  return 0;
}
