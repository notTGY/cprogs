#include<algorithm>
#include<iostream>
#include<string>
#include<vector>
using namespace std;

void printVec(ostream& stream, vector<int> v) {
  stream << "[ ";
  for (int i = 0; i < v.size(); i++) {
    stream << v[i];
    if (i != v.size() - 1) stream << ',';
    stream << ' ';
  }
  stream << ']';
}

vector<int> concat(vector<int> u, vector<int> v) {
  return u;
}

// https://www.geeksforgeeks.org/regular-expression-to-dfa/
enum ASTNodeType {
  cat_node, or_node, star_node, leaf_node
};

typedef struct {
  ASTNodeType type,
  int value,
  AST* left,
  AST* right,
} AST;

Automata fromREGEX(const char* regex) {
}

class Automata {
  // here *int* means char (hack for less fns)
  vector<int> T;
  vector<int> Q;
  int q0;
  vector<int> Q_f;
  /**
   * f[current_state][letter_idx] == new_state
   *
   * all missing edges point towards -1
   */
  vector<vector<int>> f;

  public:
    Automata(
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

    bool test(const char* word) {
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
      auto search = find(Q_f.begin(), Q_f.end(), q_cur);
      return search != Q_f.end();
    }

    friend ostream& operator<< (ostream& stream, const Automata& a) {
      auto T = a.T;
      auto Q = a.Q;
      auto q0 = a.q0;
      auto Q_f = a.Q_f;
      auto f = a.f;

      stream << "{" << endl;

      stream << "\tT: ";
      printVec(stream, T);
      stream << "," << endl;

      stream << "\tQ: ";
      printVec(stream, Q);
      stream << "," << endl;

      stream << "\tq0: " << q0 << "," << endl;

      stream << "\tQ_f: ";
      printVec(stream, Q_f);
      stream << "," << endl;

      stream << "\tf: {" << endl;
      for (int i = 0; i < f.size(); i++) {
        stream << "\t\t";
        printVec(stream, f[i]);
        stream << "," << endl;
      }
      stream << "\t}" << endl;

      stream << "}";
      return stream;
    }
};


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
  cout << (A.test((const char*)str) ? "in" : "out") << endl;

  cout << A;

  return 0;
}
