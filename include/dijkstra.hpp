#ifndef INCLUDE_DIJKSTRA_HPP
#define INCLUDE_DIJKSTRA_HPP

#include <cstddef>
#include <optional>
#include <bit>
#include <unordered_map>
#include <iterator>
#include <utility>
#include <ranges>
#include <algorithm>
#include <vector>
#include <stdexcept>

#include <boost/heap/fibonacci_heap.hpp>

#include "graph_traits.hpp"
#include "distance.hpp"

namespace graphs
{

struct Negative_Weights : public std::logic_error
{
    Negative_Weights()
        : std::logic_error{"Dijksta's algorithms can only be used "
                           "for graphs with non-negative weights"} {};
};

template<typename G, typename Traits = graph_traits<G>>
class Dijkstra final
{
public:

    using distance_type = Distance<typename Traits::weight_type>;

private:

    using vertex_iterator = typename Traits::vertex_iterator;
    using priority_pair = std::pair<vertex_iterator, distance_type>;

    struct iterator_hash final
    {
        std::size_t operator()(vertex_iterator it) const noexcept
        {
            return std::bit_cast<std::size_t>(it);
        }
    };

    // we cannot mark this class final because boost inherits from it
    struct priority_comp
    {
        bool operator()(const priority_pair &lhs, const priority_pair &rhs) const
        {
            return lhs.second > rhs.second;
        }
    };

    struct Info_Node final
    {
        distance_type distance_;
        std::optional<vertex_iterator> predecessor_;

        Info_Node() = default;
        Info_Node(typename Traits::weight_type d) : distance_{d} {}
    };

    using info_table_type = std::unordered_map<vertex_iterator,
                                               Info_Node,
                                               iterator_hash>;

public:

    Dijkstra(const G &g, vertex_iterator source_it)
    {
        if (source_it == std::ranges::end(g))
            return;

        if (has_negative_weights(g))
            throw Negative_Weights{};

        dijkstra_init(g, source_it);

        boost::heap::fibonacci_heap<priority_pair,
                                    boost::heap::compare<priority_comp>> Q;

        for (const auto &[v_it, node] : info_)
            Q.emplace(v_it, node.distance_);

        while (!Q.empty())
        {
            auto [u_it, u_d] = Q.top();
            Q.pop();

            // we are sure that find() returns a valid iterator; no need for at()
            Info_Node &u_info = info_.find(u_it)->second;

            auto [begin, end] = Traits::adjacent_vertices(g, u_it);
            for (auto v_it : std::ranges::subrange(begin, end))
            {
                // we are sure that find() returns a valid iterator; no need for at()
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

    static bool has_negative_weights(const G &g)
    {
        for (auto u_it = std::ranges::begin(g), ite = std::ranges::end(g); u_it != ite; ++u_it)
        {
            auto cond = [&g, u_it](auto v_it){ return Traits::weight(g, u_it, v_it) < 0; };
            auto [from, to] = Traits::adjacent_vertices(g, u_it);

            if (std::ranges::any_of(from, to, cond))
                return true;
        }

        return false;
    }

private:

    void dijkstra_init(const G &g, vertex_iterator source_it)
    {
        info_.reserve(Traits::n_vertices(g));

        for (auto it = std::ranges::begin(g); it != source_it; ++it)
            info_.emplace(it, Info_Node{});

        info_.emplace(source_it, Info_Node{0});

        for (auto it = std::next(source_it), ite = std::ranges::end(g); it != ite; ++it)
            info_.emplace(it, Info_Node{});
    }

    std::unordered_map<vertex_iterator,
                       Info_Node,
                       iterator_hash> info_;
};

} // namespace graphs

#endif // INCLUDE_DIJKSTRA_HPP
