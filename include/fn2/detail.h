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

#ifndef FN2_DETAIL_H
#define FN2_DETAIL_H

#include <functional>
#include <type_traits>
#include <utility>

namespace fn2::detail {

template <typename ...Ts>
    struct Overload : Ts... {
    using Ts::operator()...;
};

template <typename ...Ts>
Overload(Ts...) -> Overload<Ts...>;

template <typename R, typename ...As>
struct Vtable {
    R (*invoke)(void *self, As ...as);
    void (*destroy)(void *self) noexcept;
    void (*destroy_dealloc)(void *self) noexcept;
    void (*copy)(void *self, const void *other);
    void (*move)(void *self, void *other) noexcept;
    void (*swap)(void *self, void *other) noexcept;
    void* (*clone)(const void *self);
};

template <typename F, typename R, typename ...As>
static const Vtable<R, As...>& get_vtbl() noexcept {
    static_assert(
        std::is_invocable_r_v<R, F&, As...>,
        "F& must be invocable with arguments (As...) to return type R"
    );
    static_assert(std::is_nothrow_destructible_v<F>, "F must be nothrow destructible");
    static_assert(std::is_copy_constructible_v<F>, "F must be copy constructible");
    static_assert(std::is_nothrow_move_constructible_v<F>, "F must be nothrow move constructible");

    static const Vtable<R, As...> vtbl = {
        [](void *self, As ...as) -> R {
            return std::invoke(*static_cast<F*>(self), std::forward<As>(as)...);
        },
        [](void *self) noexcept {
            static_cast<F*>(self)->F::~F();
        },
        [](void *self) noexcept {
            delete static_cast<F*>(self);
        },
        [](void *self, const void *other) {
            new (self) F(*static_cast<const F*>(other));
        },
        [](void *self, void *other) noexcept {
            new (self) F(std::move(*static_cast<F*>(other)));
        },
        [](void *self, void *other) noexcept {
            F &lhs = *static_cast<F*>(self);
            F &rhs = *static_cast<F*>(other);

            if constexpr (std::is_nothrow_swappable_v<F>) {
                using std::swap;

                swap(lhs, rhs);
            } else {
                F temp(std::move(rhs));

                rhs.F::~F();
                new (other) F(std::move(lhs));

                lhs.F::~F();
                new (self) F(std::move(temp));
            }
        },
        [](const void *self) -> void* {
            return new F(*static_cast<const F*>(self));
        }
    };

    return vtbl;
}

} // namespace fn2::detail

#endif
