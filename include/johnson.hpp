#ifndef INCLUDE_JOHNSON_HPP
#define INCLUDE_JOHNSON_HPP

#include <cstddef>
#include <bit>
#include <functional>
#include <iterator>
#include <ranges>
#include <unordered_map>

#include "graph_traits.hpp"
#include "distance.hpp"
#include "bellman_ford.hpp"
#include "dijkstra.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>>
class Johnson final
{
    using weight_type = typename Traits::weight_type;
    using distance_type = Distance<weight_type>;
    using vertex_type = typename Traits::vertex_type;
    using vertex_iterator = typename Traits::vertex_iterator;
    using iterator_hash = typename Traits::iterator_hash;

public:

    Johnson(G &g) // temporary measure
    {
        vertex_iterator s_it = add_source_vertex(g);
        Bellman_Ford bellman_ford{g, s_it};
        g.erase_vertex(s_it); // s_it is invalidated here

        if (!bellman_ford.has_negative_weight_cycles())
        {
            reweight(g, bellman_ford, std::plus<weight_type>{});
            compute_shortest_paths(g, bellman_ford);
            reweight(g, bellman_ford, std::minus<weight_type>{});
        }
    }

    bool has_negative_weight_cycles() const noexcept { return storage_.empty(); }

    explicit operator bool() const noexcept { return !has_negative_weight_cycles(); }

    distance_type distance(vertex_iterator from, vertex_iterator to) const
    {
        return storage_.at(from).at(to);
    }

private:

    vertex_iterator add_source_vertex(G &g) const
    {
        vertex_iterator s_it = g.insert_vertex(vertex_type{});

        for (auto it = std::ranges::cbegin(g); it != s_it; ++it)
            g.insert_edge(s_it, it, 0);

        for (auto it = std::next(s_it), ite = std::ranges::cend(g); it != ite; ++it)
            g.insert_edge(s_it, it, 0);

        return s_it;
    }

    template<typename BinOp>
    void reweight(G &g, const Bellman_Ford<G> &bellman_ford, BinOp op) const
    {
        // We can easily call method value() on objects of type distance_type because
        // there is definitely a path from S to every other vertex

        for (auto u_it = std::ranges::begin(g), ite = std::ranges::end(g); u_it != ite; ++u_it)
        {
            weight_type h_u = bellman_ford.distance(u_it).value();

            auto [adj_begin, adj_end] = Traits::adjacent_vertices(g, u_it);
            for (auto v_it : std::ranges::subrange(adj_begin, adj_end))
            {
                weight_type h_v = bellman_ford.distance(v_it).value();
                g.change_weight(u_it, v_it, op(g.weight(u_it, v_it), h_u - h_v));
            }
        }
    }

    void compute_shortest_paths(G &g, const Bellman_Ford<G> &bellman_ford)
    {
        // We can easily call method value() on objects of type distance_type because
        // there is definitely a path from S to every other vertex

        auto n_vertices = Traits::n_vertices(g);
        storage_.reserve(n_vertices);

        auto begin = std::ranges::begin(g);
        auto end = std::ranges::end(g);
        for (auto u_it = begin; u_it != end; ++u_it)
        {
            Dijkstra dijkstra{g, u_it};

            auto &row = storage_[u_it];
            row.reserve(n_vertices);

            weight_type h_u = bellman_ford.distance(u_it).value();

            for (auto v_it = begin; v_it != end; ++v_it)
            {
                weight_type h_v = bellman_ford.distance(v_it).value();
                row.emplace(v_it, dijkstra.distance(v_it) + (h_v - h_u));
            }
        }
    }

    std::unordered_map<vertex_iterator,
                       std::unordered_map<vertex_iterator,
                                          distance_type,
                                          iterator_hash>,
                       iterator_hash> storage_;
};

} // namespace graphs

#endif // INCLUDE_JOHNSON_HPP
