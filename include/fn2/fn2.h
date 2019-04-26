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

#ifndef FN2_FN2_H
#define FN2_FN2_H

#include <fn2/detail.h>

#include <cassert>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

namespace fn2 {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <typename S>
class Function;
#endif

/**
 *  Function is an invocable object wrapper.
 *
 *  Function can wrap any invocable object, including regular function
 *  pointers, function-like objects (functors), lambda expressions,
 *  member function pointers, and member data pointers.
 *
 *  Function can have no wrapped object, in which case it is undefined
 *  behavior to attempt to invoke that Function. Users can query
 *  whether a Function has a wrapped object by using the
 *  Function::operator bool() member function, which returns false if
 *  there is no wrapped object.
 *
 *  The wrapped object is allocated on the free store and uses virtual
 *  functions to dispatch to the correct function implementation.
 */
template <typename R, typename ...As>
class Function<R(As...)> {
public:
    /** @returns a Function that does not wrap any object. */
    Function() noexcept = default;

    /**
     *  @tparam std::decay_t<F> must not be an object of type Function.
     *          Must be constructible from (F).
     *  @returns a Function that wraps an object of type
     *           std::decay_t<F>, direct initialized from
     *           (std::forward<F>(f)).
     *
     *  @throws std::bad_alloc
     *  @throws any exceptions that the constructor of std::decay_t<F>
     *          throws.
     */
    template <typename F, std::enable_if_t<!std::is_same_v<std::decay_t<F>, Function>, int> = 0>
    inline Function(F &&f);

    /**
     *  @tparam std::decay_t<F> must be default constructible.
     *  @returns a Function that wraps a default constructed object of
     *           type std::decay_t<F>.
     *
     *  @throws std::bad_alloc
     *  @throws aany exceptions that the default constructor of
     *          std::decay_t<F> throws.
     */
    template <typename F>
    inline explicit Function(std::in_place_type_t<F>);

    /**
     *  @tparam std::decay_t<F> Must be constructible from (U, Us...).
     *  @returns a Function that wraps an object of type
     *           std::decay_t<F>, direct initialized from
     *           (std::forward<U>(u), std::forward<Us>(us)...).
     *
     *  @throws std::bad_alloc
     *  @throws any exceptions that the constructor of std::decay_t<F>
     *          throws.
     */
    template <typename F, typename U, typename ...Us>
    inline Function(std::in_place_type_t<F>, U &&u, Us &&...us);

    /**
     *  @tparam std::decay_t<F> Must be constructible from
     *          (std::initializer_list<U>&, Us...).
     *  @returns a Function that wraps an object of type
     *           std::decay_t<F>, direct initialized from
     *           (list, std::forward<Us>(us)...).
     *
     *  @throws std::bad_alloc
     *  @throws any exceptions that the constructor of std::decay_t<F>
     *          throws.
     */
    template <typename F, typename U, typename ...Us>
    inline Function(std::in_place_type_t<F>, std::initializer_list<U> list, Us &&...us);

    /**
     *  @returns a Function that wraps an object copied from other's
     *           wrapped object.
     *
     *  @throws std::bad_alloc
     *  @throws any exceptions that the copy constructor of other's
     *          wrapped object throws.
     */
    inline Function(const Function &other);

    /**
     *  @param other will no longer wrap an object.
     *  @returns a Function that wraps the object that other wrapped.
     */
    Function(Function &&other) noexcept = default;

    /**
     *  @returns this Function, which now wraps an object copied from
     *           other's wrapped object.
     *
     *  @throws std::bad_alloc
     *  @throws any exceptions that the copy constructor of other's
     *          wrapped object throws.
     */
    inline Function& operator=(const Function &other);

    /**
     *  Destroys and deallocates this Function's wrapped object, if
     *  there is one, then takes ownership of the object wrapped by
     *  other.
     *
     *  @param other will no longer wrap an object.
     *  @returns this Function, which now that wraps the object that
     *           other wrapped.
     */
    Function& operator=(Function &&other) noexcept = default;

    /**
     *  If an exception is thrown, this Function will remain unchanged.
     *
     *  @tparam std::decay_t<F> must not be an object of type Function.
     *          Must be constructible from (F).
     *  @returns this Function, which now wraps an object of type
     *           std::decay_t<F> direct initialized from
     *           (std::forward<F>(f)).
     *
     *  @throws std::bad_alloc
     *  @throws any exceptions that the constructor of std::decay_t<F>
     *          throws.
     */
    template <typename F, std::enable_if_t<!std::is_same_v<std::decay_t<F>, Function>, int> = 0>
    inline Function& operator=(F &&f);

