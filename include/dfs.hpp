#ifndef INCLUDE_DFS_HPP
#define INCLUDE_DFS_HPP

#include <stack>
#include <vector>
#include <optional>
#include <unordered_map>
#include <ranges>
#include <ostream>
#include <memory>

#include "graph.hpp"

namespace graphs
{

template<typename T>
class DFS final
{
    using graph_type = Directed_Graph<T>;
    using vertex_iterator = typename graph_type::const_iterator;
    using hash_type = typename graph_type::iterator_hash;
    using stack_type = std::stack<vertex_iterator, std::vector<vertex_iterator>>;

public:

    using time_type = std::size_t;

    struct DFS_Node final
    {
        std::optional<vertex_iterator> predecessor_;
        time_type discovery_time_;
        time_type finished_time_;
    };

private:

    enum class Color { white, gray };

    struct Info_Node final
    {
        DFS_Node dfs_node_;
        Color color_{Color::white};
    };

    using table_type = std::unordered_map<vertex_iterator, DFS_Node, hash_type>;

public:

    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::iterator;

    DFS(const graph_type &g)
    {
        auto dfs_info = dfs_init(g);
        time_type time = 0;

        // s_ stands for "source"
        for (auto s_it = g.begin(), ite = g.end(); s_it != ite; ++s_it)
        {
            // we are sure that find() return a valid iterator; no need for at()
            Info_Node &s_info = dfs_info.find(s_it)->second;

            if (s_info.color_ == Color::white)
            {
                stack_type stack;
                stack.push(s_it);

                while(!stack.empty())
                {
                    vertex_iterator u_it = stack.top();

                    // we are sure that find() return a valid iterator; no need for at()
                    Info_Node &u_info = dfs_info.find(u_it)->second;

                    if (u_info.color_ == Color::white)
                    {
                        u_info.color_ = Color::gray;
                        u_info.dfs_node_.discovery_time_ = ++time;

                        auto [begin, end] = g.adjacent_vertices(u_it);
                        for (auto v_it : std::ranges::subrange(begin, end))
                        {
                            // we are sure that find() return a valid iterator; no need for at()
                            Info_Node &v_info = dfs_info.find(v_it)->second;

                            if (v_info.color_ == Color::white)
                            {
                                v_info.dfs_node_.predecessor_ = u_it;
                                stack.push(v_it);
                            }
                        }
                    }
                    else
                    {
                        u_info.dfs_node_.finished_time_ = ++time;
                        stack.pop();
                    }
                }
            }
        }

        dfs_table_.reserve(dfs_info.size());
        for (auto &[vertex_it, info_node] : dfs_info)
            dfs_table_.emplace(vertex_it, info_node.dfs_node_);
    }

    void graphic_dump(std::ostream &os)
    {
        os << "digraph G\n"
              "{\n";

        for (auto &[vertex_it, dfs_info] : dfs_table_)
            os << "    node_" << std::addressof(*vertex_it) << " [shape = record, label = \"key: "
               << *vertex_it << " | " << dfs_info.discovery_time_ << '/' << dfs_info.finished_time_
               << "\"];\n";

        os << '\n';

        for (auto &[vertex_it, dfs_info] : dfs_table_)
            if (dfs_info.predecessor_.has_value())
                os << "    node_" << std::addressof(*dfs_info.predecessor_.value())
                   << " -> node_" << std::addressof(*vertex_it) << ";\n";

        os << "}\n";
    }

    iterator begin() { return dfs_table_.begin(); }
    const_iterator begin() const { return dfs_table_.begin(); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return dfs_table_.end(); }
    const_iterator end() const { return dfs_table_.end(); }
    const_iterator cend() const { return end(); }

private:

    using info_table_type = std::unordered_map<vertex_iterator, Info_Node, hash_type>;

    info_table_type dfs_init(const graph_type &g)
    {
        info_table_type dfs_info;
        dfs_info.reserve(g.n_vertices());

        for (auto it = g.begin(), ite = g.end(); it != ite; ++it)
            dfs_info.emplace(it, Info_Node{});

        return dfs_info;
    }

    table_type dfs_table_;
};

} // namespace graphs

#endif // INCLUDE_DFS_HPP
