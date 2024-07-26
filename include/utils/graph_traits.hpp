#ifndef INCLUDE_UTILS_GRAPH_TRAITS_HPP
#define INCLUDE_UTILS_GRAPH_TRAITS_HPP

namespace graphs
{

// struct graph_traits must be specialized by different graph types

template<typename G> // G stands for "graph"
struct graph_traits
{
/*
 * Elements to provide:
 * ~~~~~~~~~~~~~~~~~~~~~~~
 *
 * using size_type - unsigned integer type
 *
 * using vertex_type - type of data contained in vertices
 *
 * using weight_type - type of edge weight
 *
 * static size_type n_vertices(const G &g)
 *     - returns the number of nodes in the graph
 *
 * static size_type n_edges(const G &g)
 *     - returns the number of edges in the graph
 *
 * static auto adjacent_vertices(const G &g, size_type i)
 *     - returns a range of indexes of adjacent nodes of the node with index i
 *
 * static const weight_type &weight(const G &g, size_type from, size_type to)
 *     - returns reference to const weight of the edge connecting nodes with indexes "from" and "to".
 */
};

} // namespace graphs

#endif // INCLUDE_UTILS_GRAPH_TRAITS_HPP
