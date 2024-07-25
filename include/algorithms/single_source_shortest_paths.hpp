#ifndef INCLUDE_ALGORITHMS_SINGLE_SOURCE_SHORTEST_PATHS
#define INCLUDE_ALGORITHMS_SINGLE_SOURCE_SHORTEST_PATHS

#include <optional>
#include <iterator>
#include <cstddef>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "utils/graph_traits.hpp"
#include "utils/distance.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
class SSSP // single-source shortest paths
{
protected:

    using size_type = typename Traits::size_type;
    using weight_type = typename Traits::weight_type;
    using distance_type = Distance<weight_type>;

    struct Info_Node final
    {
        distance_type distance;
        std::optional<size_type> predecessor;

        Info_Node() = default;
        Info_Node(weight_type d) : distance{d} {}
    };

    SSSP(const G &g, size_type source_i)
    {
        const size_type n_vertices = Traits::n_vertices(g);
        info_.reserve(n_vertices);

        for (auto i : std::views::iota(size_type{0}, source_i))
            info_.try_emplace(i);

        info_.try_emplace(source_i, 0);

        for (auto i : std::views::iota(source_i + 1, n_vertices))
            info_.try_emplace(i);
    }

    // No need in virtual destructor since the destructor is protected
    ~SSSP() = default;

public:

    distance_type distance(size_type u_i) const { return info_.at(u_i).distance; }

    std::vector<size_type> path_to(size_type u_i) const
    {
        distance_type u_d = distance(u_i);
        if (u_d.is_inf())
            return {};

        std::vector path{u_i};

        for (auto predecessor = info_.find(u_i)->second.predecessor; predecessor.has_value();)
        {
            u_i = *predecessor;
            predecessor = info_.find(u_i)->second.predecessor;

            path.push_back(u_i);
        }

        std::ranges::reverse(path);

        return path;
    }

protected:

    std::unordered_map<size_type, Info_Node> info_;
};

} // namespace graphs

#endif // INCLUDE_ALGORITHMS_SINGLE_SOURCE_SHORTEST_PATHS
