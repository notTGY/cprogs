#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

const int INF = 1e9; // infinity constant

struct Edge {
    int from, to, weight;
};

void distances(int n, vector<Edge>& edges) {
    int s = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<int> dist(n, INF);
    vector<int> prev(n, -1);

    dist[s] = 0;
    pq.push({0, s});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        for (auto& edge : edges) {
            if (edge.from == u && edge.to != s) {
                int newDist = min(dist[u], d) + edge.weight;
                if (newDist < dist[edge.to]) {
                    dist[edge.to] = newDist;
                    prev[edge.to] = u;
                    pq.push({newDist, edge.to});
                }
            } else if (edge.to == u && edge.from != s) {
                int newDist = min(dist[u], d) + edge.weight;
                if (newDist < dist[edge.from]) {
                    dist[edge.from] = newDist;
                    prev[edge.from] = u;
                    pq.push({newDist, edge.from});
                }
            }
        }
    }
    
    for (auto d : dist) {
        cout << d << endl;
    }
}

int main() {
    int n, m;
    cin >> n >> m;

    vector<Edge> edges(m);

    for (int i = 0; i < m; i++) {
        int from, to;
        cin >> from >> to;
        edges[i] = {from, to, 1};
    }

    distances(n, edges);

    return 0;
}