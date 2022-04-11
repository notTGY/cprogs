class Automata {
  // here *int* means char (hack for less fns)
  std::vector<int> T;
  std::vector<int> Q;
  int q0;
  std::vector<int> Q_f;
  /**
   * f[current_state][letter_idx] == new_state
   *
   * all missing edges point towards -1
   */
  std::vector<std::vector<int>> f;

  public:
    Automata(
      std::vector<int> __T,
      std::vector<int> __Q,
      int __q0,
      std::vector<int> __Q_f,
      std::vector<std::vector<int>> __f
    );

    bool test(const char* word);

    static void printVec(
      std::ostream& stream, std::vector<int> v
    ) {
      stream << "[ ";
      for (int i = 0; i < v.size(); i++) {
        stream << v[i];
        if (i != v.size() - 1) stream << ',';
        stream << ' ';
      }
      stream << ']';
    }

    friend std::ostream& operator<< (
      std::ostream& stream, const Automata& a
    ) {
      auto T = a.T;
      auto Q = a.Q;
      auto q0 = a.q0;
      auto Q_f = a.Q_f;
      auto f = a.f;

      stream << "{" << std::endl;

      stream << "\tT: ";
      printVec(stream, T);
      stream << "," << std::endl;

      stream << "\tQ: ";
      printVec(stream, Q);
      stream << "," << std::endl;

      stream << "\tq0: " << q0 << "," << std::endl;

      stream << "\tQ_f: ";
      printVec(stream, Q_f);
      stream << "," << std::endl;

      stream << "\tf: {" << std::endl;
      for (int i = 0; i < f.size(); i++) {
        stream << "\t\t";
        printVec(stream, f[i]);
        stream << "," << std::endl;
      }
      stream << "\t}" << std::endl;

      stream << "}";
      return stream;
    }
};

Automata* fromAST(AST* ast);

