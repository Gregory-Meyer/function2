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

#include <catch2/catch.hpp>

namespace {

constexpr int add(int x, int y) noexcept {
    return x + y;
}

constexpr int subtract(int x, int y) noexcept {
    return x - y;
}

constexpr int multiply(int x, int y) noexcept {
    return x * y;
}

constexpr int divide(int x, int y) noexcept {
    return x / y;
}

constexpr int modulo(int x, int y) noexcept {
    return x % y;
}

} // namespace

TEST_CASE("basic functionality") {
    using Function = fn2::Function<int(int, int)>;

    Function f = add;

    REQUIRE(f(5, 5) == 10);

    Function g = f;
    f = subtract;

    REQUIRE(f(5, 5) == 0);
    REQUIRE(g(5, 5) == 10);

    g = f;
    f = multiply;

    REQUIRE(f(5, 5) == 25);
    REQUIRE(g(5, 5) == 0);

    g = f;
    f = divide;

    REQUIRE(f(5, 5) == 1);
    REQUIRE(g(5, 5) == 25);

    g = f;
    f = modulo;

    REQUIRE(f(5, 5) == 0);
    REQUIRE(g(5, 5) == 1);
}
