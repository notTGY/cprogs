#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>

#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>


using namespace std;
using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS, property<vertex_index_t, int>> Graph;



int main() {
    int V, E;
    cin >> V >> E;

    // Create graph with V vertices and no edges
    Graph g(V);

    // Read edges from input file and add them to the graph
    for (int i = 0; i < E; ++i) {
        int s, e;
        cin >> s >> e;
        add_edge(s, e, g);
    }

    // Perform Boyer-Myrvold planarity test
    typedef std::vector< graph_traits< Graph >::edge_descriptor > vec_t;
    vec_t embedding;
    bool is_planar = boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g, boyer_myrvold_params::embedding = &embedding);

    // Write result to output file
    cout << (is_planar ? "YES" : "NO") << endl;

    return 0;
}