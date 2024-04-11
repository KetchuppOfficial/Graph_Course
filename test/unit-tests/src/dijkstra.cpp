#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <vector>

#include "graph.hpp"
#include "dijkstra.hpp"

TEST(Dijkstra, Constructor)
{
    graphs::Directed_Graph<int> g;
    using iterator = decltype(g)::iterator;

    std::array<iterator, 6> it = {g.insert_vertex(1),
                                  g.insert_vertex(2),
                                  g.insert_vertex(3),
                                  g.insert_vertex(4),
                                  g.insert_vertex(5),
                                  g.insert_vertex(6)};

    auto n_vertices = g.n_vertices();

    g.insert_edges({{it[0], it[1], 4},
                    {it[0], it[3], 2},
                    {it[0], it[4], 3},
                    {it[1], it[5], 5},
                    {it[2], it[5], 1},
                    {it[3], it[1], 1},
                    {it[4], it[2], 3},
                    {it[4], it[5], 2}});

    graphs::Dijkstra sssp{g, it[0]};

    int distance[] = {0, 3, 6, 2, 3, 5};
    for (auto i = 0; i != n_vertices; ++i)
        EXPECT_EQ(sssp.distance(it[i]), distance[i]);

    std::array<std::vector<iterator>, 6> ref_path =
    {
        std::vector{it[0]},
        std::vector{it[0], it[3], it[1]},
        std::vector{it[0], it[4], it[2]},
        std::vector{it[0], it[3]},
        std::vector{it[0], it[4]},
        std::vector{it[0], it[4], it[5]}
    };

    for (auto i = 0; i != n_vertices; ++i)
        EXPECT_TRUE(std::ranges::equal(sssp.path_to(it[i]), ref_path[i]));
}
