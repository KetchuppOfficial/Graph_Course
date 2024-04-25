#ifndef INCLUDE_DIJKSTRA_HPP
#define INCLUDE_DIJKSTRA_HPP

#include <stdexcept>
#include <utility>
#include <ranges>
#include <iterator>

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
requires std::ranges::forward_range<G>
class Dijkstra final : public SSSP<G, Traits>
{
    using sssp = SSSP<G, Traits>;
    using sssp::info_;
    using vertex_iterator = typename Traits::vertex_iterator;
    using Info_Node = typename sssp::Info_Node;

public:

    using typename sssp::distance_type;

private:

    using priority_pair = std::pair<vertex_iterator, distance_type>;

    // we cannot mark this class final because boost inherits from it
    struct priority_comp
    {
        bool operator()(const priority_pair &lhs, const priority_pair &rhs) const
        {
            return lhs.second > rhs.second;
        }
    };

public:

    Dijkstra(const G &g, vertex_iterator source_it) : sssp{g, source_it}
    {
        if (has_negative_weights(g))
            throw Negative_Weights{};

        boost::heap::fibonacci_heap<priority_pair,
                                    boost::heap::compare<priority_comp>> Q;

        for (const auto &[v_it, node] : info_)
            Q.emplace(v_it, node.distance_);

        while (!Q.empty())
        {
            auto [u_it, u_d] = Q.top();
            Q.pop();

            Info_Node &u_info = info_.find(u_it)->second;

            for (auto v_it : Traits::adjacent_vertices(g, u_it))
            {
                Info_Node &v_info = info_.find(v_it)->second;

                if (distance_type d = u_info.distance_ + Traits::weight(g, u_it, v_it);
                    d < v_info.distance_)
                {
                    v_info.distance_ = d;
                    v_info.predecessor_ = u_it;

                    auto v_pos = std::ranges::find(Q, v_it, &priority_pair::first);
                    assert(v_pos != std::ranges::end(Q));

                    Q.decrease(Q.s_handle_from_iterator(v_pos), std::pair{v_it, d.value()});
                }
            }
        }
    }

    static bool has_negative_weights(const G &g)
    {
        for (auto u_it = std::ranges::begin(g), ite = std::ranges::end(g); u_it != ite; ++u_it)
        {
            auto cond = [&g, u_it](auto v_it){ return Traits::weight(g, u_it, v_it) < 0; };

            if (std::ranges::any_of(Traits::adjacent_vertices(g, u_it), cond))
                return true;
        }

        return false;
    }
};

} // namespace graphs

#endif // INCLUDE_DIJKSTRA_HPP
