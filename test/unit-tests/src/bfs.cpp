#include <gtest/gtest.h>

#include "bfs.hpp"
#include "graph.hpp"

TEST(BFS, From_Cormen)
{
    using G = graphs::Directed_Graph<char>;
    using size_type = graphs::graph_traits<G>::size_type;

    G g;

    auto vertices = {'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

    std::unordered_map<char, size_type> it;
    for (auto v : vertices)
        it.emplace(v, g.insert_vertex(v));

    g.insert_edges({{it.at('s'), it.at('u')}, {it.at('u'), it.at('s')},
                    {it.at('s'), it.at('v')}, {it.at('v'), it.at('s')},
                    {it.at('s'), it.at('r')}, {it.at('r'), it.at('s')},
                    {it.at('u'), it.at('t')}, {it.at('t'), it.at('u')},
                    {it.at('u'), it.at('y')}, {it.at('y'), it.at('u')},
                    {it.at('r'), it.at('t')}, {it.at('t'), it.at('r')},
                    {it.at('r'), it.at('w')}, {it.at('w'), it.at('r')},
                    {it.at('v'), it.at('y')}, {it.at('y'), it.at('v')},
                    {it.at('v'), it.at('w')}, {it.at('w'), it.at('v')},
                    {it.at('x'), it.at('w')}, {it.at('w'), it.at('x')},
                    {it.at('x'), it.at('y')}, {it.at('y'), it.at('x')},
                    {it.at('x'), it.at('z')}, {it.at('z'), it.at('x')},
                    {it.at('w'), it.at('z')}, {it.at('z'), it.at('w')}});

    graphs::BFS bfs{g, it.at('s')};

    std::unordered_map<char, std::size_t> distance =
    {
        {'s', 0},
        {'r', 1}, {'v', 1}, {'u', 1},
        {'t', 2}, {'y', 2}, {'w', 2},
        {'x', 3}, {'z', 3}
    };

    for (auto v : vertices)
        EXPECT_EQ(bfs.distance(it.at(v)), distance.at(v));
}
