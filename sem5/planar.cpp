#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// Количество вершин и рёбер в графе
int V, E;

// Матрица смежности графа
bool adjacencyMatrix[1000][1000];

// Стек для хранения вершин
vector<int> stack;

// Функция для проверки плаnarности графа
bool isPlanar() {
    // Initialize queue with all vertices
    queue<int> q;
    for (int i = 0; i < V; i++) {
        q.push(i);
    }

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        // If v has no neighbors, skip it
        if (adjacencyMatrix[v][0] == 0) continue;

        // Mark all neighbors of v as visited
        for (int i = 1; i <= E; i++) {
            if (adjacencyMatrix[v][i]) {
                adjacencyMatrix[v][i] = false;
                q.push(i);
            }
        }
    }

    return !stack.empty();
}

int main() {
    cin >> V >> E;

    // Initialize adjacency matrix
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 1000; j++) {
            adjacencyMatrix[i][j] = false;
        }
    }

    // Read graph edges
    for (int i = 0; i < E; i++) {
        int u, v;
        cin >> u >> v;
        adjacencyMatrix[u][v] = true;
    }

    // Check planarity
    if (isPlanar()) {
        cout << "YES\n";
    } else {
        cout << "NO\n";
    }

    return 0;
}