#ifndef INCLUDE_GRAPHS_KGRAPH_HPP
#define INCLUDE_GRAPHS_KGRAPH_HPP

#include <variant>
#include <cstddef>
#include <initializer_list>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
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
#include <stdexcept>
#include <format>
#include <type_traits>
#include <string>

#include <boost/container_hash/hash.hpp>

#include "utils/graph_traits.hpp"

namespace graphs
{

// copy of standard library's __pair_like concept
template<typename T>
concept pair_like = !std::is_reference_v<T> && requires(T t)
{
    typename std::tuple_size<T>::type;
    requires std::derived_from<std::tuple_size<T>, std::integral_constant<std::size_t, 2>>;
    typename std::tuple_element_t<0, std::remove_const_t<T>>;
    typename std::tuple_element_t<1, std::remove_const_t<T>>;
    { std::get<0>(t) } -> std::convertible_to<const std::tuple_element_t<0, T>&>;
    { std::get<1>(t) } -> std::convertible_to<const std::tuple_element_t<1, T>&>;
};

// inspired by standard library's __pair_like concept
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
concept weighted_edge_initializer =
    trinity_like<T> &&
    std::same_as<std::tuple_element_t<0, T>, std::tuple_element_t<1, T>>;

template<typename T>
concept unweighted_edge_initializer =
    pair_like<T> &&
    std::same_as<std::tuple_element_t<0, T>, std::tuple_element_t<1, T>>;

template<typename T>
concept edge_initializer = weighted_edge_initializer<T> || unweighted_edge_initializer<T>;

template<typename It>
concept weighted_edge_initializer_iterator =
    std::forward_iterator<It> &&
    weighted_edge_initializer<typename std::iterator_traits<It>::value_type>;

template<typename It>
concept unweighted_edge_initializer_iterator =
    std::forward_iterator<It> &&
    unweighted_edge_initializer<typename std::iterator_traits<It>::value_type>;

// Graph representation like in TAOCP 7.2.1.6
template<std::equality_comparable V, typename E>
class KGraph final
{
public:

    using vertex_type = V;
    using edge_type = E;
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

    template<edge_initializer T>
    KGraph(std::initializer_list<T> ilist) : KGraph(ilist.begin(), ilist.end()) {}

    size_type n_vertices() const noexcept { return n_vertices_; }
    size_type n_edges() const noexcept { return (data_.size() - n_vertices()) / 2; }

    bool empty() const noexcept { return n_vertices_ == 0; }

    static consteval bool weighted() { return !std::is_same_v<std::remove_cv_t<E>, void>; }

    auto av_begin(size_type i) const;
    auto av_cbegin(size_type i) const { return av_begin(i); }

    auto av_end(size_type i) const;
    auto av_cend(size_type i) const { return av_end(i); }

    auto ae_begin(size_type i) const;
    auto ae_cbegin(size_type i) const { return ae_begin(i); }

    auto ae_end(size_type i) const;
    auto ae_cend(size_type i) const { return ae_end(i); }

    void dump_as_table(std::ostream &os) const
    {
        auto nothing_dumper = []([[maybe_unused]] const KNode &node){ return 'X'; };
        auto vertex_dumper = [](const KNode &node) -> const V & { return node.get_vertex(); };

        dump_header(os);
        dump_separator(os);
        if constexpr (weighted())
            dump_line(os, vertex_dumper,
                          [](const KNode &node) -> const E & { return node.get_edge(); });
        else
            dump_line(os, vertex_dumper, nothing_dumper);
        dump_separator(os);
        dump_line(os, &KNode::i, &KNode::i, 'i');
        dump_line(os, nothing_dumper,
                      [](const KNode &node){ return *node.tip; }, 't');
        dump_line(os, &KNode::next, &KNode::next, 'n');
        dump_line(os, &KNode::prev, &KNode::prev, 'p');
    }

    void dump_as_dot(std::ostream &os) const
    {
        os << "graph G\n"
              "{\n";

        for (auto v : std::views::iota(0uz, n_vertices()))
            std::println(os, "    node_{} [label = \"{}\"];", v, data_[v].get_vertex());

        os << '\n';

        for (auto e = n_vertices(); e != data_.size(); e += 2)
        {
            if constexpr (weighted())
                std::println(os, "    node_{} -- node_{} [label = \"{}\"]",
                             *data_[e].tip, *data_[e + 1].tip, weight(e));
            else
                std::println(os, "    node_{} -- node_{}", *data_[e].tip, *data_[e + 1].tip);
        }

        os << "}\n";
    }

