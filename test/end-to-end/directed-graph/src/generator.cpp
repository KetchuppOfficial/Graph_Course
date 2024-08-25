#include <random>
#include <iostream>
#include <set>
#include <utility>
#include <cstdlib>
#include <stdexcept>
#include <optional>

#include <boost/program_options.hpp>

namespace po = boost::program_options;
class Options
{
public:

    Options(int argc, char *argv[])
    {
        po::options_description desc{"Allowed options"};

        desc.add_options()
            ("help", "produce help message")
            ("no-self-loops", "generate graph without self-loops")
            ("n-vertices", po::value<int>(), "set the number of vertices")
            ("n-edges", po::value<int>(), "set the number of edges")
            ("max-weight-modulo", po::value<int>(), "set the maximal weight modulo");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            help_ = true;
            std::cout << desc << std::endl;
        }
        else
        {
            help_ = false;

            if (vm.count("n-vertices"))
            {
                v_ = vm["n-vertices"].as<int>();
                if (v_ <= 0)
                    throw std::runtime_error{"The number of vertices must be a positive number"};
            }
            else
                throw std::runtime_error{"The number of vertices must be provided"};

            if (vm.count("n-edges"))
            {
                e_ = vm["n-edges"].as<int>();
                if (e_ <= 0)
                    throw std::runtime_error{"The number of edges must be a positive number"};
            }
            else
                throw std::runtime_error{"The number of edges must be provided"};

            no_self_loops_ = vm.count("no-self-loops");

            if (no_self_loops_)
            {
                if (e_ > v_ * (v_ - 1))
                    throw std::runtime_error{"Too many edges"};
            }
            else if (e_ > v_ * v_)
                throw std::runtime_error{"Too many edges"};

            if (vm.count("max-weight-modulo"))
            {
                max_w_ = vm["max-weight-modulo"].as<int>();
                if (max_w_ <= 0)
                    throw std::runtime_error{"The maximal weight modulo must be a positive number"};
            }
            else
                max_w_ = 20;
        }
    }

    int n_vertices() const noexcept { return v_; }
    int n_edges() const noexcept { return e_; }
    int max_weight_modulo() const noexcept { return max_w_; }
    bool no_self_loops() const noexcept { return no_self_loops_; }
    bool help() const noexcept { return help_; }

private:

    int v_;
    int e_;
    int max_w_;
    bool no_self_loops_;
    bool help_;
};

int main(int argc, char *argv[])
{
    Options opts{argc, argv};
    if (opts.help())
        return 0;

    for (auto v = 0; v != opts.n_vertices(); ++v)
        std::cout << "V " << v << ' ';
    std::cout << std::endl;

    std::random_device rd;
    std::mt19937_64 gen{rd()};
    std::uniform_int_distribution<int> vertex{0, opts.n_vertices() - 1};
    std::uniform_int_distribution<int> weight{-opts.max_weight_modulo(), opts.max_weight_modulo()};
    std::set<std::pair<int, int>> edges;

    for (auto e = 0; e != opts.n_edges();)
    {
        int from = vertex(gen);
        int to = vertex(gen);

        if (auto edge = std::pair{from, to}; !edges.contains(edge))
        {
            if (!opts.no_self_loops() || from != to)
            {
                edges.emplace(edge);
                std::cout << "E " << from << ' ' << to << ' ' << weight(gen) << ' ';
                ++e;
            }
        }
    }

    std::cout << std::endl;

    return 0;
}
