//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
#define BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP

#include <functional>
#include <type_traits>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/view_base.hpp> // new

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

struct find_first_i_impl {

  private:
    template <class V, class P>
    struct Constrains {

      private:
        template <class Predicate>
        struct Impl {
            template <class T>
            using element_type_t = decltype(
                concepts::View::unsafe_at(std::declval<T const&>(),
                    std::declval<std::size_t>()));

            template <class T>
            using has_call_t =
                decltype(invoke(std::declval<Predicate>(),
                    std::declval<element_type_t<T>>()));

            BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_callable, T,
                (is_detected<has_call_t, T>::value),
                "`boost::static_views::detail::find_first_i(xs, p)` "
                "requires `p` to be callable with `xs`' element type. "
                "This condition has, I'm afraid, been violated. Hence, "
                "the error.");

            BOOST_STATIC_VIEWS_DEFINE_CHECK(Call_has_correct_return_type,
                T,
                (std::is_convertible<detected_t<has_call_t, T>,
                    bool>::value),
                "`boost::static_views::detail::find_first_i(xs, p)` "
                "requires `p(xs[i])` to return something convertible to "
                "`bool`. That's, unfortunately, not the case here. "
                "Hence, the error.");

            template <class T>
            static constexpr auto test() noexcept -> bool
            {
                return concepts::and_<concepts::View, Is_callable,
                    Call_has_correct_return_type>::template test<T>();
            }
            template <class T>
            static constexpr auto check() noexcept -> bool
            {
                return concepts::and_<concepts::View, Is_callable,
                    Call_has_correct_return_type>::template check<T>();
            }
        };

      public:
        static constexpr auto test() noexcept -> bool
        {
            return Impl<P>::template test<V>();
        }

        static constexpr auto check() noexcept -> bool
        {
            return Impl<P>::template check<V>();
        }
    };

  public:
    template <class View, class Predicate,
        class =
            std::enable_if_t<!Constrains<View, Predicate>::test()>,
        class = void>
    BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(
        View const& /*unused*/, Predicate&& /*unused*/) const noexcept
    {
        Constrains<View, Predicate>::check();
    }

    // clang-format off
    template <class View, class Predicate,
        class = std::enable_if_t<Constrains<View, Predicate>::test()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(View const& xs, Predicate&& p) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
            noexcept(xs.size()),
            noexcept(p(xs[std::declval<std::size_t>()]))
        ))
        -> std::size_t
    {
        std::size_t const n = xs.size();
        std::size_t       i = 0;

        while (i < n && !invoke(p, concepts::View::unsafe_at(xs, i))) {
            ++i;
        }
        return i;
    }
    // clang-format on
};

} // namespace detail

BOOST_STATIC_VIEWS_INLINE_VARIABLE(
    detail::find_first_i_impl, find_first_i)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_DETAIL_FIND_FIRST_HPP
