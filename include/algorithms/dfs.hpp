#ifndef INCLUDE_ALGORITHMS_DFS_HPP
#define INCLUDE_ALGORITHMS_DFS_HPP

#include <cstddef>
#include <stack>
#include <vector>
#include <unordered_map>
#include <optional>
#include <ranges>

#include "utils/graph_traits.hpp"

namespace graphs
{

struct recursive final {}; // a tag to be used if you want to run dfs recursively

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
class DFS final
{
public:

    using time_type = std::size_t;

private:

    using size_type = typename Traits::size_type;
    using stack_type = std::stack<size_type, std::vector<size_type>>;

    enum class Color { white, gray };

    using color_table_type = std::unordered_map<size_type, Color>;

    struct Info_Node final
    {
        std::optional<size_type> predecessor;
        time_type discovery_time;
        time_type finished_time;
    };

public:

    DFS(const G &g)
    {
        auto color_table = dfs_init(g);
        time_type time = 0;

        // s_ stands for "source"
        for (auto s_i : std::views::iota(size_type{0}, Traits::n_vertices(g)))
        {
            if (color_table[s_i] == Color::white)
            {
                stack_type stack;
                stack.push(s_i);

                while(!stack.empty())
                {
                    const size_type u_i = stack.top();
                    Info_Node &u_info = info_.find(u_i)->second;

                    if (color_table[u_i] == Color::white)
                    {
                        color_table[u_i] = Color::gray;

                        u_info.discovery_time = ++time;

                        for (auto v_i : Traits::adjacent_vertices(g, u_i))
                        {
                            if (color_table[v_i] == Color::white)
                            {
                                Info_Node &v_info = info_.find(v_i)->second;
                                v_info.predecessor = u_i;
                                stack.push(v_i);
                            }
                        }
                    }
                    else
                    {
                        u_info.finished_time = ++time;
                        stack.pop();
                    }
                }
            }
        }
    }

    DFS(const G &g, recursive)
    {
        auto color_table = dfs_init(g);
        time_type time = 0;

        for (auto s_i : std::views::iota(size_type{0}, Traits::n_vertices(g)))
            if (color_table[s_i] == Color::white)
                time = visit(g, color_table, s_i, time);
    }

    time_type discovery_time(size_type i) const { return info_.at(i).discovery_time; }
    time_type finished_time(size_type i) const { return info_.at(i).finished_time; }

private:

    color_table_type dfs_init(const G &g)
    {
        color_table_type color_table;

        const size_type n_vertices = Traits::n_vertices(g);
        info_.reserve(n_vertices);
        color_table.reserve(n_vertices);

        for (auto i : std::views::iota(size_type{0}, n_vertices))
        {
            info_.try_emplace(i);
            color_table.try_emplace(i, Color::white);
        }

        return color_table;
    }

    time_type visit(const G &g, color_table_type &color_table, size_type u_i, time_type time)
    {
        color_table[u_i] = Color::gray;

        Info_Node &u_info = info_.find(u_i)->second;
        u_info.discovery_time = ++time;

        for (auto v_i : Traits::adjacent_vertices(g, u_i))
        {
            if (color_table[v_i] == Color::white)
            {
                Info_Node &v_info = info_.find(v_i)->second;
                v_info.predecessor = u_i;

                time = visit(g, color_table, v_i, time);
            }
        }

        u_info.finished_time = ++time;

        return time;
    }

    std::unordered_map<size_type, Info_Node> info_;
};

} // namespace graphs

#endif // INCLUDE_ALGORITHMS_DFS_HPP
