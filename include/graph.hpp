#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include <list>
#include <bit>
#include <unordered_set>
#include <initializer_list>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <utility>
#include <ostream>
#include <memory>
#include <unordered_map>

#include "graph_traits.hpp"

namespace graphs
{

// Note: the order of vertices is preserved; all new nodes are inserted at the end of graph's
//       internal container of vertices.

template<typename T>
class Directed_Graph final
{
    using vertex_cont = std::list<T>;

public:

    using vertex_type = T;
    using size_type = typename vertex_cont::size_type;
    using iterator = typename vertex_cont::iterator;
    using const_iterator = typename vertex_cont::const_iterator;
    using const_reference = const vertex_type &;

    struct iterator_hash final
    {
        std::size_t operator()(const_iterator it) const noexcept
        {
            return std::bit_cast<std::size_t>(it);
        }
    };

private:

    using edges_cont = std::unordered_set<const_iterator, iterator_hash>;

public:

    Directed_Graph() = default;

    Directed_Graph(std::initializer_list<vertex_type> il)
    {
        std::ranges::copy(il, std::back_inserter(vertices_));
        for (auto it = begin(), ite = end(); it != ite; ++it)
            adjacency_list_.emplace(it, edges_cont{});
    }

    Directed_Graph(const Directed_Graph &rhs) = delete;
    Directed_Graph &operator=(const Directed_Graph &rhs) = delete;

    Directed_Graph(Directed_Graph &&rhs) = default;
    Directed_Graph &operator=(Directed_Graph &rhs) = default;

    size_type n_vertices() const { return vertices_.size(); }
    size_type n_edges() const
    {
        return std::accumulate(adjacency_list_.begin(), adjacency_list_.end(), size_type{0},
                               [](size_type init, auto &elem){ return init + elem.second.size(); });
    }

    bool empty() const { return n_vertices() == 0; }

    iterator begin() { return vertices_.begin(); }
    const_iterator begin() const { return vertices_.begin(); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return vertices_.end(); }
    const_iterator end() const { return vertices_.end(); }
    const_iterator cend() const { return end(); }

    // Operations on vertices

    // O(1)
    iterator insert_vertex(const_reference v)
    {
        vertices_.emplace_back(v);
        auto vertex_it = std::prev(vertices_.end());
        adjacency_list_.emplace(vertex_it, edges_cont{});

        return vertex_it;
    }

    // O(V)
    void erase_vertex(const_iterator it)
    {
        for (auto &elem : adjacency_list_)
            elem.second.erase(it);

        adjacency_list_.erase(it);
        vertices_.erase(it);
    }

    // Operations on edges

    // O(1)
    void insert_edge(const_iterator from_it, const_iterator to_it)
    {
        adjacency_list_[from_it].insert(to_it);
    }

    // O(il.size())
    void insert_edges(std::initializer_list<std::pair<const_iterator, const_iterator>> il)
    {
        for (auto [from, to] : il)
            insert_edge(from, to);
    }

    // O(1)
    void erase_edge(const_iterator from_it, const_iterator to_it)
    {
        adjacency_list_[from_it].erase(to_it);
    }

    // Mixed operations

    // O(1)
    bool are_adjacent(const_iterator from_it, const_iterator to_it) const
    {
        auto &edges = adjacency_list_.at(from_it);
        return edges.find(to_it) != edges.end();
    }

    using edge_iterator = typename edges_cont::const_iterator;

    // O(1)
    std::pair<edge_iterator, edge_iterator> adjacent_vertices(const_iterator it) const
    {
        auto &edges = adjacency_list_.at(it);
        return std::pair{edges.begin(), edges.end()};
    }

    // O(V)
    size_type vertex_in_degree(const_iterator it) const
    {
        return std::ranges::count_if(adjacency_list_, [it](auto &elem)
        {
            return elem.second.contains(it);
        });
    }

    // O(1)
    size_type vertex_out_degree(const_iterator it) const { return adjacency_list_.at(it).size(); }

    // O(V)
    size_type vertex_degree(const_iterator it) const
    {
        return vertex_in_degree(it) + vertex_out_degree(it);
    }

    // graphic dump in dot format

    void graphic_dump(std::ostream &os)
    {
        os << "digraph G\n"
              "{\n";

        for (auto &node : vertices_)
            os << "    node_" << std::addressof(node) << " [label = \"" << node << "\"];\n";

        os << '\n';

        for (auto &[from_it, edges] : adjacency_list_)
            for (auto to_it : edges)
                os << "    node_" << std::addressof(*from_it)
                   << " -> node_" << std::addressof(*to_it) << ";\n";

        os << "}\n";
    }

private:

    vertex_cont vertices_;
    std::unordered_map<const_iterator,
                       edges_cont,
                       iterator_hash> adjacency_list_;
};

template<typename T>
struct graph_traits<Directed_Graph<T>>
{
    using size_type = typename Directed_Graph<T>::size_type;
    using vertex_type = T;
    using vertex_iterator = typename Directed_Graph<T>::const_iterator;
    using edge_iterator = typename Directed_Graph<T>::edge_iterator;

    static size_type n_edges(const Directed_Graph<T> &g) { return g.n_edges(); }
    static size_type n_vertices(const Directed_Graph<T> &g) { return g.n_vertices(); }

    static auto adjacent_vertices(const Directed_Graph<T> &g, vertex_iterator it)
           -> std::pair<edge_iterator, edge_iterator>
    {
        return g.adjacent_vertices(it);
    }

};

} // namespace graphs

#endif // INCLUDE_GRAPH_HPP
