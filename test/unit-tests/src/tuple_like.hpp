#ifndef TEST_UNIT_TESTS_SRC_TUPLE_LIKE_HPP
#define TEST_UNIT_TESTS_SRC_TUPLE_LIKE_HPP

#include <tuple>

// A structure that behaves similar to std::tuple<int, int, char>
struct TupleLike { int v_1; int v_2; char e; };

namespace std
{

template<>
struct tuple_size<::TupleLike> : integral_constant<size_t, 3> {};

template<>
struct tuple_element<0, ::TupleLike> { using type = int; };

template<>
struct tuple_element<1, ::TupleLike> { using type = int; };

template<>
struct tuple_element<2, ::TupleLike> { using type = char; };

template<size_t I>
const tuple_element<I, ::TupleLike>::type &get(const ::TupleLike &t) noexcept
{
    static_assert(I < 3, "tuple index must be in range");

    if constexpr (I == 0)
        return t.v_1;
    else if constexpr (I == 1)
        return t.v_2;
    else
        return t.e;
}

template<size_t I>
auto &get(::TupleLike &t) noexcept
{
    return const_cast<tuple_element<I,
                      ::TupleLike>::type &>(get<I>(static_cast<const ::TupleLike &>(t)));
}

template<size_t I>
const tuple_element<I, ::TupleLike>::type &&get(const ::TupleLike &&t) noexcept
{
    static_assert(I < 3, "tuple index must be in range");

    if constexpr (I == 0)
        return std::move(t.v_1);
    else if constexpr (I == 1)
        return std::move(t.v_2);
    else
        return std::move(t.e);
}

template<size_t I>
auto &&get(::TupleLike &&t) noexcept
{
    return const_cast<tuple_element<I,
                      ::TupleLike>::type &&>(get<I>(static_cast<const ::TupleLike &&>(t)));
}

} // namespace std

#endif // TEST_UNIT_TESTS_SRC_TUPLE_LIKE_HPP
