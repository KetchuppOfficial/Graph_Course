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
    using pointer = vertex_type *;
    using const_pointer = const vertex_type *;
    using const_reference = const vertex_type &;

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
        adjacency_list_.erase(vertex_addr(it));
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
        adjacency_list_[vertex_addr(from_it)].insert(vertex_addr(to_it));
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
        adjacency_list_[vertex_addr(from_it)].erase(vertex_addr(to_it));
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
        auto edges_it = adjacency_list_.find(vertex_addr(from_it));
        if (edges_it == adjacency_list_.end())
            return false;
        else
        {
            auto &edges = edges_it->second;
            return edges.find(vertex_addr(to_it)) != edges.end();
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

    // Breadth-first search (BFS)

    struct BFS_Node final
    {
        enum class Color { white, gray };

        std::optional<std::size_t> distance_;
        const_pointer predecessor_{end()};
        Color color_{Color::white};

        BFS_Node() = default;
        BFS_Node(std::size_t d, Color color) : distance_{d}, color_{color} {}
    };

    void bfs(const_iterator source_it)
    {
        if (source_it == end())
            return;

        std::unordered_map<const_pointer, BFS_Node> bfs_info;

        for (auto it = begin(); it != source_it; ++it)
            bfs_info.emplace(vertex_addr(it));

        const_pointer source_addr = vertex_addr(source_it);
        bfs_info.emplace(source_addr, 0, BFS_Node::Color::gray);

        for (auto it = std::next(source_it), ite = end(); it != ite; ++it)
            bfs_info.emplace(vertex_addr(it));

        std::queue<const_pointer> Q{source_addr};
        while (!Q.empty())
        {
            const_pointer u = Q.front();
            Q.pop();

            auto u_list_it = adjacency_list_.find(u);
            if (u_list_it != adjacency_list_.end())
            {
                for (auto v : u_list_it->second)
                {
                    BFS_Node &v_info = bfs_info[v];
                    if (v_info.color_ == BFS_Node::Color::white)
                    {
                        v_info.color_ = BFS_Node::Color::gray;
                        v_info.distance_ = bfs_info[u].distance_ + 1;
                        v_info.predecessor_ = u;
                        Q.push(v);
                    }
                }
            }
        }
    }

private:

    static pointer vertex_addr(iterator it) { return std::addressof(*it); }
    static const_pointer vertex_addr(const_iterator it) { return std::addressof(*it); }

    vertex_cont vertices_;
    std::unordered_map<const_pointer, std::unordered_set<const_pointer>> adjacency_list_;
};

} // namespace graphs

#endif // INCLUDE_GRAPH_HPP