    std::optional<size_type> find_vertex(const V &v) const
    {
        auto vertices = std::views::take(data_, n_vertices());
        auto it = std::ranges::find(vertices, v,
            [](const KNode &node) -> const V & { return node.get_vertex(); });
        if (it == vertices.end())
            return std::nullopt;
        return std::optional{it - vertices.begin()};
    }

    auto adjacent_vertices(size_type v) const
    {
        return std::ranges::subrange{av_begin(v), av_end(v)};
    }

    // use auto& here because of the possibility for E to be (possibly cv-qualified) void and
    // because such type cannot be referenced.
    auto &weight(size_type e) const { return data_[e].get_edge(); }
    auto &weight(size_type from, size_type to) const
    {
        if (from >= n_vertices())
            throw std::out_of_range{std::format("no vertex with index {}", from)};
        if (to >= n_vertices())
            throw std::out_of_range{std::format("no vertex with index {}", to)};

        auto end = ae_end(from);
        auto it = std::find_if(ae_begin(from), end,
                               [this, to](size_type edge){ return *data_[mate(edge)].tip == to; });
        if (it == end)
            throw std::runtime_error{
                std::format("no edge incident on vertices with indices {} and {}", from, to)};
        return weight(*it);
    }

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
        using edges_set = std::unordered_set<edge_type, boost::hash<edge_type>>;
        using edges_map = std::unordered_map<edge_type, E, boost::hash<edge_type>>;

        std::conditional_t<weighted(), edges_map, edges_set> unique_edges;

        // we don't use structured binding here because in instantiation of structured binding
        // function get() is looked up only via ADL, and in case decltype(*first) is not in
        // namespace std, such function won't be found.
        for (; first != last; ++first)
        {
            size_type i_1 = insert_unique_vertex(std::get<0>(*first));
            size_type i_2 = insert_unique_vertex(std::get<1>(*first));

            if (i_1 > i_2)
                std::swap(i_1, i_2);

            if constexpr (weighted())
                unique_edges.emplace(std::pair{i_1, i_2}, std::get<2>(*first));
            else
                unique_edges.emplace(std::pair{i_1, i_2});
        }

        n_vertices_ = data_.size();

        if constexpr (weighted())
        {
            for (auto &[edge, e] : unique_edges)
            {
                data_.emplace_back(e, data_.size(), edge.first);
                data_.emplace_back(std::move(e), data_.size(), edge.second);
            }
        }
        else
        {
            for (auto &edge : unique_edges)
            {
                data_.emplace_back(std::nullopt, data_.size(), edge.first);
                data_.emplace_back(std::nullopt, data_.size(), edge.second);
            }
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
        std::string vertices_spaces(n_vertices() * kwidth / 2 - 4, ' ');
        std::string edges_spaces(n_edges() * kwidth - 3, ' ');

        os << "    |" << vertices_spaces  << "vertices" << vertices_spaces << '|'
           << edges_spaces << "edges" << edges_spaces << " |" << std::endl;
    }

