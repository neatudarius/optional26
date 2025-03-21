// tests/beman/optional/optional.t.cpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/optional/optional.hpp>

#include <beman/optional/optional.hpp> // test 2nd include OK

#include <beman/optional/test_types.hpp>

#include <functional>
#include <ranges>
#include <tuple>
#include <algorithm>
#include <any>

#include <gtest/gtest.h>

TEST(OptionalTest, Constructors) {
    beman::optional::optional<int> i1;
    beman::optional::optional<int> i2{beman::optional::nullopt};
    std::ignore = i1;
    std::ignore = i2;

    int                            i  = 0;
    beman::optional::optional<int> i3 = i;
    std::ignore                       = i3;

    using beman::optional::tests::empty;

    beman::optional::optional<empty> e1;
    beman::optional::optional<int>   e2{beman::optional::nullopt};

    empty                            e{};
    beman::optional::optional<empty> e3 = e;
    std::ignore                         = e1;
    std::ignore                         = e2;
    std::ignore                         = e3;
}

TEST(OptionalTest, Constructors2) {
    beman::optional::optional<int> o1;
    EXPECT_TRUE(!o1);

    beman::optional::optional<int> o2 = beman::optional::nullopt;
    EXPECT_TRUE(!o2);

    beman::optional::optional<int> o3 = 42;
    EXPECT_TRUE(*o3 == 42);

    beman::optional::optional<int> o4 = o3;
    EXPECT_TRUE(*o4 == 42);

    beman::optional::optional<int> o5 = o1;
    EXPECT_TRUE(!o5);

    beman::optional::optional<int> o6 = std::move(o3);
    EXPECT_TRUE(*o6 == 42);

    beman::optional::optional<short> o7 = 42;
    EXPECT_TRUE(*o7 == 42);

    beman::optional::optional<int> o8 = o7;
    EXPECT_TRUE(*o8 == 42);

    beman::optional::optional<int> o9 = std::move(o7);
    EXPECT_TRUE(*o9 == 42);

    {
        beman::optional::optional<int&> o;
        EXPECT_TRUE(!o);

        beman::optional::optional<int&> oo = o;
        EXPECT_TRUE(!oo);
    }

    {
        auto                            i = 42;
        beman::optional::optional<int&> o = i;
        EXPECT_TRUE(o);
        EXPECT_TRUE(*o == 42);

        beman::optional::optional<int&> oo = o;
        EXPECT_TRUE(oo);
        EXPECT_TRUE(*oo == 42);
    }

    using beman::optional::tests::base;

    std::vector<base> v;
    v.emplace_back();
    beman::optional::optional<std::vector<base>> ov = std::move(v);
    EXPECT_TRUE(ov->size() == 1);
}

TEST(OptionalTest, Constructors3) {
    beman::optional::optional<int> ie;
    beman::optional::optional<int> i4 = ie;
    EXPECT_FALSE(i4);

    using beman::optional::tests::base;
    using beman::optional::tests::derived;

    base                            b{1};
    derived                         d(1, 2);
    beman::optional::optional<base> b1{b};
    beman::optional::optional<base> b2{d};

    beman::optional::optional<derived> d2{d};
    beman::optional::optional<base>    b3 = d2;
    beman::optional::optional<base>    b4{d2};
}

TEST(OptionalTest, NonDefaultConstruct) {
    using beman::optional::tests::int_ctor;

    int_ctor                            i = 7;
    beman::optional::optional<int_ctor> v1{};
    beman::optional::optional<int_ctor> v2{i};
    std::ignore = v1;
    std::ignore = v2;
}

TEST(OptionalTest, OptionalOfOptional) {
    using O = beman::optional::optional<int>;
    O                            o;
    beman::optional::optional<O> oo1 = o;
    EXPECT_TRUE(oo1.has_value());
    oo1 = o;
    EXPECT_TRUE(oo1.has_value());
    EXPECT_FALSE(oo1->has_value());

    beman::optional::optional<O> oo2 = std::move(o);
    EXPECT_TRUE(oo2.has_value());
    oo2 = o;
    EXPECT_TRUE(oo2.has_value());
    EXPECT_FALSE(oo2->has_value());

    // emplace constructs the inner optional
    oo2.emplace(beman::optional::nullopt);
    EXPECT_TRUE(oo2.has_value());
    EXPECT_FALSE(oo2->has_value());
    oo2.emplace(beman::optional::in_place, 41);
    EXPECT_TRUE(oo2.has_value());
    EXPECT_TRUE(oo2.value() == 41);
    oo2.emplace(42);
    EXPECT_TRUE(oo2.has_value());
    EXPECT_TRUE(oo2.value() == 42);
    oo2.emplace(o);
    EXPECT_TRUE(oo2.has_value());
    EXPECT_FALSE(oo2->has_value());
    oo2.emplace(O(43));
    EXPECT_TRUE(oo2.has_value());
    EXPECT_TRUE(oo2.value() == 43);
}

