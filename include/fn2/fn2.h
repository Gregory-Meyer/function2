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
#include <cstddef>
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
 *  If the wrapped object is small enough and has a suitable alignment,
 *  it will be stored inside the Function object without dynamic
 *  allocation. Otherwise, the wrapped object will be stored on the
 *  free store and handled via std::malloc()/free().
 */
template <typename R, typename ...As>
class Function<R(As...)> {
public:
    /** @returns a Function that does not wrap any object. */
    inline Function() noexcept;

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
    inline Function(Function &&other) noexcept;

    /** Deallocates and destroys any wrapped object. */
    inline ~Function();

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
     *  Swaps ownership of this Function's wrapped object with other.
     *
     *  @param other will no longer wrap an object.
     *  @returns this Function, which now that wraps the object that
     *           other wrapped.
     */
    inline Function& operator=(Function &&other) noexcept;

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
    inline R operator()(As ...as) const;

    /** @returns true if this Function currently wraps an object. */
    inline explicit operator bool() const noexcept;

private:
    using Storage = std::aligned_storage_t<16 * sizeof(float) - sizeof(bool) - sizeof(void*)>;

    template <typename F, typename ...Ts>
    inline void construct(Ts &&...ts);

    inline void*& as_ptr() noexcept;

    inline void* as_ptr() const noexcept;

    mutable Storage storage_;
    bool is_ptr_;
    const detail::Vtable<R, As...> *vptr_ = nullptr;
};

/** Swaps ownership of two Function's wrapped objects. */
template <typename R, typename ...As>
inline void swap(Function<R(As...)> &lhs, Function<R(As...)> &rhs) noexcept;

/** @returns a Function that does not wrap any object. */
template <typename R, typename ...As>
Function<R(As...)>::Function() noexcept { }

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
Function<R(As...)>::Function(F &&f) {
    construct<F>(std::forward<F>(f));
}

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
Function<R(As...)>::Function(std::in_place_type_t<F>) {
    construct<F>();
}

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
Function<R(As...)>::Function(std::in_place_type_t<F>, U &&u, Us &&...us) {
    construct<F>(std::forward<U>(u), std::forward<Us>(us)...);
}

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
Function<R(As...)>::Function(std::in_place_type_t<F>, std::initializer_list<U> list, Us &&...us) {
    construct<F>(list, std::forward<Us>(us)...);
}

/**
 *  @returns this Function, which now wraps an object copied from
 *           other's wrapped object.
 *
 *  @throws std::bad_alloc
 *  @throws any exceptions that the copy constructor of other's
 *          wrapped object throws.
 */
template <typename R, typename ...As>
Function<R(As...)>::Function(const Function &other) : vptr_(other.vptr_) {
    if (!vptr_) {
        return;
    }

    is_ptr_ = other.is_ptr_;

    if (is_ptr_) {
        as_ptr() = vptr_->clone(other.as_ptr());
    } else {
        vptr_->copy(&storage_, &other.storage_);
    }
}

/**
 *  @param other will no longer wrap an object.
 *  @returns a Function that wraps the object that other wrapped.
 */
template <typename R, typename ...As>
Function<R(As...)>::Function(Function &&other) noexcept : vptr_(other.vptr_) {
    if (!vptr_) {
        return;
    }

    is_ptr_ = other.is_ptr_;

    if (is_ptr_) {
        as_ptr() = other.as_ptr();
        other.vptr_ = nullptr;
    } else {
        vptr_->move(&storage_, &other.storage_);
    }
}

/** Deallocates and destroys any wrapped object. */
template <typename R, typename ...As>
Function<R(As...)>::~Function() {
    reset();
}

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
        Function copy(other);
        swap(copy);
    }

    return *this;
}

/**
 *  Swaps ownership of this Function's wrapped object with other.
 *
 *  @param other will no longer wrap an object.
 *  @returns this Function, which now that wraps the object that
 *           other wrapped.
 */
