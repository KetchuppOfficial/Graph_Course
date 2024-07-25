#include <gtest/gtest.h>

#include "utils/distance.hpp"

TEST(Distance, Constructors)
{
    graphs::Distance d_1{42};
    EXPECT_FALSE(d_1.is_inf());

    auto inf = graphs::Distance<int>::inf();
    EXPECT_TRUE(inf.is_inf());
}

TEST(Distance, Value)
{
    graphs::Distance d{42};

    EXPECT_EQ(*d, 42);
    EXPECT_EQ(d.value(), 42);

    auto inf = graphs::Distance<int>::inf();

    EXPECT_THROW(inf.value(), graphs::bad_distance_access);
}

TEST(Distance, Equality)
{
    graphs::Distance d_1{10};
    graphs::Distance d_2{42};
    graphs::Distance d_3{42};
    auto inf = graphs::Distance<int>::inf();

    EXPECT_NE(d_1, d_2);
    EXPECT_NE(d_2, d_1);

    EXPECT_EQ(d_2, d_3);
    EXPECT_EQ(d_3, d_2);

    EXPECT_NE(d_1, inf);
    EXPECT_NE(inf, d_1);
    EXPECT_NE(d_2, inf);
    EXPECT_NE(inf, d_2);
    EXPECT_NE(d_3, inf);
    EXPECT_NE(inf, d_3);

    EXPECT_EQ(d_1, 10);
    EXPECT_EQ(10, d_1);
    EXPECT_NE(d_1, 42);
    EXPECT_NE(42, d_1);

    EXPECT_NE(inf, 0);
    EXPECT_NE(0, inf);
}

TEST(Distance, Ordering)
{
    graphs::Distance d_1{10};
    graphs::Distance d_2{42};
    auto inf = graphs::Distance<int>::inf();

    EXPECT_LT(d_1, inf);
    EXPECT_LT(d_2, inf);
    EXPECT_LT(0, inf);

    EXPECT_GT(inf, d_1);
    EXPECT_GT(inf, d_2);
    EXPECT_GT(inf, 0);

    EXPECT_LT(d_1, d_2);
    EXPECT_LT(0, d_1);
    EXPECT_LT(0, d_2);

    EXPECT_GT(d_2, d_1);
    EXPECT_GT(d_1, 0);
    EXPECT_GT(d_2, 0);
}

TEST(Distance, Sum)
{
    graphs::Distance d_1{10};
    graphs::Distance d_2{42};
    auto inf = graphs::Distance<int>::inf();

    EXPECT_EQ(d_1 + d_2, 52);
    EXPECT_EQ(d_2 + d_1, 52);
    EXPECT_EQ(d_1 + 42, 52);
    EXPECT_EQ(42 + d_1, 52);

    EXPECT_EQ(d_1 + inf, inf);
    EXPECT_EQ(inf + d_1, inf);
    EXPECT_EQ(inf + 10, inf);
    EXPECT_EQ(10 + inf, inf);
}