TEST(OptionalTest, AssignmentValue) {
    beman::optional::optional<int> o1 = 42;
    beman::optional::optional<int> o2 = 12;
    beman::optional::optional<int> o3;

    o1 = static_cast<beman::optional::optional<int>&>(o1);
    EXPECT_TRUE(*o1 == 42);

    o1 = o2;
    EXPECT_TRUE(*o1 == 12);

    o1 = o3;
    EXPECT_TRUE(!o1);

    o1 = 42;
    EXPECT_TRUE(*o1 == 42);

    o1 = beman::optional::nullopt;
    EXPECT_TRUE(!o1);

    o1 = std::move(o2);
    EXPECT_TRUE(*o1 == 12);

    beman::optional::optional<short> o4 = 42;

    o1 = o4;
    EXPECT_TRUE(*o1 == 42);

    o1 = std::move(o4);
    EXPECT_TRUE(*o1 == 42);

    /*
      template <class U = T>
      constexpr optional& operator=(U&& u)
    */
    short s = 54;
    o1      = s;
    EXPECT_TRUE(*o1 == 54);

    struct not_trivial_copy_assignable {
        int i_;
        constexpr not_trivial_copy_assignable(int i) : i_(i) {}
        constexpr not_trivial_copy_assignable(const not_trivial_copy_assignable&) = default;
        constexpr not_trivial_copy_assignable& operator=(const not_trivial_copy_assignable& rhs) {
            i_ = rhs.i_;
            return *this;
        }
    };

    /*
      optional& operator=(const optional& rhs)
      requires std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T> &&
      (!std::is_trivially_copy_assignable_v<T>)
     */
    beman::optional::optional<not_trivial_copy_assignable> o5{5};
    beman::optional::optional<not_trivial_copy_assignable> o6;

    // Copy into empty optional.
    o6 = o5;
    EXPECT_TRUE(o6);
    EXPECT_TRUE(o6->i_ == 5);

    // Move into empty optional.
    o6.reset();
    o6 = std::move(o5);
    EXPECT_TRUE(o6);
    EXPECT_TRUE(o6->i_ == 5);

    // Copy into engaged optional.
    beman::optional::optional<not_trivial_copy_assignable> o7{7};
    o6 = o7;
    EXPECT_TRUE(o6);
    EXPECT_TRUE(o6->i_ == 7);

    // Move into engaged optional.
    beman::optional::optional<not_trivial_copy_assignable> o8{8};
    o6 = std::move(o8);
    EXPECT_TRUE(o6);
    EXPECT_TRUE(o6->i_ == 8);

    // Copy from empty into engaged optional.
    o5.reset();
    o7 = o5;
    EXPECT_FALSE(o7);

    // Move from empty into engaged optional.
    o8 = std::move(o5);
    EXPECT_FALSE(o8);
}

