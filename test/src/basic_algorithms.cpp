#include <gtest/gtest.h>

#include "graph.hpp"

TEST(Basics, Default_Constructor)
{
    hisi::Directed_Graph<int> g;

    EXPECT_EQ(g.n_vertices(), 0);
    EXPECT_EQ(g.n_edges(), 0);
    EXPECT_TRUE(g.empty());
}

TEST(Basics, Initializer_List_Constructor)
{
    hisi::Directed_Graph g{1, 2, 3, 4};

    EXPECT_FALSE(g.empty());

    EXPECT_EQ(g.n_vertices(), 4);

    g.insert_edge(2, 3);
    g.insert_edge(1, 4);

    EXPECT_EQ(g.n_edges(), 2);
}
