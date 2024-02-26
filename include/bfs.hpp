#ifndef INCLUDE_BFS_HPP
#define INCLUDE_BFS_HPP

#include <ranges>
#include <bit>
#include <optional>
#include <map>
#include <queue>
#include <unordered_map>
#include <iterator>

#include "graph_traits.hpp"

namespace graphs
{

template<typename G, typename Traits = graph_traits<G>> // G stands for "graph"
requires std::ranges::forward_range<G>
class BFS final
{
    using graph_type = G;
    using vertex_iterator = typename Traits::vertex_iterator;
    struct iterator_hash final
    {
        std::size_t operator()(vertex_iterator it) const noexcept
        {
            return std::bit_cast<std::size_t>(it);
        }
    };

public:

    using distance_type = std::optional<std::size_t>;

    struct BFS_Node final
    {
        vertex_iterator node_it_;
        vertex_iterator predecessor_it_;
    };

private:

    enum class Color { white, gray };

    struct Info_Node final
    {
        distance_type distance_;
        vertex_iterator predecessor_;
        Color color_{Color::white};

        Info_Node(vertex_iterator default_predecessor) : predecessor_{default_predecessor} {}
        Info_Node(std::size_t distance, Color color, vertex_iterator default_predecessor)
                 : distance_{distance},
                   predecessor_{default_predecessor},
                   color_{color} {}
    };

    struct Comp final
    {
        bool operator()(distance_type lhs, distance_type rhs) const
        {
            if (!lhs)
                return false;
            else if (!rhs)
                return true;
            else
                return *lhs < *rhs;
        }
    };

    using table_type = std::multimap<distance_type, BFS_Node, Comp>;

public:

    BFS(const graph_type &g, vertex_iterator source_it)
    {
        if (source_it == std::ranges::end(g))
            return;

        auto bfs_info = bfs_init(g, source_it);

        std::queue<vertex_iterator> Q;
        Q.push(source_it);

        while (!Q.empty())
        {
            vertex_iterator u_it = Q.front();
            Q.pop();

            auto [begin, end] = Traits::adjacent_vertices(g, u_it);
            for (auto v_it : std::ranges::subrange(begin, end))
            {
                // we are sure that find() return a valid iterator; no need for at()
                Info_Node &v_info = bfs_info.find(v_it)->second;

                if (v_info.color_ == Color::white)
                {
                    // we are sure that find() return a valid iterator; no need for at()
                    Info_Node &u_info = bfs_info.find(u_it)->second;

                    v_info.color_ = Color::gray;
                    v_info.distance_ = *u_info.distance_ + 1;
                    v_info.predecessor_ = u_it;
                    Q.push(v_it);
                }
            }
        }

        for (auto &[vertex_it, info_node] : bfs_info)
            bfs_table_.emplace(info_node.distance_, BFS_Node{vertex_it, info_node.predecessor_});
    }

    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::const_iterator;

    iterator begin() { return bfs_table_.begin(); }
    const_iterator begin() const { return bfs_table_.begin(); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return bfs_table_.end(); }
    const_iterator end() const { return bfs_table_.end(); }
    const_iterator cend() const { return end(); }

private:

    using info_table_type = std::unordered_map<vertex_iterator, Info_Node, iterator_hash>;

    info_table_type bfs_init(const graph_type &g, vertex_iterator source_it)
    {
        info_table_type bfs_info;
        bfs_info.reserve(Traits::n_vertices(g));

        auto end = std::ranges::end(g);

        for (auto it = std::ranges::begin(g); it != source_it; ++it)
            bfs_info.emplace(it, Info_Node{end});

        bfs_info.emplace(source_it, Info_Node{0, Color::gray, end});

        for (auto it = std::next(source_it); it != end; ++it)
            bfs_info.emplace(it, Info_Node{end});

        return bfs_info;
    }

    table_type bfs_table_;
};

} // namespace graphs

#endif // INCLUDE_BFS_HPP
