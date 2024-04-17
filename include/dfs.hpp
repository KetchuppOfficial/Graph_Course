#ifndef INCLUDE_DFS_HPP
#define INCLUDE_DFS_HPP

#include <ranges>
#include <stack>
#include <vector>
#include <optional>
#include <unordered_map>
#include <ostream>
#include <memory>

#include "graph_traits.hpp"

namespace graphs
{

struct recursive final {}; // a tag to be used if you want to run dfs recursively

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
requires std::ranges::forward_range<G>
class DFS final
{
public:

    using time_type = std::size_t;

private:

    using vertex_iterator = typename Traits::vertex_iterator;
    using iterator_hash = typename Traits::iterator_hash;
    using stack_type = std::stack<vertex_iterator, std::vector<vertex_iterator>>;

    enum class Color { white, gray };

    struct Info_Node final
    {
        std::optional<vertex_iterator> predecessor_;
        time_type discovery_time_;
        time_type finished_time_;
    };

    using color_table_type = std::unordered_map<vertex_iterator,
                                                Color,
                                                iterator_hash>;

    using info_table_type = std::unordered_map<vertex_iterator,
                                               Info_Node,
                                               iterator_hash>;

public:

    DFS(const G &g)
    {
        auto color_table = dfs_init(g);
        time_type time = 0;

        // s_ stands for "source"
        for (auto s_it = std::ranges::begin(g), ite = std::ranges::end(g); s_it != ite; ++s_it)
        {
            if (color_table[s_it] == Color::white)
            {
                stack_type stack;
                stack.push(s_it);

                while(!stack.empty())
                {
                    vertex_iterator u_it = stack.top();
                    Info_Node &u_info = info_.find(u_it)->second;

                    if (color_table[u_it] == Color::white)
                    {
                        color_table[u_it] = Color::gray;

                        u_info.discovery_time_ = ++time;

                        for (auto v_it : Traits::adjacent_vertices(g, u_it))
                        {
                            if (color_table[v_it] == Color::white)
                            {
                                Info_Node &v_info = info_.find(v_it)->second;
                                v_info.predecessor_ = u_it;
                                stack.push(v_it);
                            }
                        }
                    }
                    else
                    {
                        u_info.finished_time_ = ++time;
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

        for (auto s_it = std::ranges::begin(g), ite = std::ranges::end(g); s_it != ite; ++s_it)
            if (color_table[s_it] == Color::white)
                time = visit(g, info_, color_table, s_it, time);
    }

    void graphic_dump(std::ostream &os)
    {
        os << "digraph G\n"
              "{\n";

        for (auto &[vertex_it, info] : info_)
            os << "    node_" << std::addressof(*vertex_it) << " [shape = record, label = \"key: "
               << *vertex_it << " | " << info.discovery_time_ << '/' << info.finished_time_
               << "\"];\n";

        os << '\n';

        for (auto &[vertex_it, info] : info_)
            if (info.predecessor_.has_value())
                os << "    node_" << std::addressof(*info.predecessor_.value())
                   << " -> node_" << std::addressof(*vertex_it) << ";\n";

        os << "}\n";
    }

private:

    color_table_type dfs_init(const G &g)
    {
        color_table_type color_table;

        auto n_vertices = Traits::n_vertices(g);
        info_.reserve(n_vertices);
        color_table.reserve(n_vertices);

        for (auto it = std::ranges::begin(g), ite = std::ranges::end(g); it != ite; ++it)
        {
            info_.try_emplace(it);
            color_table.try_emplace(it, Color::white);
        }

        return color_table;
    }

    static time_type visit(const G &g, info_table_type &info, color_table_type &color_table,
                           vertex_iterator u_it, time_type time)
    {
        color_table[u_it] = Color::gray;

        Info_Node &u_info = info.find(u_it)->second;
        u_info.discovery_time_ = ++time;

        for (auto v_it : Traits::adjacent_vertices(g, u_it))
        {
            if (color_table[v_it] == Color::white)
            {
                Info_Node &v_info = info.find(v_it)->second;
                v_info.predecessor_ = u_it;

                time = visit(g, info, color_table, v_it, time);
            }
        }

        u_info.finished_time_ = ++time;

        return time;
    }

    std::unordered_map<vertex_iterator,
                       Info_Node,
                       iterator_hash> info_;
};

} // namespace graphs

#endif // INCLUDE_DFS_HPP
