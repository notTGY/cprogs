#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

const int INF = 1e9; // infinity constant

struct Edge {
    int from, to, weight;
};

void dijkstra(int n, vector<Edge>& edges, int s, int f) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<int> dist(n, INF);
    vector<int> prev(n, -1);

    dist[s] = 0;
    pq.push({0, s});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (u == f) break;

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

    int res = dist[f];

    vector<int> path;
    
    if (res == INF) {
        cout << "No path found." << endl;
    } else {
        int curr = f;
        while (curr != -1) {
            path.push_back(curr);
            curr = prev[curr];
        }
        reverse(path.begin(), path.end());
        for (auto p : path) {
          cout << p << " ";
        }
        cout << endl;
    }

}

int main() {
    int n, m, s, f;
    cin >> n >> m >> s >> f;

    vector<Edge> edges(m);

    for (int i = 0; i < m; i++) {
        int from, to, weight;
        cin >> from >> to >> weight;
        edges[i] = {from, to, weight};
    }

    dijkstra(n, edges, s, f);

    return 0;
}