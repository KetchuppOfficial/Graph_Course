#include <random>
#include <iostream>
#include <set>
#include <utility>
#include <cstdlib>
#include <stdexcept>

int main(int argc, char *argv[])
{
    if (argc != 3)
        throw std::runtime_error{"The program expects exactly 3 command line arguments"};

    int n_vertices = std::atoi(argv[1]);
    if (n_vertices <= 0)
        throw std::runtime_error{"The number of vertices has to be a positive integer"};

    int n_edges = std::atoi(argv[2]);
    if (n_vertices < 0)
        throw std::runtime_error{"The number of vertices has to be a non-negative integer"};

    if (n_edges > n_vertices * (n_vertices - 1))
        throw std::runtime_error{"Too many edges"};

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
