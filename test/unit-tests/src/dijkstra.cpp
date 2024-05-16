#include <gtest/gtest.h>

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <initializer_list>

#include "graph.hpp"
#include "dijkstra.hpp"

TEST(Dijkstra, Member_Types)
{
    using G = graphs::Directed_Graph<char>;

    static_assert(std::is_same_v<graphs::Distance<int>, graphs::Dijkstra<G>::distance_type>);
}

TEST(Dijkstra, Unique_Paths)
{
    using G = graphs::Directed_Graph<char>;
    using size_type = graphs::graph_traits<G>::size_type;

    G g;

    auto vertices = {'a', 'b', 'c', 'd', 'e', 'f'};

    std::unordered_map<char, size_type> it;
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

    graphs::Dijkstra sssp{g, it.at('a')}; // sssp - single-source shortest paths

    std::unordered_map<char, int> distance = {{'a', 0}, {'b', 3}, {'c', 6},
                                              {'d', 2}, {'e', 3}, {'f', 5}};

    for (auto v : vertices)
        EXPECT_EQ(sssp.distance(it.at(v)), distance.at(v));

    std::unordered_map<char, std::vector<size_type>> ref_path =
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
TEST(Dijkstra, Nonunique_Paths)
{
    using G = graphs::Directed_Graph<char>;
    using size_type = graphs::graph_traits<G>::size_type;

    G g;

    auto vertices = {'s', 't', 'x', 'y', 'z'};

    std::unordered_map<char, size_type> it;
    for (auto v : vertices)
        it.emplace(v, g.insert_vertex(v));

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

    for (char v : vertices)
        EXPECT_EQ(sssp.distance(it.at(v)), distance.at(v));
}

TEST(Dijkstra, Negative_Weights)
{
    using G = graphs::Directed_Graph<char>;
    using size_type = graphs::graph_traits<G>::size_type;

    G g;

    auto vertices = {'a', 'b', 'c'};

    std::unordered_map<char, size_type> it;
    for (auto v : vertices)
        it.emplace(v, g.insert_vertex(v));

    g.insert_edges({{it.at('a'), it.at('b'), 1},
                    {it.at('a'), it.at('c'), 2},
                    {it.at('b'), it.at('c'), -1}});

    EXPECT_TRUE(graphs::Dijkstra<G>::has_negative_weights(g));
    EXPECT_THROW((graphs::Dijkstra{g, it.at('a')}), graphs::Negative_Weights);
}