    void dump_separator(std::ostream &os) const
    {
        os << "----|" << std::string(n_vertices() * kwidth, '-') << '|'
           << std::string(n_edges() * 2 * kwidth, '-') << '|' << std::endl;
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

    size_type mate(size_type i) const
    {
        assert(i >= n_vertices_);
        return ((i - n_vertices_) ^ size_type{1}) + n_vertices_;
    }

    struct KNode final
    {
        template<typename Self>
        auto &&get_vertex(this Self &&self)
        {
            if constexpr (std::is_same_v<V, E>)
                return self.payload;
            else if constexpr (weighted())
                return std::get<V>(self.payload);
            else
                return *self.payload;
        }

        template<typename Self>
        auto &&get_edge(this Self &&self) // is not instantiated for unweighted graphs
        {
            if constexpr (std::is_same_v<V, E>)
                return self.payload;
            else if constexpr (weighted())
                return std::get<E>(self.payload);
        }

        using payload_type =
        std::conditional_t<std::is_same_v<V, E>, V,
                           std::conditional_t<weighted(), std::variant<V, E>, std::optional<V>>>;

        payload_type payload;
        size_type i;
        std::optional<size_type> tip;
        size_type next;
        size_type prev;
    };

    std::vector<KNode> data_;
    size_type n_vertices_ = 0;

    template<typename v, typename e, bool type>
    friend class AdjacentPartIterator;
};

template<weighted_edge_initializer_iterator It> KGraph(It first, It last)
    -> KGraph<std::tuple_element_t<0, typename std::iterator_traits<It>::value_type>,
              std::tuple_element_t<2, typename std::iterator_traits<It>::value_type>>;

template<unweighted_edge_initializer_iterator It> KGraph(It first, It last)
    -> KGraph<std::tuple_element_t<0, typename std::iterator_traits<It>::value_type>, void>;

template<weighted_edge_initializer T> KGraph(std::initializer_list<T>)
    -> KGraph<std::tuple_element_t<0, T>, std::tuple_element_t<2, T>>;

template<unweighted_edge_initializer T> KGraph(std::initializer_list<T>)
    -> KGraph<std::tuple_element_t<0, T>, void>;

template<typename V, typename E, bool type>
class AdjacentPartIterator final
{
public:

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = typename KGraph<V, E>::size_type;
    using reference = const value_type &;
    // pointer is void since C++20

    AdjacentPartIterator() = default;

    AdjacentPartIterator(const KGraph<V, E> &g, value_type i)
        : g_{std::addressof(g)}, edge_index_{i} {}

    value_type operator*() const
    {
        if constexpr (type)
            return *g_->data_[g_->mate(edge_index_)].tip;
        else
            return edge_index_;
    }

    AdjacentPartIterator &operator++() noexcept
    {
        edge_index_ = g_->data_[edge_index_].next;
        return *this;
    }

    AdjacentPartIterator operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    AdjacentPartIterator &operator--() noexcept
    {
        edge_index_ = g_->data_[edge_index_].prev;
        return *this;
    }

    AdjacentPartIterator operator--(int) noexcept
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    bool operator==(const AdjacentPartIterator &rhs) const noexcept
    {
        return g_ == rhs.g_ && edge_index_ == rhs.edge_index_;
    }

private:

    const KGraph<V, E> *g_ = nullptr;
    value_type edge_index_;
};

template<typename V, typename E>
using AdjacentVerticesIterator = AdjacentPartIterator<V, E, true>;

template<typename V, typename E>
using AdjacentEdgesIterator = AdjacentPartIterator<V, E, false>;

static_assert(std::bidirectional_iterator<AdjacentVerticesIterator<int, int>>);
static_assert(std::bidirectional_iterator<AdjacentEdgesIterator<int, int>>);

template<std::equality_comparable V, typename E>
auto KGraph<V, E>::av_begin(size_type v) const
{
    return AdjacentVerticesIterator<V, E>{*this, data_[v].next};
}

template<std::equality_comparable V, typename E>
auto KGraph<V, E>::av_end(size_type v) const { return AdjacentVerticesIterator<V, E>{*this, v}; }

template<std::equality_comparable V, typename E>
auto KGraph<V, E>::ae_begin(size_type v) const
{
    return AdjacentEdgesIterator<V, E>{*this, data_[v].next};
}

template<std::equality_comparable V, typename E>
auto KGraph<V, E>::ae_end(size_type v) const { return AdjacentEdgesIterator<V, E>{*this, v}; }

template<typename V, typename E>
struct graph_traits<KGraph<V, E>>
{
private:

    using G = KGraph<V, E>;

public:

    using size_type = typename G::size_type;
    using vertex_type = V;
    using weight_type = E;

    static constexpr bool is_directed = false;

    static size_type n_vertices(const G &g) { return g.n_vertices(); }
    static size_type n_edges(const G &g) { return g.n_edges(); }

    static auto adjacent_vertices(const G &g, size_type i) { return g.adjacent_vertices(i); }
    static const weight_type &weight(const G &g, size_type from, size_type to)
    {
        return g.weight(from, to);
    }
};

} // namespace graphs

#endif // INCLUDE_GRAPHS_KGRAPH_HPP
