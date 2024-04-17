#ifndef INCLUDE_BFS_HPP
#define INCLUDE_BFS_HPP

#include <ranges>
#include <optional>
#include <queue>
#include <unordered_map>
#include <iterator>

#include "graph_traits.hpp"
#include "distance.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
requires std::ranges::forward_range<G>
class BFS final
{
public:

    using distance_type = Distance<std::size_t>;

private:

    using vertex_iterator = typename Traits::vertex_iterator;
    using iterator_hash = typename Traits::iterator_hash;

    enum class Color { white, gray };

    struct Info_Node final
    {
        distance_type distance_;
        std::optional<vertex_iterator> predecessor_;

        Info_Node() = default;
        Info_Node(std::size_t distance) : distance_{distance} {}
    };

    using info_table_type = std::unordered_map<vertex_iterator,
                                               Info_Node,
                                               iterator_hash>;

    using color_table_type = std::unordered_map<vertex_iterator,
                                                Color,
                                                iterator_hash>;

public:

    BFS(const G &g, vertex_iterator source_it)
    {
        auto color_table = bfs_init(g, source_it);

        std::queue<vertex_iterator> Q;
        Q.push(source_it);

        while (!Q.empty())
        {
            vertex_iterator u_it = Q.front();
            Q.pop();

            const Info_Node &u_info = info_.find(u_it)->second;

            for (auto v_it : Traits::adjacent_vertices(g, u_it))
            {
                if (color_table[v_it] == Color::white)
                {
                    color_table[v_it] = Color::gray;

                    Info_Node &v_info = info_.find(v_it)->second;
                    v_info.distance_ = u_info.distance_ + std::size_t{1};
                    v_info.predecessor_ = u_it;

                    Q.push(v_it);
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

    distance_type distance(vertex_iterator u_it) const { return info_.at(u_it).distance_; }

private:

    color_table_type bfs_init(const G &g, vertex_iterator source_it)
    {
        color_table_type color_table;

        auto n_vertices = Traits::n_vertices(g);
        info_.reserve(n_vertices);
        color_table.reserve(n_vertices);

        for (auto it = std::ranges::begin(g); it != source_it; ++it)
        {
            info_.try_emplace(it);
            color_table.try_emplace(it, Color::white);
        }

        info_.try_emplace(source_it, 0);
        color_table.try_emplace(source_it, Color::gray);

        for (auto it = std::next(source_it); it != std::ranges::end(g); ++it)
        {
            info_.try_emplace(it);
            color_table.try_emplace(it, Color::white);
        }

        return color_table;
    }

    info_table_type info_;
};

} // namespace graphs

#endif // INCLUDE_BFS_HPP
