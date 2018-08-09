//          Copyright Tom Westerhout 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/transform.hpp
///
/// \brief Implementation of #transform.

#ifndef BOOST_STATIC_VIEWS_TRANSFORM_HPP
#define BOOST_STATIC_VIEWS_TRANSFORM_HPP

#include "algorithm_base.hpp"
#include "detail/invoke.hpp"
#include "view_base.hpp"

#include <algorithm>
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Wrapper, class Function>
struct transform_view_impl
    : view_adaptor_base<transform_view_impl<Wrapper, Function>, Wrapper>
    , private Function {

  private:
    using wrapper_type = Wrapper;
    using base = view_adaptor_base<transform_view_impl<Wrapper, Function>, Wrapper>;
    using function_type = typename Function::value_type;

  public:
    using typename base::index_type;
    using typename base::size_type;
    using reference  = decltype(invoke(std::declval<function_type const&>(),
        std::declval<typename base::reference>()));
    using value_type = std::remove_reference_t<reference>;
    using base::parent;
    using base::size;

    BOOST_STATIC_VIEWS_CONSTEXPR transform_view_impl(
        Wrapper&& xs, Function&& func)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_constructible<base, Wrapper&&>::value&&
                std::is_nothrow_move_constructible<Function>::value)
        : base{std::move(xs)}, Function{std::move(func)}
    {
    }

    transform_view_impl(transform_view_impl const&) = default;
    transform_view_impl(transform_view_impl&&)      = default;
    transform_view_impl& operator=(transform_view_impl const&) = default;
    transform_view_impl& operator=(transform_view_impl&&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    decltype(auto) unsafe_at(index_type const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_CHECKS_IF(true)
    {
        BOOST_STATIC_VIEWS_EXPECT(0 <= i && i < static_cast<index_type>(size()),
            "boost::static_views::transform_view_impl::map: Precondition "
            "`0 <= i < size()` is not satisfied.");
        return invoke(
            static_cast<Function const&>(*this).get(), parent().unsafe_at(i));
    }
};

struct transform_impl {
  private:
    template <class Wrapper, class Function>
    BOOST_STATIC_VIEWS_CONSTEXPR auto call_impl(Wrapper xs, Function func) const
        BOOST_STATIC_VIEWS_AUTO_NOEXCEPT_RETURN(
            transform_view_impl<Wrapper, Function>{
                std::move(xs), std::move(func)});

    template <class T>
    using index_t =
        typename std::remove_cv_t<std::remove_reference_t<T>>::index_type;

    // clang-format off
    template <class V, class F
        BOOST_STATIC_VIEWS_REQUIRES(
            View<std::remove_cv_t<std::remove_reference_t<V>>>
        && is_invocable<F, typename std::remove_cv_t<
                                    std::remove_reference_t<
                                        V>>::reference>::value)
    BOOST_STATIC_VIEWS_CONSTEXPR auto call(V&& xs, F&& f, int /*unused*/) const
        // clang-format on
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<transform_impl const&>().call_impl(
                make_wrapper(std::forward<V>(xs)),
                make_wrapper(std::forward<F>(f)))))
    {
        return call_impl(make_wrapper(std::forward<V>(xs)),
            make_wrapper(std::forward<F>(f)));
    }

#if !defined(BOOST_STATIC_VIEWS_SFINAE)
    template <class V, class F>
    constexpr auto call(V&& /*unused*/, F&& /*unused*/, ...) const
        noexcept
    {
        static_assert(View<std::remove_cv_t<std::remove_reference_t<V>>>,
            "`boost::static_views::transform(xs, func)` requires xs to model "
            "the View concept.");
        static_assert(
            is_invocable<F, typename std::remove_cv_t<
                                std::remove_reference_t<V>>::reference>::value,
            "`boost::static_views::transform(xs, func)` requires func to be "
            "invocable with an element of xs.");
    }
#endif

  public:
    template <class V, class F>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(V&& xs, F&& f) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<transform_impl const&>().call(
                std::forward<V>(xs), std::forward<F>(f))))
    {
        return call(std::forward<V>(xs), std::forward<F>(f), int{});
    }

    template <class F>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(F&& func) const noexcept
    {
        return lazy_adaptor(transform_impl{}, std::forward<F>(func));
    }
};

} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::transform_impl, transform)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_TRANSFORM_HPP

