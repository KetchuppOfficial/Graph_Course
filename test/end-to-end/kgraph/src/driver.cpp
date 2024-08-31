#include <string_view>
#include <iostream>
#include <regex>
#include <format>
#include <vector>
#include <tuple>
#include <string>
#include <cassert>
#include <stdexcept>
#include <print>

#include "graphs/kgraph.hpp"

namespace
{

constexpr std::string_view integer_regex = "(0|-?[1-9]\\d*)";

graphs::KGraph<int, int> get_data(std::istream &is)
{
    static const std::regex line_regex{std::format("\\s*{}\\s+--\\s+{}\\s*,\\s+{}\\s*",
                                                   integer_regex, integer_regex, integer_regex)};

    std::vector<std::tuple<int, int, int>> edges_list;

    std::string line;
    std::smatch match;
    while (std::getline(is, line))
    {
        if (std::regex_match(line, match, line_regex))
        {
            assert(match.size() == 4);

            int v_1 = std::stoi(match[1].str());
            int v_2 = std::stoi(match[2].str());
            int e = std::stoi(match[3].str());

            edges_list.emplace_back(v_1, v_2, e);
        }
        else
            throw std::runtime_error{
                std::format("line \"{}\" doesn't correspond to expected input format", line)};
    }

    return graphs::KGraph(edges_list.begin(), edges_list.end());
}

} // unnamed namespace

int main() try
{
    auto graph = get_data(std::cin);

    graph.dump_as_table(std::cout); // temporary

    return 0;
}
catch (const std::exception &e)
{
    std::println("Error: {}. Abort", e.what());
    return 1;
}
catch (...)
{
    std::println("Unknown exception. Abort");
    return 1;
}
