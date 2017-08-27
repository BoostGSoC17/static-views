//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ITERATOR_HPP
#define BOOST_STATIC_VIEWS_ITERATOR_HPP

#include "detail/config.hpp"
#include "detail/utils.hpp"
#include <iterator>
#include <limits>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class View>
struct view_iterator {

    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using reference         = decltype(std::declval<
        std::decay_t<View>>()[std::declval<size_type>()]);
    using value_type        = std::remove_reference_t<reference>;
    using pointer           = std::add_pointer_t<reference>;
    using iterator_category = std::random_access_iterator_tag;

  private:
    View*          _view;
    std::ptrdiff_t _i;

    BOOST_STATIC_VIEWS_CONSTEXPR auto is_dereferenceable() const
        noexcept -> bool
    {
        return _view != nullptr && _i >= 0
               && static_cast<size_type>(_i) <= _view->size();
    }

    struct Impl {

      private:
        template <class T>
        using has_unsafe_at_t = decltype(
            std::declval<T>().unsafe_at(std::declval<std::size_t>()));

        BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_unsafe_at, T,
            (is_detected<has_unsafe_at_t, T>::value), "");

        BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_unsafe_at, T,
            (noexcept(std::declval<T>().unsafe_at(
                std::declval<std::size_t>()))),
            "");

        // clang-format off
        template <class T,
            class = std::enable_if_t<concepts::and_<Has_unsafe_at,
                Is_noexcept_unsafe_at>::template test<T>()>,
            class = void>
        BOOST_STATIC_VIEWS_FORCEINLINE
        static BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at_impl(T&& xs,
            std::size_t const i) noexcept
        {
            return std::forward<T>(xs).unsafe_at(i);
        }
        // clang-format on

        // clang-format off
        template <class T,
            class = std::enable_if_t<!concepts::and_<Has_unsafe_at,
                Is_noexcept_unsafe_at>::template test<T>()>>
        BOOST_STATIC_VIEWS_FORCEINLINE
        static BOOST_STATIC_VIEWS_CONSTEXPR
        auto unsafe_at_impl(T&& xs, std::size_t const i)
        BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
        (
            std::forward<T>(xs)[i]
        );
        // clang-format on

      public:
        // clang-format off
        template <class T>
        static BOOST_STATIC_VIEWS_CONSTEXPR
        auto unsafe_at(T&& xs, std::size_t const i)
        BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
        (
            unsafe_at_impl(std::forward<T>(xs), i)
        );
    };


  public:
    BOOST_STATIC_VIEWS_CONSTEXPR view_iterator(
        View* view = nullptr, size_type const i = 0)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
        : _view{view}, _i{static_cast<difference_type>(i)}
    {
        BOOST_STATIC_VIEWS_EXPECT(
            i < static_cast<size_type>(
                    std::numeric_limits<difference_type>::max()),
            "Overflow detected.");
        BOOST_STATIC_VIEWS_EXPECT(
            view == nullptr && i == 0
                || view != nullptr && i <= view->size(),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_iterator(view_iterator const&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_iterator(view_iterator&&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_iterator& operator=(view_iterator const&) noexcept = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_iterator& operator=(view_iterator&&) noexcept = default;

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto* operator-> () const BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(
        noexcept(std::declval<View&>().unsafe_at(
            std::declval<size_type>())))
    {
        BOOST_STATIC_VIEWS_EXPECT(is_dereferenceable(),
            "Nah, this iterator is not dereferenceable.");
        return &Impl::unsafe_at(*_view, static_cast<size_type>(_i));
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO
    operator*() const BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(noexcept(
        std::declval<View&>().unsafe_at(std::declval<size_type>())))
    {
        BOOST_STATIC_VIEWS_EXPECT(is_dereferenceable(),
            "Nah, this iterator is not dereferenceable.");
        return Impl::unsafe_at(*_view, static_cast<size_type>(_i));
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO
    operator[](difference_type const n) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_iterator const&>().operator*()))
    {
        return *(*this + n);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator++() noexcept -> view_iterator&
    {
        ++_i;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator--() noexcept -> view_iterator&
    {
        --_i;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator++(int) noexcept -> view_iterator
    {
        view_iterator temp{*this};
        ++(*this);
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator--(int) noexcept -> view_iterator
    {
        view_iterator temp{*this};
        --(*this);
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator+=(difference_type const n) noexcept
        -> view_iterator&
    {
        _i += n;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator-=(difference_type const n) noexcept
        -> view_iterator&
    {
        _i -= n;
        return *this;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator+(view_iterator const& x,
        difference_type const n) noexcept -> view_iterator
    {
        view_iterator temp{x};
        temp += n;
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator-(view_iterator const& x,
        difference_type const n) noexcept -> view_iterator
    {
        view_iterator temp{x};
        temp -= n;
        return temp;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator+(difference_type const n,
        view_iterator const& x) noexcept -> view_iterator
    {
        return x + n;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator-(view_iterator const& x,
        view_iterator const& y) noexcept -> difference_type
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to subtract iterators to different "
            "objects.");
        return x._i - y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator==(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i == y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator!=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i != y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator<(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i < y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator>(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i > y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator<=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i <= y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator>=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to compare iterators to different "
            "objects.");
        return x._i >= y._i;
    }
};

} // namespace detail

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ITERATOR_HPP
