#ifndef INCLUDE_DFS_HPP
#define INCLUDE_DFS_HPP

#include <ranges>
#include <stack>
#include <vector>
#include <bit>
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
    using vertex_iterator = typename Traits::vertex_iterator;
    using stack_type = std::stack<vertex_iterator, std::vector<vertex_iterator>>;

    struct iterator_hash final
    {
        std::size_t operator()(vertex_iterator it) const noexcept
        {
            return std::bit_cast<std::size_t>(it);
        }
    };

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

    using table_type = std::unordered_map<vertex_iterator, DFS_Node, iterator_hash>;

public:

    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::iterator;

    DFS(const G &g)
    {
        auto dfs_info = dfs_init(g);
        time_type time = 0;

        // s_ stands for "source"
        for (auto s_it = std::ranges::begin(g), ite = std::ranges::end(g); s_it != ite; ++s_it)
        {
            // we are sure that find() returns a valid iterator; no need for at()
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

                        auto [begin, end] = Traits::adjacent_vertices(g, u_it);
                        for (auto v_it : std::ranges::subrange(begin, end))
                        {
                            // we are sure that find() returns a valid iterator; no need for at()
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

        fill_dfs_table(dfs_info);
    }

    DFS(const G &g, recursive)
    {
        auto dfs_info = dfs_init(g);
        time_type time = 0;

        for (auto s_it = std::ranges::begin(g), ite = std::ranges::end(g); s_it != ite; ++s_it)
        {
            // we are sure that find() returns a valid iterator; no need for at()
            Info_Node &s_info = dfs_info.find(s_it)->second;

            if (s_info.color_ == Color::white)
                time = visit(g, s_it, dfs_info, time);
        }

        fill_dfs_table(dfs_info);
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

    using info_table_type = std::unordered_map<vertex_iterator, Info_Node, iterator_hash>;

    info_table_type dfs_init(const G &g)
    {
        info_table_type dfs_info;
        dfs_info.reserve(Traits::n_vertices(g));

        for (auto it = std::ranges::begin(g), ite = std::ranges::end(g); it != ite; ++it)
            dfs_info.emplace(it, Info_Node{});

        return dfs_info;
    }

    void fill_dfs_table(const info_table_type &dfs_info)
    {
        dfs_table_.reserve(dfs_info.size());
        for (auto &[vertex_it, info_node] : dfs_info)
            dfs_table_.emplace(vertex_it, info_node.dfs_node_);
    }

    static time_type visit(const G& g, vertex_iterator u_it,
                           info_table_type &dfs_info, time_type time)
    {
        // we are sure that find() returns a valid iterator; no need for at()
        Info_Node &u_info = dfs_info.find(u_it)->second;

        u_info.dfs_node_.discovery_time_ = ++time;
        u_info.color_ = Color::gray;

        auto [begin, end] = Traits::adjacent_vertices(g, u_it);
        for (auto v_it : std::ranges::subrange(begin, end))
        {
            // we are sure that find() returns a valid iterator; no need for at()
            Info_Node &v_info = dfs_info.find(v_it)->second;

            if (v_info.color_ == Color::white)
            {
                v_info.dfs_node_.predecessor_ = u_it;
                time = visit(g, v_it, dfs_info, time);
            }
        }

        u_info.dfs_node_.finished_time_ = ++time;

        return time;
    }

    table_type dfs_table_;
};

} // namespace graphs

#endif // INCLUDE_DFS_HPP
