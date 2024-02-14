#ifndef INCLUDE_GRAPH_HPP
#define INCLUDE_GRAPH_HPP

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <numeric>
#include <algorithm>
#include <memory>
#include <iterator>
#include <initializer_list>
#include <optional>
#include <queue>
#include <map>
#include <utility>
#include <bit>

namespace graphs
{

template<typename T>
class Directed_Graph final
{
    using vertex_cont = std::list<T>;

public:

    using vertex_type = T;
    using size_type = typename vertex_cont::size_type;
    using iterator = typename vertex_cont::iterator;
    using const_iterator = typename vertex_cont::const_iterator;
    using const_reference = const vertex_type &;

    struct iterator_hash final
    {
        std::size_t operator()(const_iterator it) const noexcept
        {
            return std::bit_cast<std::size_t>(it);
        }
    };

private:

    using edges_cont = std::unordered_set<const_iterator, iterator_hash>;
    using edge_iterator = typename edges_cont::const_iterator;
    using adjacency_range = std::pair<edge_iterator, edge_iterator>;

public:

    Directed_Graph() = default;

    Directed_Graph(std::initializer_list<vertex_type> il)
    {
        std::ranges::copy(il, std::back_inserter(vertices_));
    }

    Directed_Graph(const Directed_Graph &rhs) = delete;
    Directed_Graph &operator=(const Directed_Graph &rhs) = delete;

    Directed_Graph(Directed_Graph &&rhs) = default;
    Directed_Graph &operator=(Directed_Graph &rhs) = default;

    size_type n_vertices() const { return vertices_.size(); }
    size_type n_edges() const
    {
        return std::accumulate(adjacency_list_.begin(), adjacency_list_.end(), size_type{0},
                               [](size_type init, auto &elem){ return init + elem.second.size(); });
    }

    bool empty() const { return n_vertices() == 0; }

    iterator begin() { return vertices_.begin(); }
    const_iterator begin() const { return vertices_.begin(); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return vertices_.end(); }
    const_iterator end() const { return vertices_.end(); }
    const_iterator cend() const { return end(); }

    // Operations on vertices

    // O(V)
    iterator find_vertex(const_reference v) { return std::ranges::find(vertices_, v); }
    const_iterator find_vertex(const_reference v) const { return std::ranges::find(vertices_, v); }

    // O(V)
    bool contains(const_reference v) const { return find_vertex(v) != end(); }

    // O(1)
    iterator insert_vertex(const_reference v)
    {
        vertices_.emplace_back(v);
        return std::prev(vertices_.end());
    }

    // O(1)
    void erase_vertex(const_iterator it)
    {
        adjacency_list_.erase(it);
        vertices_.erase(it);
    }

    // O(V)
    void erase_vertex(const_reference v)
    {
        auto it = find_vertex(v);
        if (it != end())
            erase_vertex(it);
    }

    // Operations on edges

    // O(1)
    void insert_edge(const_iterator from_it, const_iterator to_it)
    {
        adjacency_list_[from_it].insert(to_it);
    }

    // O(V)
    void insert_edge(const_reference from, const_reference to)
    {
        auto from_it = find_vertex(from);
        if (from_it != end())
        {
            auto to_it = find_vertex(to);
            if (to_it != end())
                insert_edge(from_it, to_it);
        }
    }

    // O(1)
    void erase_edge(const_iterator from_it, const_iterator to_it)
    {
        adjacency_list_[from_it].erase(to_it);
    }

    // O(V)
    void erase_edge(const_reference from, const_reference to)
    {
        auto from_it = find_vertex(from);
        if (from_it != end())
        {
            auto to_it = find_vertex(to);
            if (to_it != end())
                erase_edge(from_it, to_it);
        }
    }

    // Mixed operations

    // O(1)
    bool are_adjacent(const_iterator from_it, const_iterator to_it) const
    {
        auto edges_it = adjacency_list_.find(from_it);
        if (edges_it == adjacency_list_.end())
            return false;
        else
        {
            auto &edges = edges_it->second;
            return edges.find(to_it) != edges.end();
        }
    }

