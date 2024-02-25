#include <gtest/gtest.h>

#include <initializer_list>
#include <algorithm>

#include "graph.hpp"

TEST(Graph, Default_Constructor)
{
    graphs::Directed_Graph<int> g;

    EXPECT_EQ(g.n_vertices(), 0);
    EXPECT_EQ(g.n_edges(), 0);
}

TEST(Graph, Initializer_List_Constructor)
{
    graphs::Directed_Graph g{1, 2, 3, 4};

    EXPECT_EQ(g.n_vertices(), 4);
    EXPECT_EQ(g.n_edges(), 0);

    auto list = {1, 2, 3, 4};
    EXPECT_TRUE(std::ranges::equal(g, list)); // check that the order of vertices is preserved
}

TEST(Graph, Insert_Vertex)
{
    graphs::Directed_Graph<int> g;
    auto list = {1, 2, 3, 4};

    for (auto num : list)
    {
        auto it = g.insert_vertex(num);
        EXPECT_EQ(*it, num);
        EXPECT_EQ(g.n_vertices(), num);
    }

    EXPECT_EQ(g.n_edges(), 0);

    EXPECT_TRUE(std::ranges::equal(g, list)); // check that the order of vertices is preserved
}

/*
 * 1 ---> 2 ---> 3
 */
