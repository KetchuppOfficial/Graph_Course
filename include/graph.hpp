#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <numeric>
#include <algorithm>

namespace hisi
{

template<typename T>
class Directed_Graph final
{
    using node_type = T;
    using nodes_cont = std::vector<node_type>;
    using index_type = typename nodes_cont::size_type;

    nodes_cont nodes_;
    std::unordered_map<index_type, std::vector<index_type>> adjacency_list_;

public:

    Directed_Graph() = default;

    index_type n_nodes() const { return nodes_.size(); }
    index_type n_edges() const
    {
        return std::accumulate(adjacency_list_.begin(), adjacency_list_.end(), index_type{0},
                               [](index_type init, auto elem){ return init + elem.second.size(); });
    }

    void insert_edge(const node_type &from, const node_type &to)
    {
        auto from_it = std::ranges::find(nodes_, from);
        if (from_it == nodes_.end())
            return;

        auto to_it = std::ranges::find(nodes_, to);
        if (to_it == nodes_.end())
            return;

        adjacency_list_[node_index(from_it)].emplace_back(node_index(to_it));
    }

    void insert_node(const node_type &node)
    {
        nodes_.emplace_back(node);
    }

    bool are_adjacent(const node_type &from, const node_type &to) const
    {
        auto from_it = std::ranges::find(nodes_, from);
        if (from_it == nodes_.end())
            return false;

        auto edges_it = adjacency_list_.find(node_index(from_it));
        if (edges_it == adjacency_list_.end())
            return false;

        auto to_it = std::ranges::find(nodes_, to);
        if (to_it == nodes_.end())
            return false;

        auto &edges = edges_it->second;

        return std::ranges::find(edges, node_index(to_it)) != edges.end();
    }

private:

    index_type node_index(typename nodes_cont::const_iterator it) const
    {
        return std::distance(nodes_.begin(), it);
    }
};

} // namespace hisi

#endif // INCLUDE_GRAPH_HPP