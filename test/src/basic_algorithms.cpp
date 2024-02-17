#include <gtest/gtest.h>

#include "graph.hpp"

TEST(Basics, Default_Constructor)
{
    graphs::Directed_Graph<int> g;

    EXPECT_EQ(g.n_vertices(), 0);
    EXPECT_EQ(g.n_edges(), 0);
    EXPECT_TRUE(g.empty());
}

TEST(Basics, Initializer_List_Constructor)
{
    graphs::Directed_Graph g{1, 2, 3, 4};

    EXPECT_FALSE(g.empty());

    EXPECT_EQ(g.n_vertices(), 4);

    g.insert_edge(2, 3);
    g.insert_edge(1, 4);

    EXPECT_EQ(g.n_edges(), 2);
}

TEST(Basics, Vertex_Degree)
{
    graphs::Directed_Graph g{1, 2, 3, 4};

    auto it_1 = g.find_vertex(1);
    auto it_2 = g.find_vertex(2);
    auto it_3 = g.find_vertex(3);
    auto it_4 = g.find_vertex(4);
    g.insert_edge(it_2, it_1);
    g.insert_edge(it_2, it_3);
    g.insert_edge(it_2, it_4);
    g.insert_edge(it_1, it_3);

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
