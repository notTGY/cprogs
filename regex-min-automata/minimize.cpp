#include<algorithm>
#include<iostream>
#include<vector>
#include"minimize.hpp"

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

bool areInSame(
  int a, int b, std::vector<std::vector<int>> dic
) {
  for (int i = 0; i < dic.size(); i++) {
    auto search_a = std::find(
      dic[i].begin(), dic[i].end(), a
    );
    auto search_b = std::find(
      dic[i].begin(), dic[i].end(), b
    );
    if (search_a != dic[i].end()) {
      if (search_b == dic[i].end()) return false;
    }
    if (search_b != dic[i].end()) {
      if (search_a == dic[i].end()) return false;
    }
  }
  return true;
}

void push(int item, std::vector<int>* set) {
  if (
    std::find(
      set->begin(), set->end(), item
    ) == set->end()
  ) set->push_back(item);
}

void pushIfFree(
  int item, std::vector<std::vector<int>> *dic
) {
  for (int i = 0; i < dic->size(); i++) {
    auto set = (*dic)[i];
    if (
      std::find(
        set.begin(), set.end(), item
      ) != set.end()
    ) return;
  }
  (*dic)[item].push_back(item);
}

void add(
  int item_a,
  int item_b,
  bool apart,
  std::vector<std::vector<int>> *dic
) {
  // if not merging
  if (apart) {
    pushIfFree(item_a, dic);
    pushIfFree(item_b, dic);
    return;
  }

  // if merging
  for (int i = 0; i < dic->size(); i++) {
    if (i == item_a) continue;
    auto set = (*dic)[i];
    if (
      std::find(
        set.begin(), set.end(), item_b
      ) != set.end()
    ) {
      // merge set into dic[item_a]
      for (int j = 0; j < set.size(); j++) {
        push(set[j], &((*dic)[item_a]));
      }
      set = {};
    }
  }
  pushIfFree(item_a, dic);
  pushIfFree(item_b, dic);
}

// https://www.geeksforgeeks.org/minimization-of-dfa/
Automata* minimize(Automata* A) {
  std::vector<std::vector<int>> prevPartition = {};
  std::vector<std::vector<int>> partition = {{},{}};
  auto T = A->T;
  auto Q = A->Q;
  auto Q_f = A->Q_f;
  auto f = A->f;

  for (int i = 0; i < Q_f.size(); i++)
    push(Q_f[i], &partition[0]);
  for (int i = 0; i < Q.size(); i++)
    if (
      std::find(
        Q_f.begin(), Q_f.end(), Q[i]
      ) == Q_f.end()
    ) push(Q[i], &partition[1]);
  
  while (!eq(prevPartition, partition)) {

  std::cout << "Partition:\n";
  for (int i = 0; i < partition.size(); i++) {
    std::cout << i << ":";
    auto set = partition[i];
    std::sort(set.begin(), set.end());
    for (int j = 0; j < set.size(); j++) {
      std::cout << " " << set[j];
    }
    std::cout << std::endl;
  }

    prevPartition = partition;
    partition = {};
    for (int i = 0; i < prevPartition.size(); i++) {
      auto set = prevPartition[i];
      std::vector<std::vector<int>> subset;

      if (set.size() == 1) {
        partition.push_back(set);
        continue;
      }

      subset.resize(Q.size());

      for (int j = 0; j < set.size(); j++) {
        for (int k = j + 1; k < set.size(); k++) {
          // check if we need to split set[j] and set[k] into different subsets
          bool areDistinguishable = false;
          for (int c = 0; c < T.size(); c++) {
            auto res_j = f[set[j]][c];
            auto res_k = f[set[k]][c];
            if (
              !areInSame(res_j, res_k, prevPartition)
            ) areDistinguishable = true;
          }
          add(set[j], set[k], areDistinguishable, &subset);
        }
      }
      // cleanup subset and push it into new partition
      for (int i = 0; i < subset.size(); i++) {
        auto set = subset[i];
        if (!set.empty()) partition.push_back(set);
      }
    }
  }

  return A;
}

