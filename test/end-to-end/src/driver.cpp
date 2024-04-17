#include <iostream>
#include <stdexcept>
#include <limits>
#include <unordered_map>
#include <chrono>

#include "graph.hpp"
#include "dfs.hpp"

namespace
{

enum Queries : char { vertex = 'V', edge = 'E' };

void check_stream(std::istream &is, int x)
{
    if (is.fail())
    {
        if (x == std::numeric_limits<int>::min())
            throw std::runtime_error{"int value is less than the minimum one"};
        else if (x == std::numeric_limits<int>::max())
            throw std::runtime_error{"int value is greater than the maximum one"};
        else
            throw std::runtime_error{"Error occurred while reading an int"};
    }
}

graphs::Directed_Graph<int> read_graph(std::istream &is)
{
    graphs::Directed_Graph<int> g;
    std::unordered_map<int, graphs::Directed_Graph<int>::iterator> map;

    while (true)
    {
        char query;
        is >> query;

        if (is.eof())
            break;
        else if (is.fail())
            throw std::runtime_error{"Error occurred while reading a query"};

        switch (query)
        {
            case Queries::vertex:
            {
                int v;
                is >> v;
                check_stream(is, v);

                if (map.contains(v))
                    throw std::runtime_error{"Vertices must be different in pairs"};

                map.emplace(v, g.insert_vertex(v));
                break;
            }

            case Queries::edge:
            {
                int from;
                is >> from;
                check_stream(is, from);

                int to;
                is >> to;
                check_stream(is, to);

                int w;
                is >> w;
                check_stream(is, w);

                g.insert_edge(map.at(from), map.at(to), w);
                break;
            }

            default:
                throw std::runtime_error{"Unknown query"};
        }
    }

    return g;
}

static void test_dfs(const graphs::Directed_Graph<int> &g)
{
    auto start = std::chrono::high_resolution_clock::now();

    #ifdef RECURSIVE
    graphs::DFS dfs{g, graphs::recursive{}};
    #else
    graphs::DFS dfs{g};
    #endif

    auto finish = std::chrono::high_resolution_clock::now();

    using ms = std::chrono::milliseconds;
    #ifdef RECURSIVE
    std::cout << "Recursive DFS takes: ";
    #else
    std::cout << "DFS takes: ";
    #endif
    std::cout << std::chrono::duration_cast<ms>(finish - start).count() << " ms" << std::endl;
}

} // unnamed namespace

int main()
{
    auto graph = read_graph(std::cin);

    #if defined(DFS)
    test_dfs(graph);
    #elif defined(GRAPHIC)
    graph.graphic_dump(std::cout);
    #endif

    return 0;
}
