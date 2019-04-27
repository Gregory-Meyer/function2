// Copyright (c) 2019 Gregory Meyer
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice (including
// the next paragraph) shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <fn2/fn2.h>

#include <functional>
#include <numeric>
#include <utility>
#include <vector>

#include <catch2/catch.hpp>

namespace {

constexpr int times2(int x) noexcept {
    return x * 2;
}

constexpr int div2(int x) noexcept {
    return x / 2;
}

} // namespace

TEST_CASE("Function()", "[fn2::Function]") {
    const fn2::Function<int(int)> f;

    REQUIRE_FALSE(f);
}

struct Doubler {
    constexpr int operator()(int x) const noexcept {
        return x * 2;
    }
};

TEST_CASE("Function(F&&)", "[fn2::Function]") {
    SECTION("regular function") {
        const fn2::Function<int(int)> f = times2;

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("lambda expression") {
        const fn2::Function<int(int)> f = [](int x) { return x * 2; };

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("complex lambda expression") {
        const fn2::Function<int(int)> f = [multipliers = std::vector<int>{2}](int x) {
            return std::accumulate(
                multipliers.cbegin(), multipliers.cend(), x, std::multiplies<int>()
            );
        };

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("function-like object") {
        const fn2::Function<int(int)> f = Doubler();

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("pointer to member function") {
        using Vector = std::vector<int>;

        const fn2::Function<Vector::size_type(const Vector&)> f = &Vector::size;
        const Vector v = {0, 1, 2, 3};

        REQUIRE(f);
        REQUIRE(f(v) == 4);
    }

    SECTION("pointer to member data") {
        using Pair = std::pair<int, int>;

        const fn2::Function<const int&(const Pair&)> f = &Pair::first;
        const Pair p = {0, 1};

        REQUIRE(f);
        REQUIRE(f(p) == 0);
    }
}

TEST_CASE("Function(const Function&)", "[fn2::Function]") {
    const fn2::Function<int(int)> f = times2;

    {
        const fn2::Function<int(int)> g = f;

        REQUIRE(f);
        REQUIRE(g);
        REQUIRE(f(5) == 10);
        REQUIRE(g(5) == 10);
    }

    REQUIRE(f);
    REQUIRE(f(5) == 10);
}

TEST_CASE("Function(Function&&)", "[fn2::Function]") {
    fn2::Function<int(int)> f = times2;
    const fn2::Function<int(int)> g = std::move(f);

    REQUIRE(g);
    REQUIRE(g(5) == 10);

    if (f) {
        REQUIRE(f(5) == 10);
    }
}

class Multiplier {
public:
    Multiplier() = default;

    explicit Multiplier(int multiplier) : multipliers_({multiplier}) { }

    Multiplier(int first, int second) : multipliers_({first, second}) { }

    explicit Multiplier(std::initializer_list<int> list) : multipliers_(list) { }

    int operator()(int x) const noexcept {
        return std::accumulate(
            multipliers_.cbegin(), multipliers_.cend(),
            x, std::multiplies<int>()
        );
    }

private:
    std::vector<int> multipliers_ = {2};
};

TEST_CASE("Function(std::in_place_type_t<F>, Us&&...)", "[fn2::Function]") {
    SECTION("default constructor") {
        const fn2::Function<int(int)> f(std::in_place_type<Multiplier>);

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("single argument constructor") {
        const fn2::Function<int(int)> f(std::in_place_type<Multiplier>, -2);

        REQUIRE(f);
        REQUIRE(f(5) == -10);
    }

    SECTION("two argument constructor") {
        const fn2::Function<int(int)> f(std::in_place_type<Multiplier>, 2, 4);

        REQUIRE(f);
        REQUIRE(f(5) == 40);
    }

    SECTION("initializer list constructor") {
        const fn2::Function<int(int)> f(std::in_place_type<Multiplier>, {2, 4, 6});

        REQUIRE(f);
        REQUIRE(f(5) == 240);
    }
}

TEST_CASE("operator=(const Function&)", "[fn2::Function]") {
    const fn2::Function<int(int)> f = times2;
    fn2::Function<int(int)> g;

    g = f;

    REQUIRE(f);
    REQUIRE(g);
    REQUIRE(f(5) == 10);
    REQUIRE(g(5) == 10);
}

TEST_CASE("operator=(Function&&)", "[fn2::Function]") {
    SECTION("non-empty operands") {
        fn2::Function<int(int)> f = times2;
        fn2::Function<int(int)> g = div2;

        g = std::move(f);

        REQUIRE(f);
        REQUIRE(g);
        REQUIRE(f(5) == 2);
        REQUIRE(g(5) == 10);
    }

    SECTION("empty operand") {
        fn2::Function<int(int)> f = times2;

        f = fn2::Function<int(int)>();

        REQUIRE_FALSE(f);
    }
}

TEST_CASE("operator=(F&&)", "[fn2::Function]") {
    SECTION("regular function") {
        fn2::Function<int(int)> f;
        f = times2;

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("lambda expression") {
        fn2::Function<int(int)> f;
        f = [](int x) { return x * 2; };

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("complex lambda expression") {
        fn2::Function<int(int)> f;
        f = [multipliers = std::vector<int>{2}](int x) {
            return std::accumulate(
                multipliers.cbegin(), multipliers.cend(), x, std::multiplies<int>()
            );
        };

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("function-like object") {
        fn2::Function<int(int)> f;
        f = Doubler();

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("pointer to member function") {
        using Vector = std::vector<int>;

        fn2::Function<Vector::size_type(const Vector&)> f;
        f = &Vector::size;
        const Vector v = {0, 1, 2, 3};

        REQUIRE(f);
        REQUIRE(f(v) == 4);
    }

    SECTION("pointer to member data") {
        using Pair = std::pair<int, int>;

        fn2::Function<const int&(const Pair&)> f;
        f = &Pair::first;
        const Pair p = {0, 1};

        REQUIRE(f);
        REQUIRE(f(p) == 0);
    }
}

TEST_CASE("emplace(Us&&...)", "[fn2::Function]") {
    fn2::Function<int(int)> f = times2;

    SECTION("default constructor") {
        f.emplace<Multiplier>();

        REQUIRE(f);
        REQUIRE(f(5) == 10);
    }

    SECTION("single argument constructor") {
        f.emplace<Multiplier>(-2);

        REQUIRE(f);
        REQUIRE(f(5) == -10);
    }

    SECTION("two argument constructor") {
        f.emplace<Multiplier>(2, 4);

        REQUIRE(f);
        REQUIRE(f(5) == 40);
    }

    SECTION("initializer list constructor") {
        f.emplace<Multiplier>({2, 4, 6});

        REQUIRE(f);
        REQUIRE(f(5) == 240);
    }
}

TEST_CASE("reset()", "[fn2::Function]") {
    SECTION("regular function") {
        fn2::Function<int(int)> f = times2;
        f.reset();

        REQUIRE_FALSE(f);
    }

    SECTION("lambda expression") {
        fn2::Function<int(int)> f = [](int x) { return x * 2; };
        f.reset();

        REQUIRE_FALSE(f);
    }

    SECTION("complex lambda expression") {
        fn2::Function<int(int)> f = [multipliers = std::vector<int>{2}](int x) {
            return std::accumulate(
                multipliers.cbegin(), multipliers.cend(), x, std::multiplies<int>()
            );
        };
        f.reset();

        REQUIRE_FALSE(f);
    }

    SECTION("function-like object") {
        fn2::Function<int(int)> f = Doubler();
        f.reset();

        REQUIRE_FALSE(f);
    }

    SECTION("pointer to member function") {
        using Vector = std::vector<int>;

        fn2::Function<Vector::size_type(const Vector&)> f = &Vector::size;
        f.reset();

        REQUIRE_FALSE(f);
    }

    SECTION("pointer to member data") {
        using Pair = std::pair<int, int>;

        fn2::Function<const int&(const Pair&)> f = &Pair::first;
        f.reset();

        REQUIRE_FALSE(f);
    }
}

TEST_CASE("swap(Function&)", "[fn2::Function]") {
    fn2::Function<int(int)> f = times2;
    fn2::Function<int(int)> g = div2;

    f.swap(g);

    REQUIRE(f);
    REQUIRE(g);
    REQUIRE(f(5) == 2);
    REQUIRE(g(5) == 10);

    swap(f, g);

    REQUIRE(f);
    REQUIRE(g);
    REQUIRE(f(5) == 10);
    REQUIRE(g(5) == 2);
}
