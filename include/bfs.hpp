#ifndef INCLUDE_BFS_HPP
#define INCLUDE_BFS_HPP

#include <cstddef>
#include <optional>
#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>
#include <ranges>

#include "graph_traits.hpp"
#include "distance.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
class BFS final
{
public:

    using distance_type = Distance<std::size_t>;

private:

    using size_type = typename Traits::size_type;

    enum class Color { white, gray };

    struct Info_Node final
    {
        distance_type distance;
        std::optional<size_type> predecessor;

        Info_Node() = default;
        Info_Node(std::size_t dist) : distance{dist} {}
    };

    using color_table_type = std::unordered_map<size_type, Color>;

public:

    BFS(const G &g, size_type source_i)
    {
        auto color_table = bfs_init(g, source_i);

        std::queue<size_type> Q;
        Q.push(source_i);

        while (!Q.empty())
        {
            const size_type u_i = Q.front();
            Q.pop();

            const Info_Node &u_info = info_.find(u_i)->second;

            for (auto v_i : Traits::adjacent_vertices(g, u_i))
            {
                if (color_table[v_i] == Color::white)
                {
                    color_table[v_i] = Color::gray;

                    Info_Node &v_info = info_.find(v_i)->second;
                    v_info.distance = u_info.distance + 1uz;
                    v_info.predecessor = u_i;

                    Q.push(v_i);
                }
            }
        }
    }

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

private:

    color_table_type bfs_init(const G &g, size_type source_i)
    {
        color_table_type color_table;

        const size_type n_vertices = Traits::n_vertices(g);
        info_.reserve(n_vertices);
        color_table.reserve(n_vertices);

        for (auto i : std::views::iota(size_type{0}, source_i))
        {
            info_.try_emplace(i);
            color_table.try_emplace(i, Color::white);
        }

        info_.try_emplace(source_i, 0);
        color_table.try_emplace(source_i, Color::gray);

        for (auto i : std::views::iota(source_i + 1, n_vertices))
        {
            info_.try_emplace(i);
            color_table.try_emplace(i, Color::white);
        }

        return color_table;
    }

    std::unordered_map<size_type, Info_Node> info_;
};

} // namespace graphs

#endif // INCLUDE_BFS_HPP
