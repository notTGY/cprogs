#include<algorithm>
#include<iostream>
#include<vector>

using namespace std;

#include"Automata.hpp"

Automata::Automata(
  vector<int> __T,
  vector<int> __Q,
  int __q0,
  vector<int> __Q_f,
  vector<vector<int>> __f
) {
  T = __T;
  Q = __Q;
  q0 = __q0;
  Q_f = __Q_f;
  f = __f;
}

bool Automata::test(const char* word) {
  int q_cur = q0;
  int i = 0;
  char c;
  while (c = word[i++]) {
    // 1. find corresponding charcode in alphabet
    auto search = find(T.begin(), T.end(), c);
    if (search == T.end()) return false;
    int charcode = distance(T.begin(), search);

    // 2. check if curstate has edge with that char
    if (f.size() < q_cur) return false;
    if (f[q_cur].size() < charcode) return false;
    q_cur = f[q_cur][charcode];
    if (q_cur < 0) return false;
  }

  // 3. check if we are in final state
  auto search = find(
    Q_f.begin(), Q_f.end(), q_cur
  );
  return search != Q_f.end();
}

