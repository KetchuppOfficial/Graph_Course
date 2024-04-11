#ifndef INCLUDE_GRAPH_TRAITS_HPP
#define INCLUDE_GRAPH_TRAITS_HPP

#include <ranges>

namespace graphs
{

// struct graph_traits must be specialized by different graph types

template<typename G>
requires std::ranges::forward_range<G>
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
 * using vertex_iterator - iterator over all the vertices, i.e.
 *     std::iterator_traits<vertex_iterator>::value_type == vertex_type
 *
 * using edge_iterator - iterator over range of iterators on vertices adjacent to some vertex, i.e.
 *     std::iterator_traits<edge_iterator>::value_type == vertex_iterator
 *
 * static size_type n_vertices(const G &g)
 *     - returns the number of nodes in the graph
 *
 * static size_type n_edges(const G &g)
 *     - returns the number of edges in the graph
 *
 * static std::pair<edge_iterator, edge_iterator> adjacent_vertices(const G &g, vertex_iterator i)
 *     - returns pair of iterators over a range of iterators over adjacent nodes of i
 *
 * static weight_type weight(const G &g, vertex_iterator from, vertex_iterator to)
 *     - returns weight of the edge connecting "from" with "to".
 */
};

} // namespace graphs

#endif // INCLUDE_GRAPH_TRAITS_HPP
