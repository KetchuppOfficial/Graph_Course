#ifndef INCLUDE_DISTANCE_HPP
#define INCLUDE_DISTANCE_HPP

#include <concepts>

namespace graphs
{

template<std::integral T>
class Distance final
{
public:

    Distance() : is_inf_{true} {}
    Distance(T w) : w_{w}, is_inf_{false} {} // we want implicit conversion

    bool is_inf() const noexcept { return is_inf_; }
    T value() const { return w_; } // using if is_inf() == true is UB

    static Distance inf() { return Distance{}; }

private:

    T w_;
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
        return lhs.value() < rhs.value();
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
        return rhs.is_inf() ? false : lhs.value() == rhs.value();
}

template<std::integral T>
bool operator==(Distance<T> lhs, T rhs) { return lhs.is_inf() ? false : lhs.value() == rhs; }

template<std::integral T>
bool operator==(T lhs, Distance<T> rhs) { return rhs.is_inf() ? false : lhs == rhs.value(); }

template<std::integral T>
Distance<T> operator+(Distance<T> &lhs, Distance<T> &rhs)
{
    if (lhs.is_inf() || rhs.is_inf())
        return Distance<T>::inf();
    else
        return lhs.value() + rhs.value();
}

template<std::integral T>
Distance<T> operator+(Distance<T> lhs, T rhs)
{
    return lhs.is_inf() ? Distance<T>::inf() : lhs.value() + rhs;
}

template<std::integral T>
Distance<T> operator+(T lhs, Distance<T> rhs)
{
    return rhs + lhs;
}

} // namespace graphs

#endif // INCLUDE_DISTANCE_HPP
