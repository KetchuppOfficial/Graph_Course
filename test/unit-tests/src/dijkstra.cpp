#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <vector>
#include <unordered_map>

#include "graph.hpp"
#include "dijkstra.hpp"

TEST(Dijkstra, Unique_Paths)
{
    graphs::Directed_Graph<int> g;
    using iterator = decltype(g)::iterator;

    std::array<iterator, 6> it = {g.insert_vertex(1),
                                  g.insert_vertex(2),
                                  g.insert_vertex(3),
                                  g.insert_vertex(4),
                                  g.insert_vertex(5),
                                  g.insert_vertex(6)};

    auto n_vertices = g.n_vertices();

    g.insert_edges({{it[0], it[1], 4},
                    {it[0], it[3], 2},
                    {it[0], it[4], 3},
                    {it[1], it[5], 5},
                    {it[2], it[5], 1},
                    {it[3], it[1], 1},
                    {it[4], it[2], 3},
                    {it[4], it[5], 2}});

    graphs::Dijkstra sssp{g, it[0]}; // sssp - single-source shortest paths

    int distance[] = {0, 3, 6, 2, 3, 5};
    for (auto i = 0; i != n_vertices; ++i)
        EXPECT_EQ(sssp.distance(it[i]), distance[i]);

    std::array<std::vector<iterator>, 6> ref_path =
    {
        std::vector{it[0]},
        std::vector{it[0], it[3], it[1]},
        std::vector{it[0], it[4], it[2]},
        std::vector{it[0], it[3]},
        std::vector{it[0], it[4]},
        std::vector{it[0], it[4], it[5]}
    };

    for (auto i = 0; i != n_vertices; ++i)
        EXPECT_TRUE(std::ranges::equal(sssp.path_to(it[i]), ref_path[i]));
}

// Example from "Introduction to Algorithms" by Thomas H. Cormen and others
TEST(Dijkstra, Nonunique_Paths)
{
    graphs::Directed_Graph<char> g;
    using iterator = decltype(g)::iterator;

    std::unordered_map<char, iterator> it = {{'s', g.insert_vertex('s')},
                                             {'t', g.insert_vertex('t')},
                                             {'x', g.insert_vertex('x')},
                                             {'y', g.insert_vertex('y')},
                                             {'z', g.insert_vertex('z')}};

    g.insert_edges({{it.at('s'), it.at('t'), 10},
                    {it.at('s'), it.at('y'), 5},
                    {it.at('t'), it.at('x'), 1},
                    {it.at('t'), it.at('y'), 2},
                    {it.at('x'), it.at('z'), 4},
                    {it.at('y'), it.at('t'), 3},
                    {it.at('y'), it.at('x'), 9},
                    {it.at('y'), it.at('z'), 2},
                    {it.at('z'), it.at('s'), 7},
                    {it.at('z'), it.at('x'), 6}});

    graphs::Dijkstra sssp{g, it.at('s')}; // sssp - single-source shortest paths

    std::unordered_map<char, int> distance = {{'s', 0}, {'t', 8}, {'x', 9}, {'y', 5}, {'z', 7}};

    for (char v : {'s', 't', 'x', 'y', 'z'})
        EXPECT_EQ(sssp.distance(it.at(v)), distance.at(v));
}
