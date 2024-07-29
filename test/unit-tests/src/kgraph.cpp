#include <gtest/gtest.h>

#include <array>
#include <vector>
#include <unordered_set>

#include "tuple_like.hpp"
#include "graphs/kgraph.hpp"

static_assert(graphs::edge_initializer<TupleLike>);
static_assert(graphs::edge_initializer<std::array<int, 3>>);

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

TEST(KGraph, Iterator_Constructor)
{
    // construction from std::tuple with different vertex and edge types
    std::vector vec_1{std::tuple{1, 2, 'p'},
                      std::tuple{1, 3, 'q'},
                      std::tuple{2, 3, 'r'},
                      std::tuple{2, 4, 's'},
                      std::tuple{3, 4, 't'}};

    graphs::KGraph g_1(vec_1.begin(), vec_1.end());

    static_assert(std::is_same_v<decltype(g_1)::vertex_type, int>);
    static_assert(std::is_same_v<decltype(g_1)::edge_type, char>);
    EXPECT_EQ(g_1.n_vertices(), 4);
    EXPECT_EQ(g_1.n_edges(), 5);

    // construction from std::tuple with identical vertex and edge types
    std::vector vec_2{std::tuple{1, 2, 0},
                      std::tuple{1, 3, 0},
                      std::tuple{2, 3, 0},
                      std::tuple{2, 4, 0},
                      std::tuple{3, 4, 0}};

    graphs::KGraph g_2(vec_2.begin(), vec_2.end());

    static_assert(std::is_same_v<decltype(g_2)::vertex_type, int>);
    static_assert(std::is_same_v<decltype(g_2)::edge_type, int>);
    EXPECT_EQ(g_2.n_vertices(), 4);
    EXPECT_EQ(g_2.n_edges(), 5);

    // construction from a tuple-like standard class
    std::vector vec_3{std::array<int, 3>{1, 2, 0},
                      std::array<int, 3>{1, 3, 0},
                      std::array<int, 3>{2, 3, 0},
                      std::array<int, 3>{2, 4, 0},
                      std::array<int, 3>{3, 4, 0}};

    graphs::KGraph g_3(vec_3.begin(), vec_3.end());

    static_assert(std::is_same_v<decltype(g_3)::vertex_type, int>);
    static_assert(std::is_same_v<decltype(g_3)::edge_type, int>);
    EXPECT_EQ(g_3.n_vertices(), 4);
    EXPECT_EQ(g_3.n_edges(), 5);

    // construction from a tuple-like user-defined class
    std::vector vec_4{TupleLike{1, 2, 'p'},
                      TupleLike{1, 3, 'q'},
                      TupleLike{2, 3, 'r'},
                      TupleLike{2, 4, 's'},
                      TupleLike{3, 4, 't'}};

    graphs::KGraph g_4(vec_4.begin(), vec_4.end());

    static_assert(std::is_same_v<decltype(g_4)::vertex_type, int>);
    static_assert(std::is_same_v<decltype(g_4)::edge_type, char>);
    EXPECT_EQ(g_4.n_vertices(), 4);
    EXPECT_EQ(g_4.n_edges(), 5);
}

TEST(KGraph, Weight)
{
    graphs::KGraph g{std::tuple{1, 2, 'p'},
                     std::tuple{1, 3, 'q'},
                     std::tuple{2, 3, 'r'},
                     std::tuple{2, 4, 's'},
                     std::tuple{3, 4, 't'}};

    EXPECT_EQ(g.weight(g.find_vertex(1).value(), g.find_vertex(2).value()), 'p');
    EXPECT_EQ(g.weight(g.find_vertex(2).value(), g.find_vertex(1).value()), 'p');

    EXPECT_EQ(g.weight(g.find_vertex(1).value(), g.find_vertex(3).value()), 'q');
    EXPECT_EQ(g.weight(g.find_vertex(3).value(), g.find_vertex(1).value()), 'q');

    EXPECT_EQ(g.weight(g.find_vertex(2).value(), g.find_vertex(3).value()), 'r');
    EXPECT_EQ(g.weight(g.find_vertex(3).value(), g.find_vertex(2).value()), 'r');

    EXPECT_EQ(g.weight(g.find_vertex(2).value(), g.find_vertex(4).value()), 's');
    EXPECT_EQ(g.weight(g.find_vertex(4).value(), g.find_vertex(2).value()), 's');

    EXPECT_EQ(g.weight(g.find_vertex(3).value(), g.find_vertex(4).value()), 't');
    EXPECT_EQ(g.weight(g.find_vertex(4).value(), g.find_vertex(3).value()), 't');
}

TEST(KGraph, AdjacentVerticesIterator)
{
    graphs::KGraph g{std::tuple{1, 2, 'p'},
                     std::tuple{1, 3, 'q'},
                     std::tuple{2, 3, 'r'},
                     std::tuple{2, 4, 's'},
                     std::tuple{3, 4, 't'}};

    using size_type = decltype(g)::size_type;

    std::vector<std::unordered_set<size_type>> adjacent_vertices_model{{g.find_vertex(2).value(),
                                                                        g.find_vertex(3).value()},
                                                                       {g.find_vertex(1).value(),
                                                                        g.find_vertex(3).value(),
                                                                        g.find_vertex(4).value()},
                                                                       {g.find_vertex(1).value(),
                                                                        g.find_vertex(2).value(),
                                                                        g.find_vertex(4).value()},
                                                                       {g.find_vertex(2).value(),
                                                                        g.find_vertex(3).value()}};

    for (auto [i, v] : std::views::enumerate(std::views::iota(1, 4 + 1)))
    {
        auto index = g.find_vertex(v).value();
        std::unordered_set adjacent_vertices(g.av_begin(index), g.av_end(index));
        EXPECT_EQ(adjacent_vertices, adjacent_vertices_model[i]);
    }
}

TEST(KGraph, AdjacentEdgesIterator)
{
    graphs::KGraph g{std::tuple{1, 2, 'p'},
                     std::tuple{1, 3, 'q'},
                     std::tuple{2, 3, 'r'},
                     std::tuple{2, 4, 's'},
                     std::tuple{3, 4, 't'},
                     std::tuple{1, 4, 'p'}}; // non-unique edge value

    std::vector<std::unordered_multiset<char>> adjacent_edges_model{{'p', 'q', 'p'},
                                                                    {'p', 'r', 's'},
                                                                    {'q', 'r', 't'},
                                                                    {'s', 't', 'p'}};

    for (auto [i, v] : std::views::enumerate(std::views::iota(1, 4 + 1)))
    {
        auto index = g.find_vertex(v).value();
        std::unordered_multiset<char> adjacent_edges(2);
        std::transform(g.ae_begin(index), g.ae_end(index),
                       std::inserter(adjacent_edges, adjacent_edges.end()),
                       [&g](auto e){ return g.weight(e); });
        EXPECT_EQ(adjacent_edges, adjacent_edges_model[i]);
    }
}
