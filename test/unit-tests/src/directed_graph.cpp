#include <gtest/gtest.h>

#include <initializer_list>
#include <algorithm>
#include <type_traits>
#include <set>

#include "graphs/graph.hpp"

TEST(Directed_Graph, Default_Constructor)
{
    graphs::Directed_Graph<int> g;

    EXPECT_EQ(g.n_vertices(), 0);
    EXPECT_EQ(g.n_edges(), 0);
}

TEST(Directed_Graph, Initializer_List_Constructor)
{
    graphs::Directed_Graph g{1, 2, 3, 4};
    static_assert(std::is_same_v<decltype(g)::vertex_type, int>);

    EXPECT_EQ(g.n_vertices(), 4);
    EXPECT_EQ(g.n_edges(), 0);

    auto list = {1, 2, 3, 4};
    EXPECT_TRUE(std::ranges::equal(g, list)); // check that the order of vertices is preserved
}

TEST(Directed_Graph, Iterator_Constructor)
{
    std::set set{1, 6, 12, 14, 5};
    graphs::Directed_Graph g(set.begin(), set.end());
    static_assert(std::is_same_v<decltype(g)::vertex_type, int>);

    EXPECT_EQ(g.n_vertices(), 5);
    EXPECT_EQ(g.n_edges(), 0);

    EXPECT_TRUE(std::ranges::equal(g, set));
}

TEST(Directed_Graph, Insert_Vertex)
{
    graphs::Directed_Graph<int> g;
    auto list = {1, 2, 3, 4};

    for (auto num : list)
    {
        auto i = g.insert_vertex(num);
        EXPECT_EQ(*std::next(g.begin(), i), num);
        EXPECT_EQ(g.n_vertices(), num);
    }

    EXPECT_EQ(g.n_edges(), 0);

    EXPECT_TRUE(std::ranges::equal(g, list)); // check that the order of vertices is preserved
}

/*
 * 1 ---> 2 ---> 3
 */
TEST(Directed_Graph, Insert_Edge)
{
    graphs::Directed_Graph<int> g;
    auto i_1 = g.insert_vertex(1);
    auto i_2 = g.insert_vertex(2);
    auto i_3 = g.insert_vertex(3);

    g.insert_edge(i_1, i_2);
    g.insert_edge(i_2, i_3);

    EXPECT_EQ(g.n_edges(), 2);

    EXPECT_FALSE(g.are_adjacent(i_1, i_1));
    EXPECT_TRUE(g.are_adjacent(i_1, i_2));
    EXPECT_FALSE(g.are_adjacent(i_1, i_3));

    EXPECT_FALSE(g.are_adjacent(i_2, i_1));
    EXPECT_FALSE(g.are_adjacent(i_2, i_2));
    EXPECT_TRUE(g.are_adjacent(i_2, i_3));

    EXPECT_FALSE(g.are_adjacent(i_3, i_1));
    EXPECT_FALSE(g.are_adjacent(i_3, i_2));
    EXPECT_FALSE(g.are_adjacent(i_3, i_3));

    graphs::Directed_Graph<int> g2{0};
    g2.insert_edge(0, 0);

    EXPECT_EQ(g2.n_edges(), 1);
    EXPECT_TRUE(g2.are_adjacent(0, 0));
}

/*
 *       +-- 1 --+
 *       |   ^    |
 *  2 <--+   |    +--> 3
 *  |        |
 *  +------> 4 --+
 *           ^   |
 *           |   |
 *           +---+
 */
TEST(Directed_Graph, Erase_Vertex)
{
    graphs::Directed_Graph<int> g;
    auto i_1 = g.insert_vertex(1);
    auto i_2 = g.insert_vertex(2);
    auto i_3 = g.insert_vertex(3);
    auto i_4 = g.insert_vertex(4);

    g.insert_edges({{i_1, i_2}, {i_1, i_3}, {i_2, i_4}, {i_4, i_1}, {i_4, i_4}});

    EXPECT_EQ(g.n_vertices(), 4);
    EXPECT_EQ(g.n_edges(), 5);

    EXPECT_FALSE(g.are_adjacent(i_1, i_1));
    EXPECT_TRUE(g.are_adjacent(i_1, i_2));
    EXPECT_TRUE(g.are_adjacent(i_1, i_3));
    EXPECT_FALSE(g.are_adjacent(i_1, i_4));

    EXPECT_FALSE(g.are_adjacent(i_2, i_1));
    EXPECT_FALSE(g.are_adjacent(i_2, i_2));
    EXPECT_FALSE(g.are_adjacent(i_2, i_3));
    EXPECT_TRUE(g.are_adjacent(i_2, i_4));

    EXPECT_FALSE(g.are_adjacent(i_3, i_1));
    EXPECT_FALSE(g.are_adjacent(i_3, i_2));
    EXPECT_FALSE(g.are_adjacent(i_3, i_3));
    EXPECT_FALSE(g.are_adjacent(i_3, i_4));

    EXPECT_TRUE(g.are_adjacent(i_4, i_1));
    EXPECT_FALSE(g.are_adjacent(i_4, i_2));
    EXPECT_FALSE(g.are_adjacent(i_4, i_3));
    EXPECT_TRUE(g.are_adjacent(i_4, i_4));

    g.erase_vertex(i_4);

    EXPECT_EQ(g.n_vertices(), 3);
    EXPECT_EQ(g.n_edges(), 2);

    EXPECT_FALSE(g.are_adjacent(i_1, i_1));
    EXPECT_TRUE(g.are_adjacent(i_1, i_2));
    EXPECT_TRUE(g.are_adjacent(i_1, i_3));

    EXPECT_FALSE(g.are_adjacent(i_2, i_1));
    EXPECT_FALSE(g.are_adjacent(i_2, i_2));
    EXPECT_FALSE(g.are_adjacent(i_2, i_3));

    EXPECT_FALSE(g.are_adjacent(i_3, i_1));
    EXPECT_FALSE(g.are_adjacent(i_3, i_2));
    EXPECT_FALSE(g.are_adjacent(i_3, i_3));
}

