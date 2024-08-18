#ifndef TEST_UNIT_TESTS_SRC_PAIR_LIKE_HPP
#define TEST_UNIT_TESTS_SRC_PAIR_LIKE_HPP

#include <tuple>

// A structure that behaves similar to std::pair<int, int>
struct PairLike { int v_1; int v_2; };

namespace std
{

template<>
struct tuple_size<::PairLike> : integral_constant<size_t, 2> {};

template<>
struct tuple_element<0, ::PairLike> { using type = int; };

template<>
struct tuple_element<1, ::PairLike> { using type = int; };

template<size_t I>
const tuple_element<I, ::PairLike>::type &get(const ::PairLike &t) noexcept
{
    static_assert(I < 2, "tuple index must be in range");

    if constexpr (I == 0)
        return t.v_1;
    else
        return t.v_2;
}

template<size_t I>
auto &get(::PairLike &t) noexcept
{
    return const_cast<tuple_element<I,
                      ::PairLike>::type &>(get<I>(static_cast<const ::PairLike &>(t)));
}

template<size_t I>
const tuple_element<I, ::PairLike>::type &&get(const ::PairLike &&t) noexcept
{
    static_assert(I < 2, "tuple index must be in range");

    if constexpr (I == 0)
        return std::move(t.v_1);
    else
        return std::move(t.v_2);
}

template<size_t I>
auto &&get(::PairLike &&t) noexcept
{
    return const_cast<tuple_element<I,
                      ::PairLike>::type &&>(get<I>(static_cast<const ::PairLike &&>(t)));
}

} // namespace std

#endif // TEST_UNIT_TESTS_SRC_PAIR_LIKE_HPP
