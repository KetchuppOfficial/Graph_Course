#ifndef TEST_UNIT_TESTS_SRC_TUPLE_LIKE_HPP
#define TEST_UNIT_TESTS_SRC_TUPLE_LIKE_HPP

#include <tuple>

// A structure that behaves similar to std::tuple<int, int, char>
struct TupleLike { int v_1; int v_2; char e; };

#define GET_IMPL                                                                                   \
    static_assert(I < 3, "tuple index must be in range");                                          \
                                                                                                   \
    if constexpr (I == 0)                                                                          \
        return t.v_1;                                                                              \
    else if constexpr (I == 1)                                                                     \
        return t.v_2;                                                                              \
    else                                                                                           \
        return t.e;

#define MOVE_GET_IMPL                                                                              \
    static_assert(I < 3, "tuple index must be in range");                                          \
                                                                                                   \
    if constexpr (I == 0)                                                                          \
        return std::move(t.v_1);                                                                   \
    else if constexpr (I == 1)                                                                     \
        return std::move(t.v_2);                                                                   \
    else                                                                                           \
        return std::move(t.e);

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
tuple_element<I, ::TupleLike>::type &get(::TupleLike &t) noexcept { GET_IMPL }

template<size_t I>
const tuple_element<I, ::TupleLike>::type &get(const ::TupleLike &t) noexcept { GET_IMPL }

template<size_t I>
tuple_element<I, ::TupleLike>::type &&get(::TupleLike &&t) noexcept { MOVE_GET_IMPL }

template<size_t I>
const tuple_element<I, ::TupleLike>::type &&get(const ::TupleLike &&t) noexcept { MOVE_GET_IMPL }

} // namespace std

#undef GET_IMPL
#undef CONST_GET_IMPL

#endif // TEST_UNIT_TESTS_SRC_TUPLE_LIKE_HPP