    /**
     *  Constructs a new wrapped object of type std::decay_t<F>,
     *  direct initialized from (std::forward<Us>(us)...).
     *
     *  @tparam std::decay_t<F> must be constructible from (Us...).
     *
     *  @throws std::bad_alloc
     *  @throws any exceptions that the constructor of std::decay_t<F>
     *          throws.
     */
    template <typename F, typename ...Us>
    inline void emplace(Us &&...us);

    /**
     *  Constructs a new wrapped object of type std::decay_t<F>,
     *  direct initialized from (list, std::forward<Us>(us)...).
     *
     *  @tparam std::decay_t<F> must be constructible from
     *          (std::initializer_list<U>&, Us...).
     *
     *  @throws std::bad_alloc
     *  @throws any exceptions that the constructor of std::decay_t<F>
     *          throws.
     */
    template <typename F, typename U, typename ...Us>
    inline void emplace(std::initializer_list<U> list, Us &&...us);

    /**
     *  Deallocates and destroys this Function's wrapped object, if
     *  there is one.
     */
    inline void reset() noexcept;

    /** Swaps ownership of wrapped objects with another Function. */
    inline void swap(Function &other) noexcept;

    /**
     *  @param this must wrap an object.
     *  @returns the result of invoking the wrapped object with
     *           parameters (std::forward<As>(as...)).
     *
     *  @throws any exceptions that the wrapped object throws on
     *          invocation.
     */
    inline R operator()(As ...as);

    /** @returns true if this Function currently wraps an object. */
    inline explicit operator bool() const noexcept;

private:
    std::unique_ptr<detail::Invocable<R(As...)>> clone() const;

