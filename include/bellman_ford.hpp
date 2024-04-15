#ifndef INCLUDE_BELLMAN_FORD_HPP
#define INCLUDE_BELLMAN_FORD_HPP

#include <iterator>
#include <ranges>

#include "graph_traits.hpp"
#include "single_source_shortest_paths.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>>
class Bellman_Ford final : public SSSP<G, Traits>
{
    using sssp = SSSP<G, Traits>;
    using sssp::info_;

public:

    using distance_type = typename sssp::distance_type;
    using vertex_iterator = typename sssp::vertex_iterator;
    using Info_Node = typename sssp::Info_Node;
    using iterator = typename sssp::iterator;
    using const_iterator = typename sssp::iterator;

    Bellman_Ford(const G &g, vertex_iterator source_it) : sssp{g, source_it}
    {
        auto n_vertices = Traits::n_vertices(g);
        auto begin = std::ranges::begin(g);
        auto end = std::ranges::end(g);

        for (auto i = 0; i != n_vertices - 1; ++i)
        {
            for (auto u_it = begin; u_it != end; ++u_it)
            {
                Info_Node &u_info = info_.find(u_it)->second;

                auto [adj_begin, adj_end] = Traits::adjacent_vertices(g, u_it);
                for (auto v_it : std::ranges::subrange(adj_begin, adj_end))
                {
                    Info_Node &v_info = info_.find(v_it)->second;

                    if (distance_type d = u_info.distance_ + Traits::weight(g, u_it, v_it);
                        d < v_info.distance_)
                    {
                        v_info.distance_ = d;
                        v_info.predecessor_ = u_it;
                    }
                }
            }
        }

        for (auto u_it = begin; u_it != end; ++u_it)
        {
            Info_Node &u_info = info_.find(u_it)->second;

            auto [adj_begin, adj_end] = Traits::adjacent_vertices(g,u_it);
            for (auto v_it : std::ranges::subrange(adj_begin, adj_end))
            {
                Info_Node &v_info = info_.find(v_it)->second;

                if (v_info.distance_ > u_info.distance_ + Traits::weight(g, u_it, v_it))
                {
                    info_.clear();
                    negative_cycle_ = true;
                    return;
                }
            }
        }

        negative_cycle_ = false;
    }

    bool has_negative_weight_cycles() const noexcept { return negative_cycle_; }

    explicit operator bool() const noexcept { return !negative_cycle_; }

private:

    bool negative_cycle_;
};

} // namespace graphs

#endif // INCLUDE_BELLMAN_FORD_HPP
