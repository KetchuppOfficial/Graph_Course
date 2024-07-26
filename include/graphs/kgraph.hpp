#ifndef INCLUDE_GRAPHS_KGRAPH_HPP
#define INCLUDE_GRAPHS_KGRAPH_HPP

#include <variant>
#include <cstddef>
#include <initializer_list>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <ostream>
#include <ranges>
#include <iomanip>
#include <functional>
#include <optional>
#include <cassert>
#include <vector>
#include <iterator>
#include <memory>

#include <boost/container_hash/hash.hpp>

#include "graph_traits.hpp"

namespace graphs
{

// inspired by standard library __pair_like concept
template<typename T>
concept trinity_like = !std::is_reference_v<T> && requires(T t)
{
    typename std::tuple_size<T>::type;
    requires std::derived_from<std::tuple_size<T>, std::integral_constant<std::size_t, 3>>;
    typename std::tuple_element_t<0, std::remove_const_t<T>>;
    typename std::tuple_element_t<1, std::remove_const_t<T>>;
    typename std::tuple_element_t<2, std::remove_const_t<T>>;
    { std::get<0>(t) } -> std::convertible_to<const std::tuple_element_t<0, T>&>;
    { std::get<1>(t) } -> std::convertible_to<const std::tuple_element_t<1, T>&>;
    { std::get<2>(t) } -> std::convertible_to<const std::tuple_element_t<2, T>&>;
};

template<typename T>
concept edge_initializer =
    trinity_like<T> &&
    std::same_as<std::tuple_element_t<0, T>, std::tuple_element_t<1, T>>;

// Graph representation like in TAOCP 7.2.1.6
template<typename V, typename E>
class KGraph final
{
    using payload_type = std::variant<V, E>;

public:

    using size_type = std::size_t;

    template<typename It>
    requires std::forward_iterator<It> &&
             edge_initializer<typename std::iterator_traits<It>::value_type>
    KGraph(It first, It last)
    {
        if (first == last)
            return;

        fill_payload_index_and_tip(first, last);
        fill_incident_edges_lists();
    }

    template<trinity_like T>
    KGraph(std::initializer_list<T> ilist) : KGraph(ilist.begin(), ilist.end()) {}

    size_type n_vertices() const noexcept { return n_vertices_; }
    size_type n_edges() const noexcept { return (data_.size() - n_vertices()) / 2; }

    bool empty() const noexcept { return n_vertices_ == 0; }

    void dump_as_table(std::ostream &os) const
    {
        dump_header(os);
        dump_separator(os);
        dump_line(os, [](const KNode &node) -> const V & { return std::get<V>(node.payload); },
                      [](const KNode &node) -> const E & { return std::get<E>(node.payload); });
        dump_separator(os);
        dump_line(os, &KNode::i, &KNode::i, 'i');
        dump_line(os, []([[maybe_unused]] const KNode &node){ return 'X'; },
                      [](const KNode &node){ return *node.tip; }, 't');
        dump_line(os, &KNode::next, &KNode::next, 'n');
        dump_line(os, &KNode::prev, &KNode::prev, 'p');
    }

    void dump_as_dot(std::ostream &os) const
    {
        os << "graph G\n"
              "{\n";

        for (auto v : std::views::iota(0uz, n_vertices()))
            std::println(os, "    node_{} [label = \"{}\"];", v, std::get<V>(data_[v].payload));

        os << '\n';

        for (auto e = n_vertices(); e != data_.size(); e += 2)
            std::println(os, "    node_{} -- node_{} [label = \"{}\"]",
                         *data_[e].tip, *data_[e + 1].tip, std::get<E>(data_[e].payload));

        os << "}\n";
    }

    auto adjacent_vertices(size_type v) const;

private:

    static constexpr std::size_t kwidth = 8;
    static_assert(kwidth % 2 == 0, "width parameter of the stream should be an even number");

    // to be used in constructor only
    template<std::forward_iterator It>
    void fill_payload_index_and_tip(It first, It last)
    {
        assert(data_.empty());
        assert(n_vertices_ == 0);

        std::unordered_map<V, size_type> unique_vertices(std::distance(first, last) * 2);
        auto insert_unique_vertex = [&](const V &v)
        {
            if (auto it = unique_vertices.find(v); it == unique_vertices.end())
            {
                size_type i = data_.size();
                unique_vertices.emplace(v, i);
                data_.emplace_back(v, i);
                return i;
            }
            else
                return it->second;
        };

        using edge_type = std::pair<size_type, size_type>;
        std::unordered_map<edge_type, E, boost::hash<edge_type>> unique_edges;

        for (auto &[v_1, v_2, e] : std::ranges::subrange(first, last))
        {
            size_type i_1 = insert_unique_vertex(v_1);
            size_type i_2 = insert_unique_vertex(v_2);

            if (i_1 > i_2)
                std::swap(i_1, i_2);

            unique_edges.emplace(std::pair{i_1, i_2}, std::move(e));
        }

        n_vertices_ = data_.size();

        for (auto &[edge, e] : unique_edges)
        {
            data_.emplace_back(e, data_.size(), edge.first);
            data_.emplace_back(std::move(e), data_.size(), edge.second);
        }
    }

