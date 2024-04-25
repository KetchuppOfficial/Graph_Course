#include <gtest/gtest.h>

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <initializer_list>

#include "graph.hpp"
#include "bellman_ford.hpp"

TEST(Bellman_Ford, Member_Types)
{
    using G = graphs::Directed_Graph<char>;

    static_assert(std::is_same_v<graphs::Distance<int>, graphs::Bellman_Ford<G>::distance_type>);
}

TEST(Bellman_Ford, Unique_Paths)
{
    using G = graphs::Directed_Graph<char>;
    using iterator = typename graphs::graph_traits<G>::vertex_iterator;

    G g;

    auto vertices = {'a', 'b', 'c', 'd', 'e', 'f'};

    std::unordered_map<char, iterator> it;
    for (auto v : vertices)
        it.emplace(v, g.insert_vertex(v));

    g.insert_edges({{it.at('a'), it.at('b'), 4},
                    {it.at('a'), it.at('d'), 2},
                    {it.at('a'), it.at('e'), 3},
                    {it.at('b'), it.at('f'), 5},
                    {it.at('c'), it.at('f'), 1},
                    {it.at('d'), it.at('b'), 1},
                    {it.at('e'), it.at('c'), 3},
                    {it.at('e'), it.at('f'), 2}});

    graphs::Bellman_Ford sssp{g, it.at('a')}; // sssp - single-source shortest paths

    EXPECT_TRUE(sssp);
    EXPECT_FALSE(sssp.has_negative_weight_cycles());

    std::unordered_map<char, int> distance = {{'a', 0}, {'b', 3}, {'c', 6},
                                              {'d', 2}, {'e', 3}, {'f', 5}};

    for (auto v : vertices)
        EXPECT_EQ(sssp.distance(it.at(v)), distance.at(v));

    std::unordered_map<char, std::vector<iterator>> ref_path =
    {
        {'a', std::vector{it.at('a')}},
        {'b', std::vector{it.at('a'), it.at('d'), it.at('b')}},
        {'c', std::vector{it.at('a'), it.at('e'), it.at('c')}},
        {'d', std::vector{it.at('a'), it.at('d')}},
        {'e', std::vector{it.at('a'), it.at('e')}},
        {'f', std::vector{it.at('a'), it.at('e'), it.at('f')}}
    };

    for (auto v : vertices)
        EXPECT_TRUE(std::ranges::equal(sssp.path_to(it.at(v)), ref_path.at(v)));
}

// Example from "Introduction to Algorithms" by Thomas H. Cormen and others
TEST(Bellman_Ford, Nonunique_Paths)
{
    using G = graphs::Directed_Graph<char>;
    using iterator = typename graphs::graph_traits<G>::vertex_iterator;

    G g;

    auto vertices = {'s', 't', 'x', 'y', 'z'};

    std::unordered_map<char, iterator> it;
    for (auto v : vertices)
        it.emplace(v, g.insert_vertex(v));

    g.insert_edges({{it.at('s'), it.at('t'), 6},
                    {it.at('s'), it.at('y'), 7},
                    {it.at('t'), it.at('x'), 5},
                    {it.at('t'), it.at('y'), 8},
                    {it.at('t'), it.at('z'), -4},
                    {it.at('x'), it.at('t'), -2},
                    {it.at('y'), it.at('x'), -3},
                    {it.at('y'), it.at('z'), 9},
                    {it.at('z'), it.at('s'), 2},
                    {it.at('z'), it.at('x'), 7}});

    graphs::Bellman_Ford sssp{g, it.at('s')}; // sssp - single-source shortest paths

    EXPECT_TRUE(sssp);
    EXPECT_FALSE(sssp.has_negative_weight_cycles());

    std::unordered_map<char, int> distance = {{'s', 0}, {'t', 2}, {'x', 4}, {'y', 7}, {'z', -2}};

    for (char v : vertices)
        EXPECT_EQ(sssp.distance(it.at(v)), distance.at(v));

    g.change_weight(it.at('x'), it.at('t'), -6); // creating a negative weight cycle

    graphs::Bellman_Ford sssp2{g, it.at('s')};

    EXPECT_FALSE(sssp2);
    EXPECT_TRUE(sssp2.has_negative_weight_cycles());
}

TEST(Bellman_Ford, Negative_Weights)
{
    using G = graphs::Directed_Graph<char>;
    using iterator = typename graphs::graph_traits<G>::vertex_iterator;

    G g;

    auto vertices = {'a', 'b', 'c'};

    std::unordered_map<char, iterator> it;
    for (auto v : vertices)
        it.emplace(v, g.insert_vertex(v));

    g.insert_edges({{it.at('a'), it.at('b'), 1},
                    {it.at('a'), it.at('c'), 2},
                    {it.at('b'), it.at('c'), -1}});

    graphs::Bellman_Ford sssp{g, it.at('a')}; // sssp - single-source shortest paths

    EXPECT_TRUE(sssp);
    EXPECT_FALSE(sssp.has_negative_weight_cycles());

    std::unordered_map<char, int> distance = {{'a', 0}, {'b', 1}, {'c', 0}};

    for (auto v : vertices)
        EXPECT_EQ(sssp.distance(it.at(v)), distance.at(v));
}
