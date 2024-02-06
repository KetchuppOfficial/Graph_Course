#include <gtest/gtest.h>

#include "graph.hpp"

TEST(Basics, General_Constructors)
{
    hisi::Directed_Graph<int> g1;

    EXPECT_EQ(g1.n_nodes(), 0);
    EXPECT_EQ(g1.n_edges(), 0);
    EXPECT_TRUE(g1.empty());

    hisi::Directed_Graph g2{1, 2, 3, 4};

    EXPECT_FALSE(g2.empty());

    EXPECT_EQ(g2.n_nodes(), 4);

    g2.insert_edge(2, 3);
    g2.insert_edge(1, 4);

    EXPECT_EQ(g2.n_edges(), 2);
}
