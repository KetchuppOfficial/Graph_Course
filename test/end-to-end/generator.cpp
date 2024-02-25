#include <random>
#include <iostream>
#include <set>
#include <utility>
#include <cstdlib>

int main(int argc, char *argv[])
{
    int n_vertices = std::atoi(argv[1]);
    int n_edges = std::atoi(argv[2]);

    for (auto v = 0; v != n_vertices; ++v)
        std::cout << "V " << v << ' ';
    std::cout << std::endl;

    std::random_device rd;
    std::mt19937_64 gen{rd()};
    std::uniform_int_distribution<int> vertex{0, n_vertices - 1};
    std::set<std::pair<int, int>> edges;

    for (auto e = 0; e != n_edges;)
    {
        int from = vertex(gen);
        int to = vertex(gen);

        if (auto edge = std::pair{from, to}; !edges.contains(edge))
        {
            edges.emplace(edge);
            std::cout << "E " << from << ' ' << to << ' ';
            ++e;
        }
    }

    std::cout << std::endl;

    return 0;
}
