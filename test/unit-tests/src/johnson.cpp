#include <gtest/gtest.h>

#include <unordered_map>

#include "graphs/directed_graph.hpp"
#include "algorithms/johnson.hpp"

TEST(Johnson, Nonunique_Paths)
{
    using G = graphs::Directed_Graph<char>;
    using size_type = graphs::graph_traits<G>::size_type;

    G g;

    auto vertices = {'a', 'b', 'c', 'd'};

    std::unordered_map<char, size_type> it;
    for (auto v : vertices)
        it.emplace(v, g.insert_vertex(v));

    g.insert_edges({{it.at('a'), it.at('b'), 2},
                    {it.at('a'), it.at('c'), -2},
                    {it.at('b'), it.at('a'), -1},
                    {it.at('c'), it.at('a'), 4},
                    {it.at('c'), it.at('d'), 1}});

    graphs::Johnson apsp{g}; // apsp - all-pairs shortest paths

    EXPECT_TRUE(apsp);
    EXPECT_FALSE(apsp.has_negative_weight_cycles());

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

    g.change_weight(it.at('b'), it.at('a'), -3); // creating a negative weight cycle

    graphs::Johnson apsp2{g};

    EXPECT_FALSE(apsp2);
    EXPECT_TRUE(apsp2.has_negative_weight_cycles());
}

// Example from "Introduction to Algorithms" by Thomas H. Cormen and others
TEST(Johnson, From_Cormen)
{
    using G = graphs::Directed_Graph<char>;
    using size_type = graphs::graph_traits<G>::size_type;

    G g;

    auto vertices = {'a', 'b', 'c', 'd', 'e'};

    std::unordered_map<char, size_type> it;
    for (auto v : vertices)
        it.emplace(v, g.insert_vertex(v));

    g.insert_edges({{it.at('a'), it.at('b'), 3},
                    {it.at('a'), it.at('c'), 8},
                    {it.at('a'), it.at('e'), -4},
                    {it.at('b'), it.at('d'), 1},
                    {it.at('b'), it.at('e'), 7},
                    {it.at('c'), it.at('b'), 4},
                    {it.at('d'), it.at('a'), 2},
                    {it.at('d'), it.at('c'), -5},
                    {it.at('e'), it.at('d'), 6}});

    graphs::Johnson apsp{g}; // apsp - all-pairs shortest paths

    EXPECT_TRUE(apsp);
    EXPECT_FALSE(apsp.has_negative_weight_cycles());

    EXPECT_EQ(apsp.distance(it.at('a'), it.at('a')), 0);
    EXPECT_EQ(apsp.distance(it.at('a'), it.at('b')), 1);
    EXPECT_EQ(apsp.distance(it.at('a'), it.at('c')), -3);
    EXPECT_EQ(apsp.distance(it.at('a'), it.at('d')), 2);
    EXPECT_EQ(apsp.distance(it.at('a'), it.at('e')), -4);

    EXPECT_EQ(apsp.distance(it.at('b'), it.at('a')), 3);
    EXPECT_EQ(apsp.distance(it.at('b'), it.at('b')), 0);
    EXPECT_EQ(apsp.distance(it.at('b'), it.at('c')), -4);
    EXPECT_EQ(apsp.distance(it.at('b'), it.at('d')), 1);
    EXPECT_EQ(apsp.distance(it.at('b'), it.at('e')), -1);

    EXPECT_EQ(apsp.distance(it.at('c'), it.at('a')), 7);
    EXPECT_EQ(apsp.distance(it.at('c'), it.at('b')), 4);
    EXPECT_EQ(apsp.distance(it.at('c'), it.at('c')), 0);
    EXPECT_EQ(apsp.distance(it.at('c'), it.at('d')), 5);
    EXPECT_EQ(apsp.distance(it.at('c'), it.at('e')), 3);

    EXPECT_EQ(apsp.distance(it.at('d'), it.at('a')), 2);
    EXPECT_EQ(apsp.distance(it.at('d'), it.at('b')), -1);
    EXPECT_EQ(apsp.distance(it.at('d'), it.at('c')), -5);
    EXPECT_EQ(apsp.distance(it.at('d'), it.at('d')), 0);
    EXPECT_EQ(apsp.distance(it.at('d'), it.at('e')), -2);

    EXPECT_EQ(apsp.distance(it.at('e'), it.at('a')), 8);
    EXPECT_EQ(apsp.distance(it.at('e'), it.at('b')), 5);
    EXPECT_EQ(apsp.distance(it.at('e'), it.at('c')), 1);
    EXPECT_EQ(apsp.distance(it.at('e'), it.at('d')), 6);
    EXPECT_EQ(apsp.distance(it.at('e'), it.at('e')), 0);

    g.change_weight(it.at('a'), it.at('b'), -4); // creating a negative weight cycle

    graphs::Johnson apsp2{g};

    EXPECT_FALSE(apsp2);
    EXPECT_TRUE(apsp2.has_negative_weight_cycles());
}