/*
 * +--- 1 ---> 2 ---> 3
 * |    ^
 * |    |
 * +----+
 */
TEST(Directed_Graph, Erase_Edge)
{
    graphs::Directed_Graph<int> g;
    auto i_1 = g.insert_vertex(1);
    auto i_2 = g.insert_vertex(2);
    auto i_3 = g.insert_vertex(3);
    g.insert_edges({{i_1, i_2}, {i_2, i_3}, {i_1, i_1}});

    g.erase_edge(i_2, i_3);

    EXPECT_EQ(g.n_vertices(), 3);
    EXPECT_EQ(g.n_edges(), 2);

    EXPECT_TRUE(g.are_adjacent(i_1, i_1));
    EXPECT_TRUE(g.are_adjacent(i_1, i_2));
    EXPECT_FALSE(g.are_adjacent(i_1, i_3));

    EXPECT_FALSE(g.are_adjacent(i_2, i_1));
    EXPECT_FALSE(g.are_adjacent(i_2, i_2));
    EXPECT_FALSE(g.are_adjacent(i_2, i_3));

    EXPECT_FALSE(g.are_adjacent(i_3, i_1));
    EXPECT_FALSE(g.are_adjacent(i_3, i_2));
    EXPECT_FALSE(g.are_adjacent(i_3, i_3));

    g.erase_edge(i_3, i_2); // no effect

    EXPECT_EQ(g.n_vertices(), 3);
    EXPECT_EQ(g.n_edges(), 2);

    EXPECT_TRUE(g.are_adjacent(i_1, i_1));
    EXPECT_TRUE(g.are_adjacent(i_1, i_2));
    EXPECT_FALSE(g.are_adjacent(i_1, i_3));

    EXPECT_FALSE(g.are_adjacent(i_2, i_1));
    EXPECT_FALSE(g.are_adjacent(i_2, i_2));
    EXPECT_FALSE(g.are_adjacent(i_2, i_3));

    EXPECT_FALSE(g.are_adjacent(i_3, i_1));
    EXPECT_FALSE(g.are_adjacent(i_3, i_2));
    EXPECT_FALSE(g.are_adjacent(i_3, i_3));

    g.erase_edge(i_1, i_1);

    EXPECT_EQ(g.n_vertices(), 3);
    EXPECT_EQ(g.n_edges(), 1);

    EXPECT_FALSE(g.are_adjacent(i_1, i_1));
    EXPECT_TRUE(g.are_adjacent(i_1, i_2));
    EXPECT_FALSE(g.are_adjacent(i_1, i_3));

    EXPECT_FALSE(g.are_adjacent(i_2, i_1));
    EXPECT_FALSE(g.are_adjacent(i_2, i_2));
    EXPECT_FALSE(g.are_adjacent(i_2, i_3));

    EXPECT_FALSE(g.are_adjacent(i_3, i_1));
    EXPECT_FALSE(g.are_adjacent(i_3, i_2));
    EXPECT_FALSE(g.are_adjacent(i_3, i_3));
}

/*
 *   +------> 3
 *   |        ^
 *   |        |
 *   |    +-- 2 --+
 *   |    |       |
 *   1 <--+       +--> 4
 */
TEST(Directed_Graph, Vertex_Degree)
{
    graphs::Directed_Graph<int> g;
    auto i_1 = g.insert_vertex(1);
    auto i_2 = g.insert_vertex(2);
    auto i_3 = g.insert_vertex(3);
    auto i_4 = g.insert_vertex(4);

    g.insert_edges({{i_2, i_1},
                    {i_2, i_3},
                    {i_2, i_4},
                    {i_1, i_3}});

    EXPECT_EQ(g.vertex_in_degree(i_1), 1);
    EXPECT_EQ(g.vertex_out_degree(i_1), 1);
    EXPECT_EQ(g.vertex_degree(i_1), 2);

    EXPECT_EQ(g.vertex_in_degree(i_2), 0);
    EXPECT_EQ(g.vertex_out_degree(i_2), 3);
    EXPECT_EQ(g.vertex_degree(i_2), 3);

    EXPECT_EQ(g.vertex_in_degree(i_3), 2);
    EXPECT_EQ(g.vertex_out_degree(i_3), 0);
    EXPECT_EQ(g.vertex_degree(i_3), 2);

    EXPECT_EQ(g.vertex_in_degree(i_4), 1);
    EXPECT_EQ(g.vertex_out_degree(i_4), 0);
    EXPECT_EQ(g.vertex_degree(i_4), 1);
}