TEST(OptionalTest, Triviality) {
    EXPECT_TRUE(std::is_trivially_copy_constructible<beman::optional::optional<int>>::value);
    EXPECT_TRUE(std::is_trivially_copy_assignable<beman::optional::optional<int>>::value);
    EXPECT_TRUE(std::is_trivially_move_constructible<beman::optional::optional<int>>::value);
    EXPECT_TRUE(std::is_trivially_move_assignable<beman::optional::optional<int>>::value);
    EXPECT_TRUE(std::is_trivially_destructible<beman::optional::optional<int>>::value);

    {
        struct T {
            T(const T&)            = default;
            T(T&&)                 = default;
            T& operator=(const T&) = default;
            T& operator=(T&&)      = default;
            ~T()                   = default;
        };
        EXPECT_TRUE(std::is_trivially_copy_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_trivially_copy_assignable<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_trivially_move_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_trivially_move_assignable<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_trivially_destructible<beman::optional::optional<T>>::value);
    }

    {
        struct T {
            T(const T&) {}
            T(T&&) {};
            T& operator=(const T&) { return *this; }
            T& operator=(T&&) { return *this; };
            ~T() {}
        };
        EXPECT_TRUE(!std::is_trivially_copy_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(!std::is_trivially_copy_assignable<beman::optional::optional<T>>::value);
        EXPECT_TRUE(!std::is_trivially_move_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(!std::is_trivially_move_assignable<beman::optional::optional<T>>::value);
        EXPECT_TRUE(!std::is_trivially_destructible<beman::optional::optional<T>>::value);
    }
}

TEST(OptionalTest, Deletion) {
    EXPECT_TRUE(std::is_copy_constructible<beman::optional::optional<int>>::value);
    EXPECT_TRUE(std::is_copy_assignable<beman::optional::optional<int>>::value);
    EXPECT_TRUE(std::is_move_constructible<beman::optional::optional<int>>::value);
    EXPECT_TRUE(std::is_move_assignable<beman::optional::optional<int>>::value);
    EXPECT_TRUE(std::is_destructible<beman::optional::optional<int>>::value);

    {
        struct T {
            T(const T&)            = default;
            T(T&&)                 = default;
            T& operator=(const T&) = default;
            T& operator=(T&&)      = default;
            ~T()                   = default;
        };
        EXPECT_TRUE(std::is_copy_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_copy_assignable<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_move_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_move_assignable<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_destructible<beman::optional::optional<T>>::value);
    }

    {
        struct T {
            T(const T&)            = delete;
            T(T&&)                 = delete;
            T& operator=(const T&) = delete;
            T& operator=(T&&)      = delete;
        };
        EXPECT_TRUE(!std::is_copy_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(!std::is_copy_assignable<beman::optional::optional<T>>::value);
        EXPECT_TRUE(!std::is_move_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(!std::is_move_assignable<beman::optional::optional<T>>::value);
    }

    {
        struct T {
            T(const T&)            = delete;
            T(T&&)                 = default;
            T& operator=(const T&) = delete;
            T& operator=(T&&)      = default;
        };
        EXPECT_TRUE(!std::is_copy_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(!std::is_copy_assignable<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_move_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_move_assignable<beman::optional::optional<T>>::value);
    }

    {
        struct T {
            T(const T&)            = default;
            T(T&&)                 = delete;
            T& operator=(const T&) = default;
            T& operator=(T&&)      = delete;
        };
        EXPECT_TRUE(std::is_copy_constructible<beman::optional::optional<T>>::value);
        EXPECT_TRUE(std::is_copy_assignable<beman::optional::optional<T>>::value);
    }
}

struct takes_init_and_variadic {
    std::vector<int>     v;
    std::tuple<int, int> t;
    template <class... Args>
    takes_init_and_variadic(std::initializer_list<int> l, Args&&... args) : v(l), t(std::forward<Args>(args)...) {}
};

TEST(OptionalTest, InPlace) {
    beman::optional::optional<int> o1{beman::optional::in_place};
    beman::optional::optional<int> o2(beman::optional::in_place);
    EXPECT_TRUE(o1);
    EXPECT_TRUE(o1 == 0);
    EXPECT_TRUE(o2);
    EXPECT_TRUE(o2 == 0);

    beman::optional::optional<int> o3(beman::optional::in_place, 42);
    EXPECT_TRUE(o3 == 42);

    beman::optional::optional<std::tuple<int, int>> o4(beman::optional::in_place, 0, 1);
    EXPECT_TRUE(o4);
    EXPECT_TRUE(std::get<0>(*o4) == 0);
    EXPECT_TRUE(std::get<1>(*o4) == 1);

    beman::optional::optional<std::vector<int>> o5(beman::optional::in_place, {0, 1});
    EXPECT_TRUE(o5);
    EXPECT_TRUE((*o5)[0] == 0);
    EXPECT_TRUE((*o5)[1] == 1);

    beman::optional::optional<takes_init_and_variadic> o6(beman::optional::in_place, {0, 1}, 2, 3);
    EXPECT_TRUE(o6->v[0] == 0);
    EXPECT_TRUE(o6->v[1] == 1);
    EXPECT_TRUE(std::get<0>(o6->t) == 2);
    EXPECT_TRUE(std::get<1>(o6->t) == 3);
}

TEST(OptionalTest, MakeOptional) {
    auto o1 = beman::optional::make_optional(42);
    auto o2 = beman::optional::optional<int>(42);

    constexpr bool is_same = std::is_same<decltype(o1), beman::optional::optional<int>>::value;
    EXPECT_TRUE(is_same);
    EXPECT_TRUE(o1 == o2);

    auto o3 = beman::optional::make_optional<std::tuple<int, int, int, int>>(0, 1, 2, 3);
    EXPECT_TRUE(std::get<0>(*o3) == 0);
    EXPECT_TRUE(std::get<1>(*o3) == 1);
    EXPECT_TRUE(std::get<2>(*o3) == 2);
    EXPECT_TRUE(std::get<3>(*o3) == 3);

    auto o4 = beman::optional::make_optional<std::vector<int>>({0, 1, 2, 3});
    EXPECT_TRUE(o4.value()[0] == 0);
    EXPECT_TRUE(o4.value()[1] == 1);
    EXPECT_TRUE(o4.value()[2] == 2);
    EXPECT_TRUE(o4.value()[3] == 3);

    auto o5 = beman::optional::make_optional<takes_init_and_variadic>({0, 1}, 2, 3);
    EXPECT_TRUE(o5->v[0] == 0);
    EXPECT_TRUE(o5->v[1] == 1);
    EXPECT_TRUE(std::get<0>(o5->t) == 2);
    EXPECT_TRUE(std::get<1>(o5->t) == 3);
}

TEST(OptionalTest, Nullopt) {
    beman::optional::optional<int> o1 = beman::optional::nullopt;
    beman::optional::optional<int> o2{beman::optional::nullopt};
    beman::optional::optional<int> o3(beman::optional::nullopt);
    beman::optional::optional<int> o4 = {beman::optional::nullopt};

    EXPECT_TRUE(!o1);
    EXPECT_TRUE(!o2);
    EXPECT_TRUE(!o3);
    EXPECT_TRUE(!o4);

    EXPECT_TRUE(!std::is_default_constructible<beman::optional::nullopt_t>::value);
}

struct move_detector {
    move_detector() = default;
    move_detector(move_detector&& rhs) { rhs.been_moved = true; }
    bool been_moved = false;
};

TEST(OptionalTest, Observers) {
    beman::optional::optional<int>       o1 = 42;
    beman::optional::optional<int>       o2;
    const beman::optional::optional<int> o3 = 42;

    EXPECT_TRUE(*o1 == 42);
    EXPECT_TRUE(*o1 == o1.value());
    EXPECT_TRUE(o2.value_or(42) == 42);
    EXPECT_TRUE(o3.value() == 42);
    auto success = std::is_same<decltype(o1.value()), int&>::value;
    EXPECT_TRUE(success);
    success = std::is_same<decltype(o3.value()), const int&>::value;
    EXPECT_TRUE(success);
    success = std::is_same<decltype(std::move(o1).value()), int&&>::value;
    EXPECT_TRUE(success);

    beman::optional::optional<move_detector> o4{beman::optional::in_place};
    move_detector                            o5 = std::move(o4).value();
    EXPECT_TRUE(o4->been_moved);
    EXPECT_TRUE(!o5.been_moved);
}

TEST(OptionalTest, RelationalOps) {
    beman::optional::optional<int> o1{4};
    beman::optional::optional<int> o2{42};
    beman::optional::optional<int> o3{};

    //  SECTION("self simple")
    {
        EXPECT_TRUE(!(o1 == o2));
        EXPECT_TRUE(o1 == o1);
        EXPECT_TRUE(o1 != o2);
        EXPECT_TRUE(!(o1 != o1));
        EXPECT_TRUE(o1 < o2);
        EXPECT_TRUE(!(o1 < o1));
        EXPECT_TRUE(!(o1 > o2));
        EXPECT_TRUE(!(o1 > o1));
        EXPECT_TRUE(o1 <= o2);
        EXPECT_TRUE(o1 <= o1);
        EXPECT_TRUE(!(o1 >= o2));
        EXPECT_TRUE(o1 >= o1);
    }
    //  SECTION("nullopt simple")
    {
        EXPECT_TRUE(!(o1 == beman::optional::nullopt));
        EXPECT_TRUE(!(beman::optional::nullopt == o1));
        EXPECT_TRUE(o1 != beman::optional::nullopt);
        EXPECT_TRUE(beman::optional::nullopt != o1);
        EXPECT_TRUE(!(o1 < beman::optional::nullopt));
        EXPECT_TRUE(beman::optional::nullopt < o1);
        EXPECT_TRUE(o1 > beman::optional::nullopt);
        EXPECT_TRUE(!(beman::optional::nullopt > o1));
        EXPECT_TRUE(!(o1 <= beman::optional::nullopt));
        EXPECT_TRUE(beman::optional::nullopt <= o1);
        EXPECT_TRUE(o1 >= beman::optional::nullopt);
        EXPECT_TRUE(!(beman::optional::nullopt >= o1));

        EXPECT_TRUE(o3 == beman::optional::nullopt);
        EXPECT_TRUE(beman::optional::nullopt == o3);
        EXPECT_TRUE(!(o3 != beman::optional::nullopt));
        EXPECT_TRUE(!(beman::optional::nullopt != o3));
        EXPECT_TRUE(!(o3 < beman::optional::nullopt));
        EXPECT_TRUE(!(beman::optional::nullopt < o3));
        EXPECT_TRUE(!(o3 > beman::optional::nullopt));
        EXPECT_TRUE(!(beman::optional::nullopt > o3));
        EXPECT_TRUE(o3 <= beman::optional::nullopt);
        EXPECT_TRUE(beman::optional::nullopt <= o3);
        EXPECT_TRUE(o3 >= beman::optional::nullopt);
        EXPECT_TRUE(beman::optional::nullopt >= o3);
    }
    //  SECTION("with T simple")
    {
        EXPECT_TRUE(!(o1 == 1));
        EXPECT_TRUE(!(1 == o1));
        EXPECT_TRUE(o1 != 1);
        EXPECT_TRUE(1 != o1);
        EXPECT_TRUE(!(o1 < 1));
        EXPECT_TRUE(1 < o1);
        EXPECT_TRUE(o1 > 1);
        EXPECT_TRUE(!(1 > o1));
        EXPECT_TRUE(!(o1 <= 1));
        EXPECT_TRUE(1 <= o1);
        EXPECT_TRUE(o1 >= 1);
        EXPECT_TRUE(!(1 >= o1));

        EXPECT_TRUE(o1 == 4);
        EXPECT_TRUE(4 == o1);
        EXPECT_TRUE(!(o1 != 4));
        EXPECT_TRUE(!(4 != o1));
        EXPECT_TRUE(!(o1 < 4));
        EXPECT_TRUE(!(4 < o1));
        EXPECT_TRUE(!(o1 > 4));
        EXPECT_TRUE(!(4 > o1));
        EXPECT_TRUE(o1 <= 4);
        EXPECT_TRUE(4 <= o1);
        EXPECT_TRUE(o1 >= 4);
        EXPECT_TRUE(4 >= o1);
    }
    beman::optional::optional<std::string> o4{"hello"};
    beman::optional::optional<std::string> o5{"xyz"};

    //  SECTION("self complex")
    {
        EXPECT_TRUE(!(o4 == o5));
        EXPECT_TRUE(o4 == o4);
        EXPECT_TRUE(o4 != o5);
        EXPECT_TRUE(!(o4 != o4));
        EXPECT_TRUE(o4 < o5);
        EXPECT_TRUE(!(o4 < o4));
        EXPECT_TRUE(!(o4 > o5));
        EXPECT_TRUE(!(o4 > o4));
        EXPECT_TRUE(o4 <= o5);
        EXPECT_TRUE(o4 <= o4);
        EXPECT_TRUE(!(o4 >= o5));
        EXPECT_TRUE(o4 >= o4);
    }
    //  SECTION("nullopt complex")
    {
        EXPECT_TRUE(!(o4 == beman::optional::nullopt));
        EXPECT_TRUE(!(beman::optional::nullopt == o4));
        EXPECT_TRUE(o4 != beman::optional::nullopt);
        EXPECT_TRUE(beman::optional::nullopt != o4);
        EXPECT_TRUE(!(o4 < beman::optional::nullopt));
        EXPECT_TRUE(beman::optional::nullopt < o4);
        EXPECT_TRUE(o4 > beman::optional::nullopt);
        EXPECT_TRUE(!(beman::optional::nullopt > o4));
        EXPECT_TRUE(!(o4 <= beman::optional::nullopt));
        EXPECT_TRUE(beman::optional::nullopt <= o4);
        EXPECT_TRUE(o4 >= beman::optional::nullopt);
        EXPECT_TRUE(!(beman::optional::nullopt >= o4));

        EXPECT_TRUE(o3 == beman::optional::nullopt);
        EXPECT_TRUE(beman::optional::nullopt == o3);
        EXPECT_TRUE(!(o3 != beman::optional::nullopt));
        EXPECT_TRUE(!(beman::optional::nullopt != o3));
        EXPECT_TRUE(!(o3 < beman::optional::nullopt));
        EXPECT_TRUE(!(beman::optional::nullopt < o3));
        EXPECT_TRUE(!(o3 > beman::optional::nullopt));
        EXPECT_TRUE(!(beman::optional::nullopt > o3));
        EXPECT_TRUE(o3 <= beman::optional::nullopt);
        EXPECT_TRUE(beman::optional::nullopt <= o3);
        EXPECT_TRUE(o3 >= beman::optional::nullopt);
        EXPECT_TRUE(beman::optional::nullopt >= o3);
    }

    //  SECTION("with T complex")
    {
        EXPECT_TRUE(!(o4 == "a"));
        EXPECT_TRUE(!("a" == o4));
        EXPECT_TRUE(o4 != "a");
        EXPECT_TRUE("a" != o4);
        EXPECT_TRUE(!(o4 < "a"));
        EXPECT_TRUE("a" < o4);
        EXPECT_TRUE(o4 > "a");
        EXPECT_TRUE(!("a" > o4));
        EXPECT_TRUE(!(o4 <= "a"));
        EXPECT_TRUE("a" <= o4);
        EXPECT_TRUE(o4 >= "a");
        EXPECT_TRUE(!("a" >= o4));

        EXPECT_TRUE(o4 == "hello");
        EXPECT_TRUE("hello" == o4);
        EXPECT_TRUE(!(o4 != "hello"));
        EXPECT_TRUE(!("hello" != o4));
        EXPECT_TRUE(!(o4 < "hello"));
        EXPECT_TRUE(!("hello" < o4));
        EXPECT_TRUE(!(o4 > "hello"));
        EXPECT_TRUE(!("hello" > o4));
        EXPECT_TRUE(o4 <= "hello");
        EXPECT_TRUE("hello" <= o4);
        EXPECT_TRUE(o4 >= "hello");
        EXPECT_TRUE("hello" >= o4);
    }
}

TEST(OptionalTest, SwapValue) {
    beman::optional::optional<int> o1 = 42;
    beman::optional::optional<int> o2 = 12;
    o1.swap(o2);
    EXPECT_EQ(o1.value(), 12);
    EXPECT_EQ(o2.value(), 42);
}

TEST(OptionalTest, SwapWNull) {
    beman::optional::optional<int> o1 = 42;
    beman::optional::optional<int> o2 = beman::optional::nullopt;
    o1.swap(o2);
    EXPECT_TRUE(!o1.has_value());
    EXPECT_EQ(o2.value(), 42);
}

TEST(OptionalTest, SwapNullIntializedWithValue) {
    beman::optional::optional<int> o1 = beman::optional::nullopt;
    beman::optional::optional<int> o2 = 42;
    o1.swap(o2);
    EXPECT_EQ(o1.value(), 42);
    EXPECT_TRUE(!o2.has_value());
}

TEST(OptionalTest, Emplace) {
    beman::optional::optional<std::pair<std::pair<int, int>, std::pair<double, double>>> i;
    i.emplace(std::piecewise_construct, std::make_tuple(0, 2), std::make_tuple(3, 4));
    EXPECT_TRUE(i->first.first == 0);
    EXPECT_TRUE(i->first.second == 2);
    EXPECT_TRUE(i->second.first == 3);
    EXPECT_TRUE(i->second.second == 4);
}

struct A {
    A() { throw std::exception(); }
};

TEST(OptionalTest, EmplaceWithExceptionThrown) {
    beman::optional::optional<A> a;
    EXPECT_ANY_THROW(a.emplace());
}

TEST(OptionalTest, RangeTest) {
    beman::optional::optional<int> o1 = beman::optional::nullopt;
    beman::optional::optional<int> o2 = 42;
    EXPECT_EQ(*o2, 42);
    for (auto k : o1) {
        std::ignore = k;
        EXPECT_TRUE(false);
    }
    for (auto k : o2) {
        EXPECT_EQ(k, 42);
    }
}

TEST(OptionalTest, RangeConstructors) {
    std::ranges::single_view<std::optional<int>> s;
    std::ranges::single_view<std::optional<int>> s2{s};
    std::ranges::single_view<std::optional<int>> s3{std::optional<int>{}};
    std::ignore = s2;
    std::ignore = s3;

    beman::optional::optional<std::optional<int>> n;
    beman::optional::optional<std::optional<int>> n2{n};
    beman::optional::optional<std::optional<int>> n3{std::optional<int>{}};
    std::ignore = n2;
    std::ignore = n3;
}

TEST(OptionalTest, ConceptCheckRef) {
    static_assert(std::ranges::range<beman::optional::optional<int&>>);
    static_assert(std::ranges::view<beman::optional::optional<int&>>);
    static_assert(std::ranges::input_range<beman::optional::optional<int&>>);
    static_assert(std::ranges::forward_range<beman::optional::optional<int&>>);
    static_assert(std::ranges::bidirectional_range<beman::optional::optional<int&>>);
    static_assert(std::ranges::contiguous_range<beman::optional::optional<int&>>);
    static_assert(std::ranges::common_range<beman::optional::optional<int&>>);
    static_assert(std::ranges::viewable_range<beman::optional::optional<int&>>);
    static_assert(std::ranges::borrowed_range<beman::optional::optional<int&>>);
    static_assert(std::ranges::random_access_range<beman::optional::optional<int&>>);

    static_assert(std::ranges::range<beman::optional::optional<int*&>>);
    static_assert(std::ranges::view<beman::optional::optional<int*&>>);
    static_assert(std::ranges::input_range<beman::optional::optional<int*&>>);
    static_assert(std::ranges::forward_range<beman::optional::optional<int*&>>);
    static_assert(std::ranges::bidirectional_range<beman::optional::optional<int*&>>);
    static_assert(std::ranges::contiguous_range<beman::optional::optional<int*&>>);
    static_assert(std::ranges::common_range<beman::optional::optional<int*&>>);
    static_assert(std::ranges::viewable_range<beman::optional::optional<int*&>>);
    static_assert(std::ranges::borrowed_range<beman::optional::optional<int*&>>);
    static_assert(std::ranges::random_access_range<beman::optional::optional<int*&>>);

    using ref = std::reference_wrapper<int>&;
    static_assert(std::ranges::range<beman::optional::optional<ref>>);
    static_assert(std::ranges::view<beman::optional::optional<ref>>);
    static_assert(std::ranges::input_range<beman::optional::optional<ref>>);
    static_assert(std::ranges::forward_range<beman::optional::optional<ref>>);
    static_assert(std::ranges::bidirectional_range<beman::optional::optional<ref>>);
    static_assert(std::ranges::contiguous_range<beman::optional::optional<ref>>);
    static_assert(std::ranges::common_range<beman::optional::optional<ref>>);
    static_assert(std::ranges::viewable_range<beman::optional::optional<ref>>);
    static_assert(std::ranges::borrowed_range<beman::optional::optional<ref>>);
    static_assert(std::ranges::random_access_range<beman::optional::optional<ref>>);
}

TEST(OptionalTest, BreathingTest) {
    beman::optional::optional<int> m;
    beman::optional::optional<int> m1{1};

    m = m1;
    ASSERT_EQ(*std::begin(m), 1);

    m = {};
    ASSERT_FALSE(m);

    beman::optional::optional<double> d0{0};

    beman::optional::optional<double> d1{1};

    d0 = d1;
    ASSERT_EQ(*std::begin(d0), 1.0);
}

TEST(OptionalTest, BreathingTestRef) {
    beman::optional::optional<int&> m;

    int                             one = 1;
    beman::optional::optional<int&> m1{one};

    m = m1;
    ASSERT_EQ(*std::begin(m), 1);

    double                             zero = 0.0;
    beman::optional::optional<double&> d0{zero};

    double                             one_d = 1.0;
    beman::optional::optional<double&> d1{one_d};

    d0 = d1;
    ASSERT_EQ(*std::begin(d0), 1.0);
}

TEST(OptionalTest, CompTest) {
    beman::optional::optional<int> m;
    beman::optional::optional<int> m0{0};
    beman::optional::optional<int> m1{1};
    beman::optional::optional<int> m1a{1};

    ASSERT_EQ(m, m);
    ASSERT_EQ(m0, m0);
    ASSERT_EQ(m1, m1);
    ASSERT_EQ(m1a, m1a);
    ASSERT_EQ(m1, m1a);

    ASSERT_NE(m, m0);
    ASSERT_NE(m0, m1);

    ASSERT_TRUE(m < m0);
    ASSERT_TRUE(m0 < m1);
    ASSERT_TRUE(m1 <= m1a);
}

TEST(OptionalTest, CompTestRef) {
    beman::optional::optional<int&> m;
    int                             zero  = 0;
    int                             one   = 1;
    int                             one_a = 1;
    beman::optional::optional<int&> m0{zero};
    beman::optional::optional<int&> m1{one};
    beman::optional::optional<int&> m1a{one_a};

    ASSERT_EQ(m, m);
    ASSERT_EQ(m0, m0);
    ASSERT_EQ(m1, m1);
    ASSERT_EQ(m1a, m1a);
    ASSERT_EQ(m1, m1a);

    ASSERT_NE(m, m0);
    ASSERT_NE(m0, m1);

    ASSERT_TRUE(m < m0);
    ASSERT_TRUE(m0 > m);
    ASSERT_TRUE(m0 < m1);
    ASSERT_TRUE(m1a <= m1);
}

// "and_then" creates a new view by applying a
// transformation to each element in an input
// range, and flattening the resulting range of
// ranges. A.k.a. bind
// (This uses one syntax for constrained lambdas
// in C++20.)
inline constexpr auto and_then = [](auto&& r, auto fun) {
    return decltype(r)(r) | std::views::transform(std::move(fun)) | std::views::join;
};

// "yield_if" takes a bool and a value and
// returns a view of zero or one elements.
inline constexpr auto yield_if = []<class T>(bool b, T x) {
    return b ? beman::optional::optional<T>{std::move(x)} : beman::optional::nullopt;
};

TEST(OptionalTest, PythTripleTest) {
    using std::views::iota;
    auto triples = and_then(iota(1), [](int z) {
        return and_then(iota(1, z + 1), [=](int x) {
            return and_then(iota(x, z + 1),
                            [=](int y) { return yield_if(x * x + y * y == z * z, std::make_tuple(x, y, z)); });
        });
    });

    ASSERT_EQ(*std::ranges::begin(triples), std::make_tuple(3, 4, 5));
}

using namespace beman;

TEST(OptionalTest, ValueBase) {
    int                            i = 7;
    beman::optional::optional<int> v1{};

    beman::optional::optional<int> v2{i};
    for (auto i : v1)
        ASSERT_TRUE(i != i); // tautology so i is used and not warned

    for (auto i : v2)
        ASSERT_EQ(i, 7);
}

TEST(OptionalTest, RefWrapper) {
    int i = 7;

    beman::optional::optional<int> v2{std::ref(i)};

    for (auto i : v2)
        ASSERT_EQ(i, 7);
}

TEST(OptionalTest, ValueNonDefaultConstruct) {
    using beman::optional::tests::int_ctor;
    int_ctor                            i = 7;
    beman::optional::optional<int_ctor> v1{};
    beman::optional::optional<int_ctor> v2{i};
    std::ignore = v1;
    std::ignore = v2;
}

TEST(OptionalTest, RefBase) {
    int                             i = 7;
    beman::optional::optional<int&> v1{};
    // ASSERT_TRUE(v1.size() == 0);

    beman::optional::optional<int&> v2{i};
    // ASSERT_TRUE(v2.size() == 1);
    for (auto i : v1)
        ASSERT_TRUE(i != i); // tautology so i is used and not warned

    for (auto i : v2) {
        ASSERT_EQ(i, 7);
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(i, 7);

    for (auto&& i : v2) {
        ASSERT_EQ(i, 7);
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(i, 9);

    int s = 4;

    for (auto&& i : beman::optional::optional<int&>(s)) {
        ASSERT_EQ(i, 4);
        i = 9;
        ASSERT_EQ(i, 9);
    }
    ASSERT_EQ(s, 9);
}

TEST(OptionalTest, HashTest) {
    beman::optional::optional<int> o1 = beman::optional::nullopt;
    beman::optional::optional<int> o2 = beman::optional::nullopt;
    beman::optional::optional<int> o3 = 42;
    beman::optional::optional<int> o4 = 42;

    auto h1 = std::hash<beman::optional::optional<int>>{}(o1);
    auto h2 = std::hash<beman::optional::optional<int>>{}(o2);
    auto h3 = std::hash<beman::optional::optional<int>>{}(o3);
    auto h4 = std::hash<beman::optional::optional<int>>{}(o4);

    EXPECT_EQ(h1, h2);
    EXPECT_EQ(h3, h4);
    EXPECT_NE(h1, h3);

    for (int i : std::views::iota(0, 1000)) {
        auto h1 = std::hash<beman::optional::optional<int>>{}(i);
        auto h2 = std::hash<int>{}(i);
        EXPECT_EQ(h1, h2);
    }
}

TEST(OptionalTest, CanHoldValueOfImmovableType) {
    using beman::optional::tests::immovable;

    beman::optional::optional<immovable> o1(beman::optional::in_place);
    EXPECT_TRUE(o1);

    // ...and can reset it with `nullopt`.
    static_assert(noexcept(o1 = beman::optional::nullopt));
    o1 = beman::optional::nullopt;
    EXPECT_FALSE(o1);

    // Also, can construct with `nullopt`.
    beman::optional::optional<immovable> o2 = beman::optional::nullopt;
    EXPECT_FALSE(o2);
}

// Moving an `optional<T&>` should not move the remote value.
TEST(OptionalTest, OptionalFromOptionalRef) {
    using beman::optional::tests::copyable_from_non_const_lvalue_only;

    copyable_from_non_const_lvalue_only cm;

    beman::optional::optional<copyable_from_non_const_lvalue_only&> o1 = cm;
    ASSERT_TRUE(o1);

    {
        beman::optional::optional<copyable_from_non_const_lvalue_only> o2 = o1;
        ASSERT_TRUE(o2);
    }

    beman::optional::optional<copyable_from_non_const_lvalue_only> o2 = std::move(o1);
    ASSERT_TRUE(o2);

    o2 = o1;
    ASSERT_TRUE(o2);

    o2 = std::move(o1);
    ASSERT_TRUE(o2);

    o2.reset();
    o2 = o1;
    ASSERT_TRUE(o2);

    o2.reset();
    o2 = std::move(o1);
    ASSERT_TRUE(o2);
}

TEST(OptionalTest, OptionalFromOptionalRefExplicit) {
    using beman::optional::tests::copyable_from_non_const_lvalue_only;
    using beman::optional::tests::explicitly_convertible_from_non_const_lvalue_only;

    explicitly_convertible_from_non_const_lvalue_only ec;

    beman::optional::optional<explicitly_convertible_from_non_const_lvalue_only&> o3 = ec;

    beman::optional::optional<copyable_from_non_const_lvalue_only> o4(o3);
    ASSERT_TRUE(o4);
    beman::optional::optional<copyable_from_non_const_lvalue_only> o5(std::move(o3));
    ASSERT_TRUE(o5);
}

TEST(OptionalTest, OptionalFromOptionalConstRef) {
    using beman::optional::tests::copyable_from_const_lvalue_only;

    copyable_from_const_lvalue_only cm;

    beman::optional::optional<const copyable_from_const_lvalue_only&> o1 = cm;
    ASSERT_TRUE(o1);

    {
        beman::optional::optional<copyable_from_const_lvalue_only> o2 = o1;
        ASSERT_TRUE(o2);
    }

    beman::optional::optional<copyable_from_const_lvalue_only> o2 = std::move(o1);
    ASSERT_TRUE(o2);

    o2 = o1;
    ASSERT_TRUE(o2);

    o2 = std::move(o1);
    ASSERT_TRUE(o2);

    o2.reset();
    o2 = o1;
    ASSERT_TRUE(o2);

    o2.reset();
    o2 = std::move(o1);
    ASSERT_TRUE(o2);
}

TEST(OptionalTest, OptionalFromOptionalConstRefExplicit) {
    using beman::optional::tests::copyable_from_const_lvalue_only;
    using beman::optional::tests::explicitly_convertible_from_const_lvalue_only;

    explicitly_convertible_from_const_lvalue_only ec;

    beman::optional::optional<const explicitly_convertible_from_const_lvalue_only&> o3 = ec;

    beman::optional::optional<copyable_from_const_lvalue_only> o4(o3);
    ASSERT_TRUE(o4);
    beman::optional::optional<copyable_from_const_lvalue_only> o5(std::move(o3));
    ASSERT_TRUE(o5);
}

TEST(OptionalTest, OptionalOfAnyWorks) {
    beman::optional::optional<std::any> o1 = 42;

    beman::optional::optional<std::any> o2 = o1;
    EXPECT_TRUE(std::any_cast<const int&>(*o2) == 42);

    beman::optional::optional<std::any> o3 = std::move(o1);
    EXPECT_TRUE(std::any_cast<const int&>(*o3) == 42);
}