    void fill_incident_edges_lists()
    {
        auto first_edge_it = std::next(data_.begin(), n_vertices());
        for (auto v : std::views::iota(0uz, n_vertices()))
        {
            auto it = std::ranges::find(first_edge_it, data_.end(), v, &KNode::tip);

            assert(it != data_.end());
            assert(it->tip.has_value());

            data_[v].next = it->i;
            it->prev = v;

            while (true)
            {
                auto next_it = std::ranges::find(std::next(it), data_.end(), v, &KNode::tip);
                if (next_it == data_.end())
                    break;

                assert(next_it->tip.has_value());
                it->next = next_it->i;
                next_it->prev = it->i;
                it = next_it;
            }

            data_[v].prev = it->i;
            it->next = v;
        }
    }

    void dump_header(std::ostream &os) const
    {
        os << "    |";
        for (auto _ : std::views::iota(0uz, n_vertices() * kwidth / 2 - 4))
            os << ' ';
        os << "vertices";
        for (auto _ : std::views::iota(0uz, n_vertices() * kwidth / 2 - 4))
            os << ' ';

        os << '|';
        for (auto _ : std::views::iota(0uz, n_edges() * kwidth - 3))
            os << ' ';
        os << "edges";
        for (auto _ : std::views::iota(0uz, n_edges() * kwidth - 2))
            os << ' ';

        os << '|' << std::endl;
    }

    void dump_separator(std::ostream &os) const
    {
        os << "----|" ;
        for (auto _ : std::views::iota(0uz, n_vertices() * kwidth))
            os << '-';
        os << '|';
        for (auto _ : std::views::iota(0uz, n_edges() * 2 * kwidth))
            os << '-';
        os << '|' << std::endl;
    }

    template<typename VDumper, typename EDumper>
    void dump_line(std::ostream &os, VDumper vd, EDumper ed, char line_name = ' ') const
    {
        os << "   " << line_name << '|';
        for (const auto &node : std::views::take(data_, n_vertices()))
            os << std::setw(kwidth) << std::invoke(vd, node);
        os << '|';
        for (const auto &node : std::views::drop(data_, n_vertices()))
            os << std::setw(kwidth) << std::invoke(ed, node);
        os << '|' << std::endl;
    }

    struct KNode final
    {
        payload_type payload;
        size_type i;
        std::optional<size_type> tip;
        size_type next = 0;
        size_type prev = 0;
    };

    size_type mate(size_type i) const
    {
        assert(i >= n_vertices_);
        return ((i - n_vertices_) ^ size_type{1}) + n_vertices_;
    }

    std::vector<KNode> data_;
    size_type n_vertices_ = 0;

    template<typename v, typename e>
    friend class AdjacentVerticesIterator;
};

template<typename It> KGraph(It first, It last)
    -> KGraph<std::tuple_element_t<0, typename std::iterator_traits<It>::value_type>,
              std::tuple_element_t<2, typename std::iterator_traits<It>::value_type>>;

template<typename V, typename E>
class AdjacentVerticesIterator final
{
public:

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = typename KGraph<V, E>::size_type;
    using reference = const value_type &;
    // pointer is void since C++20

    AdjacentVerticesIterator() = default;

    AdjacentVerticesIterator(const KGraph<V, E> &g, value_type i)
        : g_{std::addressof(g)}, index_{i} {}

    value_type operator*() const { return *g_->data_[g_->mate(index_)].tip; }

    AdjacentVerticesIterator &operator++() noexcept
    {
        index_ = g_->data_[index_].next;
        return *this;
    }

    AdjacentVerticesIterator operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    AdjacentVerticesIterator &operator--() noexcept
    {
        index_ = g_->data_[index_].prev;
        return *this;
    }

    AdjacentVerticesIterator operator--(int) noexcept
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    bool operator==(const AdjacentVerticesIterator &rhs) const noexcept
    {
        return g_ == rhs.g_ && index_ == rhs.index_;
    }

private:

    const KGraph<V, E> *g_ = nullptr;
    value_type index_;
};

static_assert(std::bidirectional_iterator<AdjacentVerticesIterator<int, int>>);

template<typename V, typename E>
auto KGraph<V, E>::adjacent_vertices(size_type v) const
{
    return std::ranges::subrange{AdjacentVerticesIterator{*this, data_[v].next},
                                 AdjacentVerticesIterator{*this, v}};
}

template<typename V, typename E>
struct graph_traits<KGraph<V, E>>
{
private:

    using G = KGraph<V, E>;

public:

    using size_type = typename G::size_type;
    using vertex_type = V;
    using weight_type = E;

    static size_type n_vertices(const G &g) { return g.n_vertices(); }
    static size_type n_edges(const G &g) { return g.n_edges(); }

    static auto adjacent_vertices(const G &g, size_type i) { return g.adjacent_vertices(i); }
};

} // namespace graphs

#endif // INCLUDE_GRAPHS_KGRAPH_HPP
