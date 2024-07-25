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

    Distance() : is_inf_{true} {}
    Distance(T w) : w_{w}, is_inf_{false} {} // we want implicit conversion

    bool is_inf() const noexcept { return is_inf_; }
    T operator*() const noexcept { return w_; } // using if is_inf() == true is UB
    T value() const
    {
        if (is_inf())
            throw bad_distance_access{};
        return w_;
    }

    static Distance inf() { return Distance{}; }

private:

    T w_{};
    bool is_inf_;
};

template<std::integral T>
bool operator<(Distance<T> lhs, Distance<T> rhs)
{
    if (lhs.is_inf())
        return false;
    else if (rhs.is_inf())
        return true;
    else
        return *lhs < *rhs;
}

template<std::integral T>
bool operator>=(Distance<T> lhs, Distance<T> rhs) { return !(lhs < rhs); }

template<std::integral T>
bool operator>(Distance<T> lhs, Distance<T> rhs) { return rhs < lhs; }

template<std::integral T>
bool operator<=(Distance<T> lhs, Distance<T> rhs) { return !(rhs < lhs); }

template<std::integral T>
bool operator==(Distance<T> lhs, Distance<T> rhs)
{
    if (lhs.is_inf())
        return rhs.is_inf();
    else
        return rhs.is_inf() ? false : *lhs == *rhs;
}

template<std::integral T>
bool operator==(Distance<T> lhs, T rhs) { return lhs.is_inf() ? false : *lhs == rhs; }

template<std::integral T>
bool operator==(T lhs, Distance<T> rhs) { return rhs.is_inf() ? false : lhs == *rhs; }

template<std::integral T>
Distance<T> operator+(Distance<T> &lhs, Distance<T> &rhs)
{
    if (lhs.is_inf() || rhs.is_inf())
        return Distance<T>::inf();
    else
        return *lhs + *rhs;
}

template<std::integral T>
Distance<T> operator+(Distance<T> lhs, T rhs)
{
    return lhs.is_inf() ? Distance<T>::inf() : *lhs + rhs;
}

template<std::integral T>
Distance<T> operator+(T lhs, Distance<T> rhs)
{
    return rhs + lhs;
}

} // namespace graphs

#endif // INCLUDE_DISTANCE_HPP
