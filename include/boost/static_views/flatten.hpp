//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/flatten.hpp
///
/// \brief Implementation of #flatten.

#ifndef BOOST_STATIC_VIEWS_FLATTEN_HPP
#define BOOST_STATIC_VIEWS_FLATTEN_HPP

#include "algorithm_base.hpp"
#include "view_base.hpp"

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper>
struct flatten_view_impl
    : view_adaptor_base<flatten_view_impl<Wrapper>, Wrapper> {

  private:
    using wrapper_type = Wrapper;
    using base = view_adaptor_base<flatten_view_impl<Wrapper>, Wrapper>;

  public:
    using typename base::index_type;
    using typename base::size_type;
    using reference = typename base::value_type::reference;
    using value_type = std::remove_reference_t<reference>;
    using base::parent;

  public:
    BOOST_STATIC_VIEWS_CONSTEXPR flatten_view_impl(Wrapper&& xs)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<base, Wrapper&&>::value)
        : base{std::move(xs)}
    {
    }

    flatten_view_impl(flatten_view_impl const&) = default;
    flatten_view_impl(flatten_view_impl&&)      = default;
    flatten_view_impl& operator=(flatten_view_impl const&) = default;
    flatten_view_impl& operator=(flatten_view_impl&&) = default;

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        constexpr auto a = wrapper_type::value_type::extent();
        constexpr auto b = wrapper_type::value_type::value_type::extent();
        if (a == dynamic_extent || b == dynamic_extent) {
            return dynamic_extent;
        }
        else {
            return a * b;
        }
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto size() const noexcept -> size_type
    {
        if (parent().size() != 0) {
            return parent().size() * parent().unsafe_at(0).size();
        }
        else {
            return 0;
        }
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) unsafe_at(index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::flatten_view_impl::unsafe_at: Precondition "
            "`0 <= i < size()` is not satisfied.");
        auto const n      = static_cast<index_type>(parent().unsafe_at(0).size());
        auto const first  = i / n;
        auto const second = i % n;
        return parent().unsafe_at(first).unsafe_at(second);
    }
};

struct flatten_impl {
  private:
    template <class Wrapper>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(Wrapper xs) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            flatten_view_impl<Wrapper>{std::move(xs)});

    // clang-format off
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(View<remove_cvref_t<V>>)
    BOOST_STATIC_VIEWS_CONSTEXPR auto call(V&& xs, int /*unused*/) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<flatten_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)))))
    {
        return call_impl(make_wrapper(std::forward<V>(xs)));
    }

#if !defined(BOOST_STATIC_VIEWS_SFINAE)
    template <class V
        BOOST_STATIC_VIEWS_REQUIRES(!View<remove_cvref_t<V>>)
    constexpr auto call(V&& /*unused*/, ...) const
        noexcept
    {
        static_assert(View<std::remove_cv_t<std::remove_reference_t<V>>>,
            "`boost::static_views::flatten(xs)` requires xs to model "
            "the View concept.");
        return why_is_my_argument_not_a_view<remove_cvref_t<V>>();
    }
#endif

  public:
    template <class V>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(V&& xs) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<flatten_impl const&>().call(std::forward<V>(xs), int{})))
    {
        return call(std::forward<V>(xs), int{});
    }

    template <class F>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()() const noexcept
    {
        return lazy_adaptor(flatten_impl{});
    }
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::flatten_impl, flatten)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_FLATTEN_HPP

