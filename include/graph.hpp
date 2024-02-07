#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <numeric>
#include <algorithm>
#include <memory>
#include <iterator>
#include <initializer_list>

namespace hisi
{

template<typename T>
class Directed_Graph final
{
    using node_type = T;

    using const_node_ptr = const node_type *;
    using nodes_cont = std::list<node_type>;
    using size_type = typename nodes_cont::size_type;
    using iterator = typename nodes_cont::iterator;
    using const_iterator = typename nodes_cont::const_iterator;

    nodes_cont nodes_;
    std::unordered_map<size_type, std::unordered_set<size_type>> adjacency_list_;

public:

    Directed_Graph() = default;

    Directed_Graph(std::initializer_list<node_type> il)
    {
        std::ranges::copy(il, std::back_inserter(nodes_));
    }

    // As far as adjacency list is totally based on indexes, the default copy- and move- constructor
    // and assignment are acceptable

    size_type n_nodes() const { return nodes_.size(); }
    size_type n_edges() const
    {
        return std::accumulate(adjacency_list_.begin(), adjacency_list_.end(), size_type{0},
                               [](size_type init, auto &elem){ return init + elem.second.size(); });
    }

    bool empty() const { return n_nodes() == 0; }

    iterator begin() { return nodes_.begin(); }
    const_iterator begin() const { return nodes_.begin(); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return nodes_.end(); }
    const_iterator end() const { return nodes_.end(); }
    const_iterator cend() const { return end(); }

    void insert_node(const node_type &node)
    {
        nodes_.emplace_back(node);
    }

    void insert_edge(const node_type &from, const node_type &to)
    {
        auto from_it = std::ranges::find(nodes_, from);
        if (from_it == end())
            return;

        auto to_it = std::ranges::find(nodes_, to);
        if (to_it == end())
            return;

        adjacency_list_[index_of(from_it)].insert(index_of(to_it));
    }

    void erase_node(const node_type &node)
    {
        auto it = std::ranges::find(nodes_, node);
        if (it == end())
            return;

        adjacency_list_.erase(index_of(it));
        nodes_.erase(it);
    }

    void erase_edge(const node_type& from, const node_type &to)
    {
        auto from_it = std::ranges::find(nodes_, from);
        if (from_it == end())
            return;

        auto to_it = std::ranges::find(nodes_, to);
        if (to_it == end())
            return;

        adjacency_list_[index_of(from_it)].erase(index_of(to_it));
    }

    bool are_adjacent(const node_type &from, const node_type &to) const
    {
        auto from_it = std::ranges::find(nodes_, from);
        if (from_it == end())
            return false;

        auto to_it = std::ranges::find(nodes_, to);
        if (to_it == end())
            return false;

        auto edges_it = adjacency_list_.find(index_of(from_it));
        if (edges_it == adjacency_list_.end())
            return false;

        auto &edges = edges_it->second;

        return edges.find(index_of(to_it)) != edges.end();
    }

    bool contains(const node_type &node) const
    {
        return std::ranges::find(nodes_, node) != end();
    }

private:

    size_type index_of(const_iterator it) const
    {
        return std::distance(begin(), it);
    }
};

} // namespace hisi

#endif // INCLUDE_GRAPH_HPP
