#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <numeric>
#include <algorithm>
#include <memory>

namespace hisi
{

template<typename T>
class Directed_Graph final
{
    using node_type = T;
    using const_node_ptr = const node_type *;
    using nodes_cont = std::list<node_type>;
    using size_type = typename nodes_cont::size_type;
    using node_iterator = typename nodes_cont::iterator;

    nodes_cont nodes_;
    std::unordered_map<const_node_ptr, std::unordered_set<const_node_ptr>> adjacency_list_;

public:

    Directed_Graph() = default;

    size_type n_nodes() const { return nodes_.size(); }
    size_type n_edges() const
    {
        return std::accumulate(adjacency_list_.begin(), adjacency_list_.end(), size_type{0},
                               [](size_type init, auto &elem){ return init + elem.second.size(); });
    }

    bool empty() const { return n_nodes() == 0; }

    void insert_node(const node_type &node)
    {
        nodes_.emplace_back(node);
    }

    void insert_edge(const node_type &from, const node_type &to)
    {
        auto from_it = std::ranges::find(nodes_, from);
        if (from_it == nodes_.end())
            return;

        auto to_it = std::ranges::find(nodes_, to);
        if (to_it == nodes_.end())
            return;

        adjacency_list_[std::addressof(*from_it)].insert(std::addressof(*to_it));
    }

    void erase_node(const node_type &node)
    {
        auto it = std::ranges::find(nodes_, node);
        if (it == nodes_.end())
            return;

        adjacency_list_.erase(std::addressof(*it));
        nodes_.erase(it);
    }

    void erase_edge(const node_type& from, const node_type &to)
    {
        auto from_it = std::ranges::find(nodes_, from);
        if (from_it == nodes_.end())
            return;

        auto to_it = std::ranges::find(nodes_, to);
        if (to_it == nodes_.end())
            return;

        adjacency_list_[std::addressof(*from_it)].erase(std::addressof(*to_it));
    }

    bool are_adjacent(const node_type &from, const node_type &to) const
    {
        auto from_it = std::ranges::find(nodes_, from);
        if (from_it == nodes_.end())
            return false;

        auto to_it = std::ranges::find(nodes_, to);
        if (to_it == nodes_.end())
            return false;

        auto edges_it = adjacency_list_.find(std::addressof(*from_it));
        if (edges_it == adjacency_list_.end())
            return false;

        auto &edges = edges_it->second;

        return edges.find(std::addressof(*to_it)) != edges.end();
    }
};

} // namespace hisi

#endif // INCLUDE_GRAPH_HPP