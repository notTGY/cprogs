
void printVec(std::ostream& stream, vector<int> v);

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

  static void printVec(
    ostream& stream, vector<int> v
  ) {
    stream << "[ ";
    for (int i = 0; i < v.size(); i++) {
      stream << v[i];
      if (i != v.size() - 1) stream << ',';
      stream << ' ';
    }
    stream << ']';
  }

  public:
    Automata(
      vector<int> __T,
      vector<int> __Q,
      int __q0,
      vector<int> __Q_f,
      vector<vector<int>> __f
    );

    bool test(const char* word);

    friend ostream& operator<< (
      ostream& stream, const Automata& a
    ) {
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

