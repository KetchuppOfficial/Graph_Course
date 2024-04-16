#include <gtest/gtest.h>

#include <unordered_map>

#include "graph.hpp"
#include "johnson.hpp"

TEST(Johnson, Test)
{
    graphs::Directed_Graph<char> g;
    using iterator = decltype(g)::iterator;

    auto vertices = {'a', 'b', 'c', 'd'};

    std::unordered_map<char, iterator> it;
    for (auto v : vertices)
        it.emplace(v, g.insert_vertex(v));

    g.insert_edges({{it.at('a'), it.at('b'), 2},
                    {it.at('a'), it.at('c'), -2},
                    {it.at('b'), it.at('a'), -1},
                    {it.at('c'), it.at('a'), 4},
                    {it.at('c'), it.at('d'), 1}});

    graphs::Johnson apsp{g}; // apsp - all-pairs shortest paths

    EXPECT_EQ(apsp.distance(it.at('a'), it.at('a')), 0);
    EXPECT_EQ(apsp.distance(it.at('a'), it.at('b')), 2);
    EXPECT_EQ(apsp.distance(it.at('a'), it.at('c')), -2);
    EXPECT_EQ(apsp.distance(it.at('a'), it.at('d')), -1);

    EXPECT_EQ(apsp.distance(it.at('b'), it.at('a')), -1);
    EXPECT_EQ(apsp.distance(it.at('b'), it.at('b')), 0);
    EXPECT_EQ(apsp.distance(it.at('b'), it.at('c')), -3);
    EXPECT_EQ(apsp.distance(it.at('b'), it.at('d')), -2);

    EXPECT_EQ(apsp.distance(it.at('c'), it.at('a')), 4);
    EXPECT_EQ(apsp.distance(it.at('c'), it.at('b')), 6);
    EXPECT_EQ(apsp.distance(it.at('c'), it.at('c')), 0);
    EXPECT_EQ(apsp.distance(it.at('c'), it.at('d')), 1);

    auto inf = graphs::Distance<int>::inf();
    EXPECT_EQ(apsp.distance(it.at('d'), it.at('a')), inf);
    EXPECT_EQ(apsp.distance(it.at('d'), it.at('b')), inf);
    EXPECT_EQ(apsp.distance(it.at('d'), it.at('c')), inf);
    EXPECT_EQ(apsp.distance(it.at('d'), it.at('d')), 0);
}
