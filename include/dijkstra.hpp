#ifndef INCLUDE_DIJKSTRA_HPP
#define INCLUDE_DIJKSTRA_HPP

#include <stdexcept>
#include <utility>
#include <vector>
#include <ranges>
#include <algorithm>

#include <boost/heap/fibonacci_heap.hpp>

#include "graph_traits.hpp"
#include "single_source_shortest_paths.hpp"

namespace graphs
{

struct Negative_Weights : public std::logic_error
{
    Negative_Weights()
        : std::logic_error{"Dijksta's algorithms can only be used "
                           "for graphs with non-negative weights"} {};
};

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
class Dijkstra final : public SSSP<G, Traits>
{
    using sssp = SSSP<G, Traits>;
    using sssp::info_;
    using typename sssp::size_type;
    using typename sssp::Info_Node;

public:

    using typename sssp::distance_type;

private:

    using priority_pair = std::pair<size_type, distance_type>;

    // we cannot mark this class final because boost inherits from it
    struct priority_comp
    {
        bool operator()(const priority_pair &lhs, const priority_pair &rhs) const
        {
            return lhs.second > rhs.second;
        }
    };

public:

    Dijkstra(const G &g, size_type source_i) : sssp{g, source_i}
    {
        if (has_negative_weights(g))
            throw Negative_Weights{};

        boost::heap::fibonacci_heap<priority_pair,
                                    boost::heap::compare<priority_comp>> Q;
        using handle_type = decltype(Q)::handle_type;

        std::vector<handle_type> handles;
        handles.reserve(Traits::n_vertices(g));

        for (const auto &[u_i, node] : info_)
            handles[u_i] = Q.emplace(u_i, node.distance);

        while (!Q.empty())
        {
            const auto [u_i, u_d] = Q.top();
            Q.pop();

            const Info_Node &u_info = info_.find(u_i)->second;

            for (auto v_i : Traits::adjacent_vertices(g, u_i))
            {
                Info_Node &v_info = info_.find(v_i)->second;

                if (distance_type d = u_info.distance + Traits::weight(g, u_i, v_i);
                    d < v_info.distance)
                {
                    v_info.distance = d;
                    v_info.predecessor = u_i;

                    Q.decrease(handles[v_i], std::pair{v_i, d.value()});
                }
            }
        }
    }

    static bool has_negative_weights(const G &g)
    {
        const size_type n_vertices = Traits::n_vertices(g);

        for (auto u_i : std::views::iota(size_type{0}, n_vertices))
        {
            auto cond = [&g, u_i](size_type v_i){ return Traits::weight(g, u_i, v_i) < 0; };

            if (std::ranges::any_of(Traits::adjacent_vertices(g, u_i), cond))
                return true;
        }

        return false;
    }
};

} // namespace graphs

#endif // INCLUDE_DIJKSTRA_HPP