TEST(Graph, Insert_Edge)
{
    graphs::Directed_Graph<int> g;
    auto it_1 = g.insert_vertex(1);
    auto it_2 = g.insert_vertex(2);
    auto it_3 = g.insert_vertex(3);

    g.insert_edge(it_1, it_2);
    g.insert_edge(it_2, it_3);

    EXPECT_EQ(g.n_edges(), 2);

    EXPECT_FALSE(g.are_adjacent(it_1, it_1));
    EXPECT_TRUE(g.are_adjacent(it_1, it_2));
    EXPECT_FALSE(g.are_adjacent(it_1, it_3));

    EXPECT_FALSE(g.are_adjacent(it_2, it_1));
    EXPECT_FALSE(g.are_adjacent(it_2, it_2));
    EXPECT_TRUE(g.are_adjacent(it_2, it_3));

    EXPECT_FALSE(g.are_adjacent(it_3, it_1));
    EXPECT_FALSE(g.are_adjacent(it_3, it_2));
    EXPECT_FALSE(g.are_adjacent(it_3, it_3));

    graphs::Directed_Graph<int> g2{0};
    g2.insert_edge(g2.begin(), g2.begin());

    EXPECT_EQ(g2.n_edges(), 1);
    EXPECT_TRUE(g2.are_adjacent(g2.begin(), g2.begin()));
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
TEST(Graph, Erase_Vertex)
{
    graphs::Directed_Graph<int> g{1, 2, 3, 4};
    auto it_1 = g.begin();
    auto it_2 = std::next(it_1);
    auto it_3 = std::next(it_2);
    auto it_4 = std::next(it_3);

    g.insert_edges({{it_1, it_2}, {it_1, it_3}, {it_2, it_4}, {it_4, it_1}, {it_4, it_4}});

    EXPECT_EQ(g.n_vertices(), 4);
    EXPECT_EQ(g.n_edges(), 5);

    EXPECT_FALSE(g.are_adjacent(it_1, it_1));
    EXPECT_TRUE(g.are_adjacent(it_1, it_2));
    EXPECT_TRUE(g.are_adjacent(it_1, it_3));
    EXPECT_FALSE(g.are_adjacent(it_1, it_4));

    EXPECT_FALSE(g.are_adjacent(it_2, it_1));
    EXPECT_FALSE(g.are_adjacent(it_2, it_2));
    EXPECT_FALSE(g.are_adjacent(it_2, it_3));
    EXPECT_TRUE(g.are_adjacent(it_2, it_4));

    EXPECT_FALSE(g.are_adjacent(it_3, it_1));
    EXPECT_FALSE(g.are_adjacent(it_3, it_2));
    EXPECT_FALSE(g.are_adjacent(it_3, it_3));
    EXPECT_FALSE(g.are_adjacent(it_3, it_4));

    EXPECT_TRUE(g.are_adjacent(it_4, it_1));
    EXPECT_FALSE(g.are_adjacent(it_4, it_2));
    EXPECT_FALSE(g.are_adjacent(it_4, it_3));
    EXPECT_TRUE(g.are_adjacent(it_4, it_4));

    g.erase_vertex(it_4);

    EXPECT_EQ(g.n_vertices(), 3);
    EXPECT_EQ(g.n_edges(), 2);

    EXPECT_FALSE(g.are_adjacent(it_1, it_1));
    EXPECT_TRUE(g.are_adjacent(it_1, it_2));
    EXPECT_TRUE(g.are_adjacent(it_1, it_3));

    EXPECT_FALSE(g.are_adjacent(it_2, it_1));
    EXPECT_FALSE(g.are_adjacent(it_2, it_2));
    EXPECT_FALSE(g.are_adjacent(it_2, it_4));

    EXPECT_FALSE(g.are_adjacent(it_3, it_1));
    EXPECT_FALSE(g.are_adjacent(it_3, it_3));
    EXPECT_FALSE(g.are_adjacent(it_3, it_3));
}

/*
 * +--- 1 ---> 2 ---> 3
 * |    ^
 * |    |
 * +----+
 */
TEST(Graph, Erase_Edge)
{
    graphs::Directed_Graph<int> g;
    auto it_1 = g.insert_vertex(1);
    auto it_2 = g.insert_vertex(2);
    auto it_3 = g.insert_vertex(3);
    g.insert_edges({{it_1, it_2}, {it_2, it_3}, {it_1, it_1}});

    g.erase_edge(it_2, it_3);

    EXPECT_EQ(g.n_vertices(), 3);
    EXPECT_EQ(g.n_edges(), 2);

    EXPECT_TRUE(g.are_adjacent(it_1, it_1));
    EXPECT_TRUE(g.are_adjacent(it_1, it_2));
    EXPECT_FALSE(g.are_adjacent(it_1, it_3));

    EXPECT_FALSE(g.are_adjacent(it_2, it_1));
    EXPECT_FALSE(g.are_adjacent(it_2, it_2));
    EXPECT_FALSE(g.are_adjacent(it_2, it_3));

    EXPECT_FALSE(g.are_adjacent(it_3, it_1));
    EXPECT_FALSE(g.are_adjacent(it_3, it_2));
    EXPECT_FALSE(g.are_adjacent(it_3, it_3));

    g.erase_edge(it_3, it_2); // no effect

    EXPECT_EQ(g.n_vertices(), 3);
    EXPECT_EQ(g.n_edges(), 2);

    EXPECT_TRUE(g.are_adjacent(it_1, it_1));
    EXPECT_TRUE(g.are_adjacent(it_1, it_2));
    EXPECT_FALSE(g.are_adjacent(it_1, it_3));

    EXPECT_FALSE(g.are_adjacent(it_2, it_1));
    EXPECT_FALSE(g.are_adjacent(it_2, it_2));
    EXPECT_FALSE(g.are_adjacent(it_2, it_3));

    EXPECT_FALSE(g.are_adjacent(it_3, it_1));
    EXPECT_FALSE(g.are_adjacent(it_3, it_2));
    EXPECT_FALSE(g.are_adjacent(it_3, it_3));

    g.erase_edge(it_1, it_1);

    EXPECT_EQ(g.n_vertices(), 3);
    EXPECT_EQ(g.n_edges(), 1);

    EXPECT_FALSE(g.are_adjacent(it_1, it_1));
    EXPECT_TRUE(g.are_adjacent(it_1, it_2));
    EXPECT_FALSE(g.are_adjacent(it_1, it_3));

    EXPECT_FALSE(g.are_adjacent(it_2, it_1));
    EXPECT_FALSE(g.are_adjacent(it_2, it_2));
    EXPECT_FALSE(g.are_adjacent(it_2, it_3));

    EXPECT_FALSE(g.are_adjacent(it_3, it_1));
    EXPECT_FALSE(g.are_adjacent(it_3, it_2));
    EXPECT_FALSE(g.are_adjacent(it_3, it_3));
}

/*
 *   +------> 3
 *   |        ^
 *   |        |
 *   |    +-- 2 --+
 *   |    |       |
 *   1 <--+       +--> 4
 */
TEST(Graph, Vertex_Degree)
{
    graphs::Directed_Graph<int> g{1, 2, 3, 4};
    auto it_1 = g.begin();
    auto it_2 = std::next(it_1);
    auto it_3 = std::next(it_2);
    auto it_4 = std::next(it_3);

    g.insert_edges({{it_2, it_1},
                    {it_2, it_3},
                    {it_2, it_4},
                    {it_1, it_3}});

    EXPECT_EQ(g.vertex_in_degree(it_1), 1);
    EXPECT_EQ(g.vertex_out_degree(it_1), 1);
    EXPECT_EQ(g.vertex_degree(it_1), 2);

    EXPECT_EQ(g.vertex_in_degree(it_2), 0);
    EXPECT_EQ(g.vertex_out_degree(it_2), 3);
    EXPECT_EQ(g.vertex_degree(it_2), 3);

    EXPECT_EQ(g.vertex_in_degree(it_3), 2);
    EXPECT_EQ(g.vertex_out_degree(it_3), 0);
    EXPECT_EQ(g.vertex_degree(it_3), 2);

    EXPECT_EQ(g.vertex_in_degree(it_4), 1);
    EXPECT_EQ(g.vertex_out_degree(it_4), 0);
    EXPECT_EQ(g.vertex_degree(it_4), 1);
}
