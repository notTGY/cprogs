bool eq(
  std::vector<std::vector<int>> a,
  std::vector<std::vector<int>> b
) {
  if (a.size() != b.size()) return false;

  for (int i = 0; i < a.size(); i++) {
    std::sort(a[i].begin(), a[i].end());
    std::sort(b[i].begin(), b[i].end());
  }

  for (int i = 0; i < a.size(); i++) {
    if (vecInVecSet(a[i], b) == -1) return false;
    if (vecInVecSet(b[i], a) == -1) return false;
  }
  return true;
}

// https://www.geeksforgeeks.org/minimization-of-dfa/
Automata* minimize(Automata* A) {
  std::vector<std::vector<int>> prevPartition = {};
  std::vector<std::vector<int>> partition = {{},{}};
  auto Q = A->Q;
  auto Q_f = A->Q_f;
  auto f = A->Q;

  for (int i = 0; i < Q_f.size(); i++)
    partition[0].push_back(Q_f[i]);
  for (int i = 0; i < Q.size(); i++)
    if (
      std::find(
        Q_f.begin(), Q_f.end(), Q[i]
      ) == Q_f.end()
    ) partition[1].push_back(Q[i]);
  
  while (!eq(prevPartition, partition)) {
    prevPartition = partition;
    partition = {};
    for (int i = 0; i < prevPartition.size(); i++) {
      auto set = prevPartition[i];
      for (int j = 0; j < set.size(); j++) {
        for (int k = j + 1; k < set.size(); k++) {
          // check if we need to split set[j] and set[k] into different sets
        }
      }
    }
  }

  return A;
}