    std::unique_ptr<detail::Invocable<R(As...)>> invocable_ptr_;
};

/** Swaps ownership of two Function's wrapped objects. */
template <typename R, typename ...As>
inline void swap(Function<R(As...)> &lhs, Function<R(As...)> &rhs) noexcept;

/**
 *  @tparam std::decay_t<F> must not be an object of type Function.
 *          Must be constructible from (F).
 *  @returns a Function that wraps an object of type
 *           std::decay_t<F>, direct initialized from
 *           (std::forward<F>(f)).
 *
 *  @throws std::bad_alloc
 *  @throws any exceptions that the constructor of std::decay_t<F>
 *          throws.
 */
template <typename R, typename ...As>
template <typename F, std::enable_if_t<!std::is_same_v<std::decay_t<F>, Function<R(As...)>>, int>>
Function<R(As...)>::Function(F &&f) : invocable_ptr_(
    detail::make_invocable<R(As...)>(
        std::in_place_type<std::decay_t<F>>,
        std::forward<F>(f)
    )
) { }

/**
 *  @tparam std::decay_t<F> must be default constructible.
 *  @returns a Function that wraps a default constructed object of
 *           type std::decay_t<F>.
 *
 *  @throws std::bad_alloc
 *  @throws aany exceptions that the default constructor of
 *          std::decay_t<F> throws.
 */
template <typename R, typename ...As>
template <typename F>
Function<R(As...)>::Function(std::in_place_type_t<F>) : invocable_ptr_(
    detail::make_invocable<R(As...)>(std::in_place_type<std::decay_t<F>>)
) { }

/**
 *  @tparam std::decay_t<F> Must be constructible from (U, Us...).
 *  @returns a Function that wraps an object of type
 *           std::decay_t<F>, direct initialized from
 *           (std::forward<U>(u), std::forward<Us>(us)...).
 *
 *  @throws std::bad_alloc
 *  @throws any exceptions that the constructor of std::decay_t<F>
 *          throws.
 */
template <typename R, typename ...As>
template <typename F, typename U, typename ...Us>
Function<R(As...)>::Function(std::in_place_type_t<F>, U &&u, Us &&...us) : invocable_ptr_(
    detail::make_invocable<R(As...)>(
        std::in_place_type<F>,
        std::forward<U>(u),
        std::forward<Us>(us)...
    )
) { }

/**
 *  @tparam std::decay_t<F> Must be constructible from
 *          (std::initializer_list<U>&, Us...).
 *  @returns a Function that wraps an object of type
 *           std::decay_t<F>, direct initialized from
 *           (list, std::forward<Us>(us)...).
 *
 *  @throws std::bad_alloc
 *  @throws any exceptions that the constructor of std::decay_t<F>
 *          throws.
 */
template <typename R, typename ...As>
template <typename F, typename U, typename ...Us>
Function<R(As...)>::Function(std::in_place_type_t<F>, std::initializer_list<U> list, Us &&...us)
: invocable_ptr_(
    detail::make_invocable<R(As...)>(std::in_place_type<F>, list, std::forward<Us>(us)...)
) { }

/**
 *  @returns this Function, which now wraps an object copied from
 *           other's wrapped object.
 *
 *  @throws std::bad_alloc
 *  @throws any exceptions that the copy constructor of other's
 *          wrapped object throws.
 */
template <typename R, typename ...As>
Function<R(As...)>::Function(const Function &other) : invocable_ptr_(other.clone()) { }

/**
 *  @returns this Function, which now wraps an object copied from
 *           other's wrapped object.
 *
 *  @throws std::bad_alloc
 *  @throws any exceptions that the copy constructor of other's
 *          wrapped object throws.
 */
template <typename R, typename ...As>
Function<R(As...)>& Function<R(As...)>::operator=(const Function &other) {
    if (this != &other) {
        invocable_ptr_ = other.clone();
    }

    return *this;
}

/**
 *  If an exception is thrown, this Function will remain unchanged.
 *
 *  @tparam std::decay_t<F> must not be an object of type Function.
 *          Must be constructible from (F).
 *  @returns this Function, which now wraps an object of type
 *           std::decay_t<F> direct initialized from
 *           (std::forward<F>(f)).
 *
 *  @throws std::bad_alloc
 *  @throws any exceptions that the constructor of std::decay_t<F>
 *          throws.
 */
template <typename R, typename ...As>
template <typename F, std::enable_if_t<!std::is_same_v<std::decay_t<F>, Function<R(As...)>>, int>>
Function<R(As...)>& Function<R(As...)>::operator=(F &&f) {
    invocable_ptr_ = detail::make_invocable<R(As...)>(
        std::in_place_type<std::decay_t<F>>,
        std::forward<F>(f)
    );

    return *this;
}

/**
 *  Constructs a new wrapped object of type std::decay_t<F>,
 *  direct initialized from (std::forward<Us>(us)...).
 *
 *  @tparam std::decay_t<F> must be constructible from (Us...).
 *
 *  @throws std::bad_alloc
 *  @throws any exceptions that the constructor of std::decay_t<F>
 *          throws.
 */
template <typename R, typename ...As>
template <typename F, typename ...Us>
void Function<R(As...)>::emplace(Us &&...us) {
    invocable_ptr_ = detail::make_invocable<R(As...)>(
        std::in_place_type<std::decay_t<F>>,
        std::forward<Us>(us)...
    );
}

/**
 *  Constructs a new wrapped object of type std::decay_t<F>,
 *  direct initialized from (list, std::forward<Us>(us)...).
 *
 *  @tparam std::decay_t<F> must be constructible from
 *          (std::initializer_list<U>&, Us...).
 *
 *  @throws std::bad_alloc
 *  @throws any exceptions that the constructor of std::decay_t<F>
 *          throws.
 */
template <typename R, typename ...As>
template <typename F, typename U, typename ...Us>
void Function<R(As...)>::emplace(std::initializer_list<U> list, Us &&...us) {
    invocable_ptr_ = detail::make_invocable<R(As...)>(
        std::in_place_type<std::decay_t<F>>,
        list,
        std::forward<Us>(us)...
    );
}

/**
 *  Deallocates and destroys this Function's wrapped object, if
 *  there is one.
 */
template <typename R, typename ...As>
void Function<R(As...)>::reset() noexcept {
    invocable_ptr_.reset();
}

/** Swaps ownership of wrapped objects with another Function. */
template <typename R, typename ...As>
void Function<R(As...)>::swap(Function &other) noexcept {
    invocable_ptr_.swap(other.invocable_ptr_);
}

/**
 *  @param this must wrap an object.
 *  @returns the result of invoking the wrapped object with
 *           parameters (std::forward<As>(as...)).
 *
 *  @throws any exceptions that the wrapped object throws on
 *          invocation.
 */
template <typename R, typename ...As>
R Function<R(As...)>::operator()(As ...as) {
    assert(invocable_ptr_);

    return (*invocable_ptr_)(std::forward<As>(as)...);
}

/** @returns true if this Function currently wraps an object. */
template <typename R, typename ...As>
Function<R(As...)>::operator bool() const noexcept {
    return invocable_ptr_ != nullptr;
}

/** Swaps ownership of two Function's wrapped objects. */
template <typename R, typename ...As>
void swap(Function<R(As...)> &lhs, Function<R(As...)> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename R, typename ...As>
std::unique_ptr<detail::Invocable<R(As...)>> Function<R(As...)>::clone() const {
    if (!invocable_ptr_) {
        return nullptr;
    }

    return invocable_ptr_->clone();
}

} // namespace fn2

#endif
