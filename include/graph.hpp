#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include <unordered_map>
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
    using nodes_cont = std::list<node_type>;
    using size_type = typename nodes_cont::size_type;
    using node_iterator = typename nodes_cont::iterator;

    nodes_cont nodes_;
    std::unordered_map<const node_type *, std::list<node_iterator>> adjacency_list_;

public:

    Directed_Graph() = default;

    size_type n_nodes() const { return nodes_.size(); }
    size_type n_edges() const
    {
        return std::accumulate(adjacency_list_.begin(), adjacency_list_.end(), index_type{0},
                               [](index_type init, auto elem){ return init + elem.second.size(); });
    }

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

        adjacency_list_[std::addressof(*from_it)].emplace_back(to_it);
    }

    bool are_adjacent(const node_type &from, const node_type &to) const
    {
        auto from_it = std::ranges::find(nodes_, from);
        if (from_it == nodes_.end())
            return false;

        auto edges_it = adjacency_list_.find(std::addressof(*from_it));
        if (edges_it == adjacency_list_.end())
            return false;

        auto to_it = std::ranges::find(nodes_, to);
        if (to_it == nodes_.end())
            return false;

        auto &edges = edges_it->second;

        return std::ranges::find(edges, to_it) != edges.end();
    }
};

} // namespace hisi

#endif // INCLUDE_GRAPH_HPP