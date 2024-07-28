#include <gtest/gtest.h>

#include <array>

#include "tuple_like.hpp"
#include "graphs/kgraph.hpp"

TEST(KGraph, Initializer_List_Constructor)
{
    // construction from std::tuple with different vertex and edge types
    graphs::KGraph g_1{std::tuple{1, 2, 'p'},
                       std::tuple{1, 3, 'q'},
                       std::tuple{2, 3, 'r'},
                       std::tuple{2, 4, 's'},
                       std::tuple{3, 4, 't'}};

    static_assert(std::is_same_v<decltype(g_1)::vertex_type, int>);
    static_assert(std::is_same_v<decltype(g_1)::edge_type, char>);
    EXPECT_EQ(g_1.n_vertices(), 4);
    EXPECT_EQ(g_1.n_edges(), 5);

    // construction from std::tuple with identical vertex and edge types
    graphs::KGraph g_2{std::tuple{1, 2, 0},
                       std::tuple{1, 3, 0},
                       std::tuple{2, 3, 0},
                       std::tuple{2, 4, 0},
                       std::tuple{3, 4, 0}};

    static_assert(std::is_same_v<decltype(g_2)::vertex_type, int>);
    static_assert(std::is_same_v<decltype(g_2)::edge_type, int>);
    EXPECT_EQ(g_2.n_vertices(), 4);
    EXPECT_EQ(g_2.n_edges(), 5);

    // construction from a tuple-like standard class
    static_assert(graphs::edge_initializer<std::array<int, 3>>);
    graphs::KGraph g_3{std::array<int, 3>{1, 2, 0},
                       std::array<int, 3>{1, 3, 0},
                       std::array<int, 3>{2, 3, 0},
                       std::array<int, 3>{2, 4, 0},
                       std::array<int, 3>{3, 4, 0}};

    static_assert(std::is_same_v<decltype(g_3)::vertex_type, int>);
    static_assert(std::is_same_v<decltype(g_3)::edge_type, int>);
    EXPECT_EQ(g_3.n_vertices(), 4);
    EXPECT_EQ(g_3.n_edges(), 5);

    // construction from a tuple-like user-defined class
    static_assert(graphs::trinity_like<TupleLike>);
    graphs::KGraph g_4{TupleLike{1, 2, 'p'},
                       TupleLike{1, 3, 'q'},
                       TupleLike{2, 3, 'r'},
                       TupleLike{2, 4, 's'},
                       TupleLike{3, 4, 't'}};

    static_assert(std::is_same_v<decltype(g_4)::vertex_type, int>);
    static_assert(std::is_same_v<decltype(g_4)::edge_type, char>);
    EXPECT_EQ(g_4.n_vertices(), 4);
    EXPECT_EQ(g_4.n_edges(), 5);
}
