#ifndef INCLUDE_SINGLE_SOURCE_SHORTEST_PATHS
#define INCLUDE_SINGLE_SOURCE_SHORTEST_PATHS

#include <optional>
#include <iterator>
#include <cstddef>
#include <bit>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "graph_traits.hpp"
#include "distance.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>>
class SSSP // single-source shortest paths
{
public:

    using distance_type = Distance<typename Traits::weight_type>;
    using vertex_iterator = typename Traits::vertex_iterator;

    struct Info_Node final
    {
        distance_type distance_;
        std::optional<vertex_iterator> predecessor_;

        Info_Node() = default;
        Info_Node(typename Traits::weight_type d) : distance_{d} {}
    };

protected:

    // No need in virtual destructor since the only constructor is protected
    SSSP(const G &g, vertex_iterator source_it)
    {
        info_.reserve(Traits::n_vertices(g));

        for (auto it = std::ranges::begin(g); it != source_it; ++it)
            info_.emplace(it, Info_Node{});

        info_.emplace(source_it, Info_Node{0});

        for (auto it = std::next(source_it), ite = std::ranges::end(g); it != ite; ++it)
            info_.emplace(it, Info_Node{});
    }

    struct iterator_hash final
    {
        std::size_t operator()(vertex_iterator it) const noexcept
        {
            return std::bit_cast<std::size_t>(it);
        }
    };

    using info_table_type = std::unordered_map<vertex_iterator,
                            Info_Node,
                            iterator_hash>;

public:

    using iterator = typename info_table_type::iterator;
    using const_iterator = typename info_table_type::const_iterator;

    iterator begin() { return info_.begin(); }
    const_iterator begin() const { return info_.begin(); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return info_.end(); }
    const_iterator end() const { return info_.end(); }
    const_iterator cend() const { return end(); }

    distance_type distance(vertex_iterator u_it) const
    {
        auto &info_node = info_.at(u_it);
        return info_node.distance_;
    }

    std::vector<vertex_iterator> path_to(vertex_iterator u_it) const
    {
        distance_type u_d = distance(u_it);
        if (u_d.is_inf())
            return {};

        std::vector<vertex_iterator> path_{u_it};

        auto it = u_it;
        auto predecessor = info_.find(it)->second.predecessor_;
        while (predecessor.has_value())
        {
            it = predecessor.value();
            predecessor = info_.find(it)->second.predecessor_;

            path_.push_back(it);
        }

        std::ranges::reverse(path_);

        return path_;
    }

protected:

    std::unordered_map<vertex_iterator,
                       Info_Node,
                       iterator_hash> info_;
};

} // namespace graphs

#endif // INCLUDE_SINGLE_SOURCE_SHORTEST_PATHS
