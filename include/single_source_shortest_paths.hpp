#ifndef INCLUDE_SINGLE_SOURCE_SHORTEST_PATHS
#define INCLUDE_SINGLE_SOURCE_SHORTEST_PATHS

#include <optional>
#include <iterator>
#include <cstddef>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "graph_traits.hpp"
#include "distance.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
requires std::ranges::forward_range<G>
class SSSP // single-source shortest paths
{
public:

    using distance_type = Distance<typename Traits::weight_type>;

protected:

    using vertex_iterator = typename Traits::vertex_iterator;
    using iterator_hash = typename Traits::iterator_hash;

    struct Info_Node final
    {
        distance_type distance_;
        std::optional<vertex_iterator> predecessor_;

        Info_Node() = default;
        Info_Node(typename Traits::weight_type d) : distance_{d} {}
    };

    // No need in virtual destructor since the only constructor is protected
    SSSP(const G &g, vertex_iterator source_it)
    {
        info_.reserve(Traits::n_vertices(g));

        for (auto it = std::ranges::begin(g); it != source_it; ++it)
            info_.try_emplace(it);

        info_.try_emplace(source_it, 0);

        for (auto it = std::next(source_it), ite = std::ranges::end(g); it != ite; ++it)
            info_.try_emplace(it);
    }

public:

    distance_type distance(vertex_iterator u_it) const { return info_.at(u_it).distance_; }

    std::vector<vertex_iterator> path_to(vertex_iterator u_it) const
    {
        distance_type u_d = distance(u_it);
        if (u_d.is_inf())
            return {};

        std::vector path{u_it};

        for (auto predecessor = info_.find(u_it)->second.predecessor_; predecessor.has_value();)
        {
            u_it = predecessor.value();
            predecessor = info_.find(u_it)->second.predecessor_;

            path.push_back(u_it);
        }

        std::ranges::reverse(path);

        return path;
    }

protected:

    std::unordered_map<vertex_iterator,
                       Info_Node,
                       iterator_hash> info_;
};

} // namespace graphs

#endif // INCLUDE_SINGLE_SOURCE_SHORTEST_PATHS
