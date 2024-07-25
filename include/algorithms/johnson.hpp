#ifndef INCLUDE_JOHNSON_HPP
#define INCLUDE_JOHNSON_HPP

#include <cstddef>
#include <functional>
#include <iterator>
#include <ranges>
#include <unordered_map>

#include <boost/container_hash/hash.hpp>

#include "utils/graph_traits.hpp"
#include "utils/distance.hpp"
#include "bellman_ford.hpp"
#include "dijkstra.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
class Johnson final
{
    using weight_type = typename Traits::weight_type;
    using vertex_type = typename Traits::vertex_type;
    using size_type = typename Traits::size_type;

public:

    using distance_type = Distance<weight_type>;

    Johnson(G g)
    {
        const size_type s_i = add_source_vertex(g);
        const Bellman_Ford bellman_ford{g, s_i};
        g.erase_vertex(s_i);

        if (!bellman_ford.has_negative_weight_cycles())
        {
            reweight(g, bellman_ford);
            compute_shortest_paths(g, bellman_ford);
        }
    }

    bool has_negative_weight_cycles() const noexcept { return storage_.empty(); }

    explicit operator bool() const noexcept { return !has_negative_weight_cycles(); }

    distance_type distance(size_type from, size_type to) const
    {
        return storage_.at(std::pair{from, to});
    }

private:

    size_type add_source_vertex(G &g) const
    {
        const size_type s_i = g.insert_vertex(vertex_type{});

        for (auto i : std::views::iota(size_type{0}, s_i))
            g.insert_edge(s_i, i, 0);

        for (auto i : std::views::iota(s_i, Traits::n_vertices(g)))
            g.insert_edge(s_i, i, 0);

        return s_i;
    }

    static void reweight(G &g, const Bellman_Ford<G> &bellman_ford)
    {
        // We can easily call operator*() on objects of type distance_type because
        // there is definitely a path from S to every other vertex

        const size_type n_vertices = Traits::n_vertices(g);

        for (auto u_i : std::views::iota(size_type{0}, n_vertices))
        {
            const weight_type h_u = *bellman_ford.distance(u_i);

            for (auto v_i : Traits::adjacent_vertices(g, u_i))
            {
                const weight_type h_v = *bellman_ford.distance(v_i);
                g.change_weight(u_i, v_i, g.weight(u_i, v_i) + (h_u - h_v));
            }
        }
    }

    void compute_shortest_paths(const G &g, const Bellman_Ford<G> &bellman_ford)
    {
        // We can easily call operator*() on objects of type distance_type because
        // there is definitely a path from S to every other vertex

        auto n_vertices = Traits::n_vertices(g);
        storage_.reserve(n_vertices * n_vertices);

        for (auto u_i : std::views::iota(size_type{0}, n_vertices))
        {
            const Dijkstra dijkstra{g, u_i};
            const weight_type h_u = *bellman_ford.distance(u_i);

            for (auto v_i : std::views::iota(size_type{0}, n_vertices))
            {
                const weight_type h_v = *bellman_ford.distance(v_i);
                storage_[std::pair{u_i, v_i}] = dijkstra.distance(v_i) + (h_v - h_u);
            }
        }
    }

    std::unordered_map<std::pair<size_type, size_type>,
                       distance_type,
                       boost::hash<std::pair<size_type, size_type>>> storage_;
};

} // namespace graphs

#endif // INCLUDE_JOHNSON_HPP
