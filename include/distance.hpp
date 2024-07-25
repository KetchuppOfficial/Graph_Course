#ifndef INCLUDE_DISTANCE_HPP
#define INCLUDE_DISTANCE_HPP

#include <concepts>
#include <exception>

namespace graphs
{

struct bad_distance_access : public std::exception
{
    const char *what() const noexcept override { return "bad distance access"; }
};

template<typename T>
concept arithmetic = std::integral<T> && !std::same_as<bool, std::remove_cv_t<T>>;

template<arithmetic T>
class Distance final
{
public:

    using value_type = T;

    Distance() noexcept : is_inf_{true} {}
    Distance(T w) noexcept : w_{w}, is_inf_{false} {} // we want implicit conversion

    bool is_inf() const noexcept { return is_inf_; }
    T operator*() const noexcept { return w_; } // using if is_inf() == true is UB
    T value() const
    {
        if (is_inf())
            throw bad_distance_access{};
        return w_;
    }

    static Distance inf() noexcept { return Distance{}; }

private:

    T w_{};
    bool is_inf_;
};

// Less than

template<arithmetic T>
bool operator<(Distance<T> lhs, Distance<T> rhs) noexcept
{
    if (lhs.is_inf())
        return false;
    else if (rhs.is_inf())
        return true;
    else
        return *lhs < *rhs;
}

template<arithmetic T>
bool operator<(Distance<T> lhs, T rhs) noexcept { return lhs.is_inf() ? false : *lhs < rhs; }

template<arithmetic T>
bool operator<(T lhs, Distance<T> rhs) noexcept { return rhs.is_inf() ? true : lhs < *rhs; }

// Greater than or equal to

template<arithmetic T>
bool operator>=(Distance<T> lhs, Distance<T> rhs) noexcept { return !(lhs < rhs); }

template<arithmetic T>
bool operator>=(Distance<T> lhs, T rhs) noexcept { return !(lhs < rhs); }

template<arithmetic T>
bool operator>=(T lhs, Distance<T> rhs) noexcept { return !(lhs < rhs); }

// Greater than

template<arithmetic T>
bool operator>(Distance<T> lhs, Distance<T> rhs) noexcept { return rhs < lhs; }

template<arithmetic T>
bool operator>(Distance<T> lhs, T rhs) noexcept { return rhs < lhs; }

template<arithmetic T>
bool operator>(T lhs, Distance<T> rhs) noexcept { return rhs < lhs; }

// Less than or equal to

template<arithmetic T>
bool operator<=(Distance<T> lhs, Distance<T> rhs) noexcept { return !(rhs < lhs); }

template<arithmetic T>
bool operator<=(Distance<T> lhs, T rhs) noexcept { return !(rhs < lhs); }

template<arithmetic T>
bool operator<=(T lhs, Distance<T> rhs) noexcept { return !(rhs < lhs); }

// Equal

template<arithmetic T>
bool operator==(Distance<T> lhs, Distance<T> rhs) noexcept
{
    if (lhs.is_inf())
        return rhs.is_inf();
    else
        return rhs.is_inf() ? false : *lhs == *rhs;
}

template<arithmetic T>
bool operator==(Distance<T> lhs, T rhs) noexcept { return lhs.is_inf() ? false : *lhs == rhs; }

template<arithmetic T>
bool operator==(T lhs, Distance<T> rhs) noexcept { return rhs.is_inf() ? false : lhs == *rhs; }

// Sum

template<arithmetic T>
Distance<T> operator+(Distance<T> &lhs, Distance<T> &rhs) noexcept
{
    if (lhs.is_inf() || rhs.is_inf())
        return Distance<T>::inf();
    else
        return *lhs + *rhs;
}

template<arithmetic T>
Distance<T> operator+(Distance<T> lhs, T rhs) noexcept
{
    return lhs.is_inf() ? Distance<T>::inf() : *lhs + rhs;
}

template<arithmetic T>
Distance<T> operator+(T lhs, Distance<T> rhs) noexcept
{
    return rhs + lhs;
}

} // namespace graphs

#endif // INCLUDE_DISTANCE_HPP
