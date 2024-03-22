//=======================================================================
// Copyright 2007 Aaron Windsor
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
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

// This example shows how to start with a connected planar graph
// and add edges to make the graph maximal planar (triangulated.)
// Any maximal planar simple graph on n vertices has 3n - 6 edges and
// 2n - 4 faces, a consequence of Euler's formula.

using namespace boost;

// This visitor is passed to planar_face_traversal to count the
// number of faces.
struct face_counter : public planar_face_traversal_visitor
{
    face_counter() : count(0) {}
    void begin_face() { ++count; }
    int count;
};

using namespace std;

int main()
{

    typedef adjacency_list< vecS, vecS, undirectedS,
        property< vertex_index_t, int >, property< edge_index_t, int > >
        graph;

    // Create the graph - a straight line
    
    int V, E;
    cin >> V >> E;

    graph g(V);

    // Read edges from input file and add them to the graph
    for (int i = 0; i < E; ++i) {
        int s, e;
        cin >> s >> e;
        add_edge(s, e, g);
    }

    // Initialize the interior edge index
    property_map< graph, edge_index_t >::type e_index = get(edge_index, g);
    graph_traits< graph >::edges_size_type edge_count = 0;
    graph_traits< graph >::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        put(e_index, *ei, edge_count++);

    // Test for planarity; compute the planar embedding as a side-effect
    typedef std::vector< graph_traits< graph >::edge_descriptor > vec_t;
    std::vector< vec_t > embedding(num_vertices(g));
    if (boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
            boyer_myrvold_params::embedding = &embedding[0]))
        std::cout << "YES" << std::endl;
    else
        std::cout << "NO" << std::endl;

    return 0;
}
