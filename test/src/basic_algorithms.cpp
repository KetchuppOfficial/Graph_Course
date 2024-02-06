#include <gtest/gtest.h>

#include "graph.hpp"

TEST(Basics, Capacity)
{
    hisi::Directed_Graph<int> g;

    g.insert_node(1);
    g.insert_node(2);
    g.insert_node(3);
    g.insert_node(4);

    EXPECT_EQ(g.n_nodes(), 4);

    g.insert_edge(2, 3);
    g.insert_edge(1, 4);

    EXPECT_EQ(g.n_edges(), 2);
}
