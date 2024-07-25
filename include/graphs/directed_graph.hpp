#ifndef INCLUDE_GRAPHS_DIRECTED_GRAPH_HPP
#define INCLUDE_GRAPHS_DIRECTED_GRAPH_HPP

#include <cstddef>
#include <list>
#include <iterator>
#include <algorithm>
#include <ranges>
#include <initializer_list>
#include <numeric>
#include <utility>
#include <tuple>
#include <ostream>
#include <unordered_map>
#include <unordered_set>

#include <boost/container_hash/hash.hpp>

#include "utils/graph_traits.hpp"

namespace graphs
{

// Note: the order of vertices is preserved; all new vertices are inserted at the end of graph's
//       internal container.

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
    using weight_type = int;

    static constexpr weight_type default_weight = 1;

    Directed_Graph() = default;

    template<std::input_iterator It>
    Directed_Graph(It first, It last)
    {
        std::ranges::copy(first, last, std::back_inserter(vertices_));
        for (auto i : std::views::iota(size_type{0}, n_vertices()))
            adjacency_list_.try_emplace(i);
    }

    Directed_Graph(std::initializer_list<vertex_type> il) : Directed_Graph(il.begin(), il.end()) {}

    size_type n_vertices() const { return vertices_.size(); }
    size_type n_edges() const
    {
        return std::accumulate(adjacency_list_.begin(), adjacency_list_.end(), size_type{0},
                               [](size_type init, auto &elem){ return init + elem.second.size(); });
    }

    bool empty() const { return n_vertices() == 0; }

    void clear()
    {
        vertices_.clear();
        adjacency_list_.clear();
    }

    iterator begin() { return vertices_.begin(); }
    const_iterator begin() const { return vertices_.begin(); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return vertices_.end(); }
    const_iterator end() const { return vertices_.end(); }
    const_iterator cend() const { return end(); }

    // Operations on vertices

    // O(1)
    size_type insert_vertex(const_reference v)
    {
        vertices_.emplace_back(v);
        const size_type vertex_i = n_vertices() - 1;
        adjacency_list_.try_emplace(vertex_i);

        return vertex_i;
    }

    // O(V)
    void erase_vertex(size_type vertex_i)
    {
        for (auto &elem : adjacency_list_)
            elem.second.erase(vertex_i);

        adjacency_list_.erase(vertex_i);
        vertices_.erase(std::next(vertices_.begin(), vertex_i));
    }

    // Operations on edges

    // O(1)
    void insert_edge(size_type from_i, size_type to_i, weight_type w = default_weight)
    {
        adjacency_list_[from_i].insert(to_i);
        weights_.emplace(std::pair{from_i, to_i}, w);
    }

    // O(il.size())
    void insert_edges(std::initializer_list<std::tuple<size_type, size_type, weight_type>> il)
    {
        for (const auto &[from, to, w] : il)
            insert_edge(from, to, w);
    }

    // O(il.size())
    void insert_edges(std::initializer_list<std::pair<size_type, size_type>> il)
    {
        for (const auto &[from, to] : il)
            insert_edge(from, to);
    }

    // O(1)
    void erase_edge(size_type from_i, size_type to_i)
    {
        adjacency_list_[from_i].erase(to_i);
        weights_.erase(std::pair{from_i, to_i});
    }

    // O(1)
    weight_type weight(size_type from_i, size_type to_i) const
    {
        return weights_.at(std::pair{from_i, to_i});
    }

    // O(1)
    void change_weight(size_type from_i, size_type to_i, weight_type new_w)
    {
        weights_.at(std::pair{from_i, to_i}) = new_w;
    }

    // Mixed operations

    // O(1)
    bool are_adjacent(size_type from_i, size_type to_i) const
    {
        return adjacency_list_.at(from_i).contains(to_i);
    }

    // O(1)
    auto adjacent_vertices(size_type vertex_i) const
        -> std::ranges::subrange<typename std::unordered_set<size_type>::const_iterator>
    {
        return adjacency_list_.at(vertex_i);
    }

    // O(V)
    std::size_t vertex_in_degree(size_type vertex_i) const
    {
        return std::ranges::count_if(adjacency_list_, [vertex_i](auto &elem)
        {
            return elem.second.contains(vertex_i);
        });
    }

    // O(1)
    std::size_t vertex_out_degree(size_type vertex_i) const
    {
        return adjacency_list_.at(vertex_i).size();
    }

    // O(V)
    size_type vertex_degree(size_type vertex_i) const
    {
        return vertex_in_degree(vertex_i) + vertex_out_degree(vertex_i);
    }

    // graphic dump in dot format

    void graphic_dump(std::ostream &os) const
    {
        os << "digraph G\n"
              "{\n";

        auto it = begin();
        for (auto i : std::views::iota(size_type{0}, n_vertices()))
            std::println(os, "    node_{} [label = \"{}\"];", i, *it++);

        os << '\n';

        for (auto &[from_i, edges] : adjacency_list_)
            for (auto to_i : edges)
                std::println(os, "    node_{} -> node_{} [label = \"{}\"];", from_i, to_i,
                             weights_.at(std::pair{from_i, to_i}));

        os << "}\n";
    }

private:

    vertex_cont vertices_;
    std::unordered_map<size_type,
                       std::unordered_set<size_type>> adjacency_list_;

    std::unordered_map<std::pair<size_type, size_type>,
                       weight_type,
                       boost::hash<std::pair<size_type, size_type>>> weights_;
};

template<std::input_iterator It> Directed_Graph(It first, It last)
    -> Directed_Graph<typename std::iterator_traits<It>::value_type>;

template<typename T>
struct graph_traits<Directed_Graph<T>>
{
    using vertex_type = T;
    using size_type = typename Directed_Graph<T>::size_type;
    using weight_type = typename Directed_Graph<T>::weight_type;

    static size_type n_edges(const Directed_Graph<T> &g) { return g.n_edges(); }
    static size_type n_vertices(const Directed_Graph<T> &g) { return g.n_vertices(); }

    static auto adjacent_vertices(const Directed_Graph<T> &g, size_type vertex_i)
    {
        return g.adjacent_vertices(vertex_i);
    }

    static weight_type weight(const Directed_Graph<T> &g, size_type from, size_type to)
    {
        return g.weight(from, to);
    }
};

} // namespace graphs

#endif // INCLUDE_GRAPHS_DIRECTED_GRAPH_HPP
