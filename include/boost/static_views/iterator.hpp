//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_ITERATOR_HPP
#define BOOST_STATIC_VIEWS_ITERATOR_HPP

#include "detail/config.hpp"
#include "view_concept.hpp"
#include <iterator>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

template <class V
    BOOST_STATIC_VIEWS_REQUIRES(View<V>)
struct view_iterator {

    using view_type         = V;
    using size_type         = typename V::size_type;
    using difference_type   = typename V::index_type;
    using value_type        = typename V::value_type;
    using reference         = typename V::reference;
    using pointer           = typename V::pointer;
    using iterator_category = std::random_access_iterator_tag;

  private:
    view_type*      _view;
    difference_type _i;

    BOOST_STATIC_VIEWS_CONSTEXPR auto is_dereferenceable() const
        noexcept -> bool
    {
        return _view != nullptr && _i >= 0
               && static_cast<size_type>(_i) <= _view->size();
    }

    static constexpr auto unsafe_at_is_noexcept() noexcept -> bool
    {
        return noexcept(std::declval<view_type&>().unsafe_at(
            std::declval<difference_type>()));
    }

  public:
    BOOST_STATIC_VIEWS_CONSTEXPR
    explicit view_iterator(
        view_type* const view = nullptr, difference_type const i = 0)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
        : _view{view}, _i{i}
    {
        BOOST_STATIC_VIEWS_EXPECT(
            view == nullptr && i == 0
                || view != nullptr && 0 <= i && i <= view->size(),
            "It's a bad idea to create an iterator pointing neither "
            "into the view nor to the one-past-the-end element.");
    }

    view_iterator(view_iterator const&) = default;
    view_iterator(view_iterator&&)      = default;
    view_iterator& operator=(view_iterator const&) = default;
    view_iterator& operator=(view_iterator&&) = default;

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto* operator-> () const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(unsafe_at_is_noexcept())
    {
        BOOST_STATIC_VIEWS_EXPECT(is_dereferenceable(),
            "Nah, this iterator is not dereferenceable.");
        return &_view->unsafe_at(_i);
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator*() const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(unsafe_at_is_noexcept())
    {
        BOOST_STATIC_VIEWS_EXPECT(is_dereferenceable(),
            "Nah, this iterator is not dereferenceable.");
        return _view->unsafe_at(_i);
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
    friend auto operator+(view_iterator x,
        difference_type const           n) noexcept -> view_iterator
    {
        return x += n;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator-(view_iterator x,
        difference_type const           n) noexcept -> view_iterator
    {
        return x -= n;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator+(difference_type const n,
        view_iterator x) noexcept -> view_iterator
    {
        return x += n;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator-(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> difference_type
    {
        BOOST_STATIC_VIEWS_EXPECT(x._view == y._view,
            "It's a bad idea to subtract "
            "iterators to different views.");
        return x._i - y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator==(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(
            x._view == y._view, "It's a bad idea to compare "
                                "iterators to different views.");
        return x._i == y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator!=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(
            x._view == y._view, "It's a bad idea to compare "
                                "iterators to different views.");
        return x._i != y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator<(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(
            x._view == y._view, "It's a bad idea to compare "
                                "iterators to different views.");
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
        BOOST_STATIC_VIEWS_EXPECT(
            x._view == y._view, "It's a bad idea to compare "
                                "iterators to different views.");
        return x._i <= y._i;
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    friend auto operator>=(
        view_iterator const& x, view_iterator const& y)
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true) -> bool
    {
        BOOST_STATIC_VIEWS_EXPECT(
            x._view == y._view, "It's a bad idea to compare "
                                "iterators to different views.");
        return x._i >= y._i;
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_ITERATOR_HPP
