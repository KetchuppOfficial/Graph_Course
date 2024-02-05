#include <iostream>

#include "graph.hpp"

template<typename T>
void check_adjacency(const hisi::Directed_Graph<T> &g, const T &n1, const T &n2)
{
    std::cout << std::boolalpha << n1 << " and " << n2 << " are ";
    if (!g.are_adjacent(n1, n2))
        std::cout << "not ";
    std::cout << "adjacent" << std::endl;
}

int main()
{
    hisi::Directed_Graph<int> g;
    g.insert_node(1);
    g.insert_node(2);
    g.insert_node(3);
    g.insert_edge(1, 2);

    std::cout << "n_nodes = " << g.n_nodes() << "; n_edges = " << g.n_edges() << std::endl;

    check_adjacency(g, 1, 2);
    check_adjacency(g, 1, 3);
    check_adjacency(g, 2, 3);

    return 0;
}
