#include <iostream>
#include <stdexcept>
#include <limits>
#include <unordered_map>
#include <chrono>

#include "graph.hpp"
#include "dfs.hpp"

enum Queries : char { vertex = 'V', edge = 'E' };

void check_vertex(int v)
{
    if (std::cin.fail())
    {
        if (v == std::numeric_limits<int>::min())
            throw std::runtime_error{"Vertex value is less than the minimum one"};
        else if (v == std::numeric_limits<int>::max())
            throw std::runtime_error{"Vertex value is greater than the maximum one"};
        else
            throw std::runtime_error{"Error occurred while reading a vertex"};
    }
}

int main()
{
    graphs::Directed_Graph<int> g;
    std::unordered_map<int, graphs::Directed_Graph<int>::iterator> map;

    while (true)
    {
        char query;
        std::cin >> query;

        if (std::cin.eof())
            break;
        else if (std::cin.fail())
            throw std::runtime_error{"Error occurred while reading a query"};

        switch (query)
        {
            case Queries::vertex:
            {
                int v;
                std::cin >> v;

                check_vertex(v);

                if (map.contains(v))
                    throw std::runtime_error{"Vertices must be different in pairs"};

                map.emplace(v, g.insert_vertex(v));
                break;
            }

            case Queries::edge:
            {
                int from, to;
                std::cin >> from >> to;

                check_vertex(from);
                check_vertex(to);

                g.insert_edge(map.at(from), map.at(to));
                break;
            }

            default:
                throw std::runtime_error{"Unknown query"};
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    #ifdef RECURSIVE
    graphs::DFS dfs{g, graphs::recursive{}};
    #else
    graphs::DFS dfs{g};
    #endif

    auto finish = std::chrono::high_resolution_clock::now();

    using ms = std::chrono::milliseconds;
    std::cout << "DFS takes: " << std::chrono::duration_cast<ms>(finish - start).count()
              << " ms" << std::endl;

    return 0;
}
