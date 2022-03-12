#include<iostream>
#include<vector>
#include<algorithm>

using namespace std;

vector<int> T;
vector<int> T_full;
vector<bool> passed;

vector<pair<int, int>> B;


int f(int i) {
  vector<int> c;
  passed[i] = true;
  if (T_full[i]) return T_full[i];

  for (auto p : B) {
    if (p.second - 1 == i) {
      c.push_back(f(p.first - 1));
    }
  }
  
  if (c.empty()) {
    T_full[i] = T[i];
  } else {
    T_full[i] = *max_element(c.begin(), c.end()) + T[i];
  }
  return T_full[i];
}

int main() {
  int N, M;

  cin >> N;
  
  T.resize(N);
  T_full.resize(N);
  passed.resize(N);
  
  for (int i = 0; i < N; i++) {
    cin >> T[i];
    passed[i] = false;
  }

  cin >> M;

  if (N == 0) cout << 0;

  B.resize(M);
  
  for (int i = 0; i < M; i++) {
    pair<int, int> p;
    cin >> p.first >> p.second;
    B[i] = p;
  }

  for (int i = N - 1; i >= 0; i--) {
    if (!passed[i]) T_full[i] = f(i);
  }

  cout << *max_element(T_full.begin(), T_full.end());
  
  return 0;
}