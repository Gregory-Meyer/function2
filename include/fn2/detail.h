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
#include <memory>
#include <type_traits>

namespace fn2::detail {

template <typename S>
class Invocable;

template <typename R, typename ...As>
class Invocable<R(As...)> {
public:
    virtual ~Invocable() = default;

    virtual R operator()(As ...as) = 0;

    virtual std::unique_ptr<Invocable<R(As...)>> clone() const = 0;
};

template <typename S, typename F>
class InvocableImpl;

template <typename R, typename ...As, typename F>
class InvocableImpl<R(As...), F> : public Invocable<R(As...)> {
public:
    static_assert(
        std::is_invocable_r_v<R, F&, As...>,
        "F& must be invocable with arguments (As...) to return type R"
    );

    static_assert(std::is_copy_constructible_v<F>, "F must be copy constructible");

    template <typename ...Ts>
    InvocableImpl(Ts &&...ts) noexcept(std::is_nothrow_constructible_v<F, Ts...>)
    : f_(std::forward<Ts>(ts)...) {
        static_assert(
            std::is_constructible_v<F, Ts...>,
            "F must be constructible from (Ts...)"
        );
    }

    virtual ~InvocableImpl() = default;

    R operator()(As ...as) override {
        return std::invoke(f_, std::forward<As>(as)...);
    }

    std::unique_ptr<Invocable<R(As...)>> clone() const override {
        return std::make_unique<InvocableImpl>(f_);
    }

private:
    F f_;
};

template <typename S, typename T, typename ...Ts>
std::unique_ptr<Invocable<S>> make_invocable(std::in_place_type_t<T>, Ts &&...ts) {
    static_assert(
        std::is_constructible_v<T, Ts...>,
        "T must be constructible from (Ts...)"
    );

    return std::make_unique<InvocableImpl<S, T>>(std::forward<Ts>(ts)...);
}

} // namespace fn2::detail

#endif
