#ifndef INCLUDE_BELLMAN_FORD_HPP
#define INCLUDE_BELLMAN_FORD_HPP

#include <ranges>

#include "graph_traits.hpp"
#include "single_source_shortest_paths.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
class Bellman_Ford final : public SSSP<G, Traits>
{
    using sssp = SSSP<G, Traits>;
    using sssp::info_;
    using typename sssp::size_type;
    using typename sssp::Info_Node;

public:

    using typename sssp::distance_type;

    Bellman_Ford(const G &g, size_type source_i) : sssp{g, source_i}
    {
        const size_type n_vertices = Traits::n_vertices(g);

        for (auto _ : std::views::iota(size_type{0}, n_vertices - 1))
        {
            for (auto u_i : std::views::iota(size_type{0}, n_vertices))
            {
                const Info_Node &u_info = info_.find(u_i)->second;

                for (auto v_i : Traits::adjacent_vertices(g, u_i))
                {
                    Info_Node &v_info = info_.find(v_i)->second;

                    if (distance_type d = u_info.distance + Traits::weight(g, u_i, v_i);
                        d < v_info.distance)
                    {
                        v_info.distance = d;
                        v_info.predecessor = u_i;
                    }
                }
            }
        }

        for (auto u_i : std::views::iota(size_type{0}, n_vertices))
        {
            const Info_Node &u_info = info_.find(u_i)->second;

            for (auto v_i : Traits::adjacent_vertices(g, u_i))
            {
                const Info_Node &v_info = info_.find(v_i)->second;

                if (v_info.distance > u_info.distance + Traits::weight(g, u_i, v_i))
                {
                    info_.clear();
                    return;
                }
            }
        }
    }

    bool has_negative_weight_cycles() const noexcept { return info_.empty(); }

    explicit operator bool() const noexcept { return !has_negative_weight_cycles(); }
};

} // namespace graphs

#endif // INCLUDE_BELLMAN_FORD_HPP
