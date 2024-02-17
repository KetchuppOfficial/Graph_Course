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
#include <stack>
#include <vector>
#include <stdexcept>

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

public:

    Directed_Graph() = default;

    Directed_Graph(std::initializer_list<vertex_type> il)
    {
        std::ranges::copy(il, std::back_inserter(vertices_));
        for (auto it = begin(), ite = end(); it != ite; ++it)
            adjacency_list_.emplace(it, edges_cont{});
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
        vertices_.emplace_front(v);
        auto vertex_it = vertices_.begin();
        adjacency_list_.emplace(vertex_it); // ensure that each node has an empty list of edges

        return vertex_it;
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
        if (auto it = find_vertex(v); it != end())
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
        if (auto from_it = find_vertex(from); from_it != end())
        {
            if (auto to_it = find_vertex(to); to_it != end())
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
        if (auto from_it = find_vertex(from); from_it != end())
        {
            if (auto to_it = find_vertex(to); to_it != end())
                erase_edge(from_it, to_it);
        }
    }

    // Mixed operations

    // O(1)
    bool are_adjacent(const_iterator from_it, const_iterator to_it) const
    {
        auto &edges = adjacency_list_.at(from_it);
        return edges.find(to_it) != edges.end();
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

    // O(1)
    std::pair<edge_iterator, edge_iterator> adjacent_vertices(const_iterator it) const
    {
        auto &edges = adjacency_list_.at(it);
        return std::pair{edges.begin(), edges.end()};
    }

    // O(V)
    size_type vertex_in_degree(const_iterator it) const
    {
        return std::ranges::count_if(adjacency_list_, [it](auto &elem)
        {
            return elem.second.contains(it);
        });
    }

    // O(1)
    size_type vertex_out_degree(const_iterator it) const { return adjacency_list_.at(it).size(); }

    // O(V)
    size_type vertex_degree(const_iterator it) const
    {
        return vertex_in_degree(it) + vertex_out_degree(it);
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

    struct Info_Node final
    {
        distance_type distance_;
        vertex_iterator predecessor_;
        Color color_{Color::white};

        Info_Node(vertex_iterator default_predecessor) : predecessor_{default_predecessor} {}
        Info_Node(std::size_t distance, Color color, vertex_iterator default_predecessor)
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

    using info_table_type = std::unordered_map<vertex_iterator, Info_Node, hash_type>;

public:

    struct BFS_Node final
    {
        vertex_iterator node_it_;
        vertex_iterator predecessor_it_;
    };

    using table_type = std::multimap<distance_type, BFS_Node, Comp>;
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

            auto [begin, end] = g.adjacent_vertices(u_it);
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
        bfs_info.reserve(g.n_vertices());

        auto end = g.end();

        for (auto it = g.begin(); it != source_it; ++it)
            bfs_info.emplace(it, Info_Node{end});

        bfs_info.emplace(source_it, Info_Node{0, Color::gray, end});

        for (auto it = std::next(source_it); it != end; ++it)
            bfs_info.emplace(it, Info_Node{end});

        return bfs_info;
    }

    table_type bfs_table_;
};

template<typename T>
class DFS final
{
    using graph_type = Directed_Graph<T>;
    using vertex_iterator = typename graph_type::const_iterator;
    using hash_type = typename graph_type::iterator_hash;
    using time_t = std::size_t;
    using stack_type = std::stack<vertex_iterator, std::vector<vertex_iterator>>;

    struct DFS_Node final
    {
        vertex_iterator predecessor_;
        time_t discovery_time_;
        time_t finished_time_;
    };

    enum class Color { white, gray };

    struct Info_Node final
    {
        DFS_Node dfs_node_;
        Color color_{Color::white};

        Info_Node(vertex_iterator default_predecessor) : dfs_node_{default_predecessor} {}
    };

    using info_table_type = std::unordered_map<vertex_iterator, Info_Node, hash_type>;

public:

    DFS(const graph_type &g)
    {
        info_table_type dfs_info = dfs_init(g);
        time_t time = 0;

        // s_ stands for "source"
        for (auto s_it = g.begin(), ite = g.end(); s_it != ite; ++s_it)
        {
            if (Info_Node &s_info = dfs_info.find(s_it)->second; s_info.color_ == Color::white)
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
            dfs_table_.emplace_back(info_node.dfs_node_);
    }

private:

    info_table_type dfs_init(const graph_type &g)
    {
        info_table_type dfs_info;
        dfs_info.reserve(g.n_vertices());

        for (auto it = g.begin(), ite = g.end(); it != ite; ++it)
            dfs_info.emplace(it, Info_Node{ite});

        return dfs_info;
    }

    std::vector<DFS_Node> dfs_table_;
};

} // namespace graphs

#endif // INCLUDE_GRAPH_HPP