template <typename R, typename ...As>
Function<R(As...)>& Function<R(As...)>::operator=(Function &&other) noexcept {
    if (this != &other) {
        swap(other);
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
    Function new_func = std::forward<F>(f);
    swap(new_func);

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
    Function g(std::in_place_type<F>, std::forward<Us>(us)...);
    swap(g);
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
    Function g(std::in_place_type<F>, list, std::forward<Us>(us)...);
    swap(g);
}

/**
 *  Deallocates and destroys this Function's wrapped object, if
 *  there is one.
 */
template <typename R, typename ...As>
void Function<R(As...)>::reset() noexcept {
    if (!vptr_) {
        return;
    }

    if (is_ptr_) {
        vptr_->destroy(as_ptr());
        std::free(as_ptr());
        as_ptr() = nullptr;
    } else {
        vptr_->destroy(&storage_);
    }

    vptr_ = nullptr;
}

/** Swaps ownership of wrapped objects with another Function. */
template <typename R, typename ...As>
void Function<R(As...)>::swap(Function &other) noexcept {
    if (this == &other || (!vptr_ && !other.vptr_)) {
        return;
    }

    if (vptr_ == other.vptr_ && !is_ptr_ && !other.is_ptr_) {
        vptr_->swap(&storage_, &other.storage_);
    } else if (!vptr_) {
        is_ptr_ = other.is_ptr_;

        if (is_ptr_) {
            as_ptr() = other.as_ptr();
            other.as_ptr() = nullptr;
        } else {
            other.vptr_->move(&storage_, &other.storage_);
            other.vptr_->destroy(&other.storage_);
        }
    } else if (!other.vptr_) {
        other.is_ptr_ = is_ptr_;

        if (is_ptr_) {
            other.as_ptr() = as_ptr();
            as_ptr() = nullptr;
        } else {
            vptr_->move(&other.storage_, &storage_);
            vptr_->destroy(&storage_);
        }
    } else if (is_ptr_) {
        if (other.is_ptr_) {
            std::swap(as_ptr(), other.as_ptr());
        } else {
            Storage temp;
            other.vptr_->move(&temp, &other.storage_);
            other.vptr_->destroy(&other.storage_);

            other.is_ptr_ = true;
            other.as_ptr() = as_ptr();

            is_ptr_ = false;
            other.vptr_->move(&storage_, &temp);
            other.vptr_->destroy(&temp);
        }
    } else {
        if (other.is_ptr_) {
            Storage temp;
            vptr_->move(&temp, &storage_);
            vptr_->destroy(&storage_);

            is_ptr_ = true;
            as_ptr() = other.as_ptr();

            other.is_ptr_ = false;
            vptr_->move(&other.storage_, &temp);
            vptr_->destroy(&temp);
        } else {
            Storage temp;
            vptr_->move(&temp, &storage_);
            vptr_->destroy(&storage_);

            other.vptr_->move(&storage_, &other.storage_);
            other.vptr_->destroy(&other.storage_);

            vptr_->move(&other.storage_, &temp);
            vptr_->destroy(&temp);
        }
    }

    std::swap(vptr_, other.vptr_);
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
R Function<R(As...)>::operator()(As ...as) const {
    assert(vptr_);

    if (is_ptr_) {
        return vptr_->invoke(as_ptr(), std::forward<As>(as)...);
    } else {
        return vptr_->invoke(&storage_, std::forward<As>(as)...);
    }
}

/** @returns true if this Function currently wraps an object. */
template <typename R, typename ...As>
Function<R(As...)>::operator bool() const noexcept {
    return vptr_ != nullptr;
}

/** Swaps ownership of two Function's wrapped objects. */
template <typename R, typename ...As>
void swap(Function<R(As...)> &lhs, Function<R(As...)> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename R, typename ...As>
template <typename F, typename ...Ts>
void Function<R(As...)>::construct(Ts &&...ts) {
    static_assert(
        std::is_constructible_v<std::decay_t<F>, Ts...>,
        "std::decay_t<F> must be constructible from (Ts...)"
    );

    using Obj = std::decay_t<F>;

    assert(!vptr_);

    vptr_ = &detail::get_vtbl<Obj, R, As...>();

    if constexpr (sizeof(Obj) <= sizeof(Storage) && alignof(Storage) % alignof(Obj) == 0) {
        is_ptr_ = false;
        new (&storage_) Obj(std::forward<Ts>(ts)...);
    } else {
        void *const ptr = std::malloc(sizeof(Obj));

        if (!ptr) {
            throw std::bad_alloc();
        }

        try {
            new (ptr) Obj(std::forward<Ts>(ts)...);
        } catch (...) {
            std::free(ptr);

            throw;
        }

        is_ptr_ = true;
        as_ptr() = ptr;
    }
}

template <typename R, typename ...As>
void*& Function<R(As...)>::as_ptr() noexcept {
    assert(is_ptr_);

    return *reinterpret_cast<void**>(&storage_);
}

template <typename R, typename ...As>
void* Function<R(As...)>::as_ptr() const noexcept {
    assert(is_ptr_);

    return *reinterpret_cast<void *const *>(&storage_);
}

} // namespace fn2

#endif