    // O(V)
    bool are_adjacent(const_reference from, const_reference to) const
    {
        auto from_it = find_vertex(from);
        if (from_it == end())
            return false;

        auto to_it = find_vertex(to);
        if (to_it == end())
            return false;

        return are_adjacent(from_it, to_it);
    }

    std::optional<adjacency_range> adjacent_vertices(const_iterator it) const
    {
        auto edges_it = adjacency_list_.find(it);
        if (edges_it == adjacency_list_.end())
            return std::nullopt;
        else
            return std::pair{edges_it->second.begin(), edges_it->second.end()};
    }

private:

    vertex_cont vertices_;
    std::unordered_map<const_iterator,
                       edges_cont,
                       iterator_hash> adjacency_list_;
};

template<typename T>
class BFS final
{
    using graph_type = Directed_Graph<T>;
    using vertex_iterator = typename graph_type::const_iterator;
    using distance_type = std::optional<std::size_t>;
    using hash_type = typename graph_type::iterator_hash;

    enum class Color { white, gray };

    struct Tmp_Node final
    {
        distance_type distance_;
        vertex_iterator predecessor_;
        Color color_{Color::white};

        Tmp_Node(vertex_iterator default_predecessor) : predecessor_{default_predecessor} {}
        Tmp_Node(std::size_t distance, Color color, vertex_iterator default_predecessor)
                : distance_{distance}, predecessor_{default_predecessor}, color_{color} {}
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


    using info_table_type = std::unordered_map<vertex_iterator, Tmp_Node, hash_type>;

public:

    struct Info_Node final
    {
        vertex_iterator node_it_;
        vertex_iterator predecessor_it_;
    };

    using table_type = std::multimap<distance_type, Info_Node, Comp>;
    using iterator = typename table_type::iterator;
    using const_iterator = typename table_type::const_iterator;

    BFS(const graph_type &g, vertex_iterator source_it)
    {
        if (source_it == g.end())
            return;

        info_table_type bfs_info = bfs_init(g, source_it);

        std::queue<vertex_iterator> Q;
        Q.push(source_it);

        while (!Q.empty())
        {
            vertex_iterator u_it = Q.front();
            Q.pop();

            auto adj = g.adjacent_vertices(u_it);
            if (adj.has_value())
            {
                for (auto v_it : std::ranges::subrange(adj->first, adj->second))
                {
                    // we are sure that find() return a valid iterator
                    Tmp_Node &v_info = bfs_info.find(v_it)->second;

                    if (v_info.color_ == Color::white)
                    {
                        // we are sure that find() return a valid iterator
                        Tmp_Node &u_info = bfs_info.find(u_it)->second;

                        v_info.color_ = Color::gray;
                        v_info.distance_ = *u_info.distance_ + 1;
                        v_info.predecessor_ = u_it;
                        Q.push(v_it);
                    }
                }
            }
        }

        for (auto &elem : bfs_info)
            bfs_table_.emplace(elem.second.distance_,
                               Info_Node{elem.first, elem.second.predecessor_});
    }

    iterator begin() { return bfs_table_.begin(); }
    const_iterator begin() const { return bfs_table_.begin(); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return bfs_table_.end(); }
    const_iterator end() const { return bfs_table_.end(); }
    const_iterator cend() const { return end(); }

private:

    info_table_type bfs_init(const graph_type &g, vertex_iterator source_it)
    {
        info_table_type bfs_info;
        auto end = g.end();

        for (auto it = g.begin(); it != source_it; ++it)
            bfs_info.emplace(it, Tmp_Node{end});

        bfs_info.emplace(source_it, Tmp_Node{0, Color::gray, end});

        for (auto it = std::next(source_it), ite = g.end(); it != ite; ++it)
            bfs_info.emplace(it, Tmp_Node{end});

        return bfs_info;
    }

    table_type bfs_table_;
};

} // namespace graphs

#endif // INCLUDE_GRAPH_HPP
