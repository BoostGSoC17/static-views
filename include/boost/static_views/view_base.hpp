//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implementation of boost::static_views::view_base,
// boost::static_views::is_view,
// boost::static_views::view_adaptor_core_access and
// boost::static_views::view_adaptor_base.

#ifndef BOOST_STATIC_VIEWS_VIEW_BASE_HPP
#define BOOST_STATIC_VIEWS_VIEW_BASE_HPP

#include "detail/config.hpp"
#include "detail/utils.hpp"
#include "detail/wrapper.hpp"
#include "errors.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Base class for all the views.

/// \verbatim embed:rst:leading-slashes
/// By deriving from :cpp:class:`view_base`, you tell StaticView that
/// the derived class models the :ref:`view <view-concept>` concept.
/// \endverbatim
struct view_base {
};

/// \brief Checks whether `V` models the View concept.

/// \verbatim embed:rst:leading-slashes
/// Metafunction that returns whether ``V`` models the :ref:`View
/// <view-concept>` concept:
///
/// .. code-block:: cpp
///
///   template <class View>
///   struct is_view : std::is_base_of<view_base, View>
///   {};
///
/// \endverbatim
// template <class V>
// struct is_view : std::is_base_of<view_base, V> {
// };

/// \brief Helper class one can befriend to give StaticViews access to
/// the private `map` function.

/// Defines a single private member function
/// \code
/// template <class View>
/// static constexpr auto map(View const& xs, std::size_t const i)
///     noexcept( whenever possible );
/// \endcode
/// that calls `xs.map(i)`. By befriending this class, you give
/// #boost::static_views::view_adaptor_base access to private `map`
/// function,
/// but nothing else. The very same technique is used in
/// [Boost.Iterator](http://www.boost.org/doc/libs/1_64_0/libs/iterator/doc/iterator_facade.html#iterator-core-access).
struct view_adaptor_core_access {
    template <class T, class V>
    friend struct view_adaptor_base;

    //  private: This is unfortunate and is to be fixed later!

    // clang-format off
    template <class View>
    static BOOST_STATIC_VIEWS_CONSTEXPR
    auto map(View const& xs, std::size_t const i)
    BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
    (
        xs.map(i)
    );
    // clang-format on
};

namespace concepts {
namespace view {

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Derives_from_view_base, T,
        (std::is_base_of<view_base, T>::value),
        "`T` doesn't derive from `boost::static_views::view_base`, "
        "which is, unfortunately, required by the View concept. "
        "Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_move_constructible, T,
        (std::is_move_constructible<T>::value),
        "`T` doesn't model the MoveConstructible concept, which "
        "it should, to have a chance at modeling the View concept. "
        "Hence, the error.");

    template <class T>
    using has_static_extent_t = decltype(T::extent());

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_static_extent, T,
        (is_detected<has_static_extent_t, T>::value),
        "`T` has no static member function `extent()`. "
        "Availability of it is, unfortunately, required by the "
        "View concept. Hence, the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type_extent, T,
        (std::is_convertible<detected_t<has_static_extent_t, T>,
            std::size_t>::value),
        "The View concept requires the return type of "
        "`T::extent()` to be convertible to `size_t`. This "
        "condition is, I'm afraid, not satisfied. Hence, the "
        "error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_extent, T,
        (noexcept(T::extent())),
        "`T::extent()` is not noexcept while it should be "
        "to have a chance at modeling the View concept. Hence, "
        "the error.");

    template <class T>
    using has_member_size_t =
        decltype(std::declval<T const&>().size());

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_member_size, T,
        (is_detected<has_member_size_t, T>::value),
        "`T` has no member function `size()`. Availability of it "
        "is, unfortunately, required by the View concept. Hence, "
        "the error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type_size, T,
        (std::is_convertible<detected_t<has_member_size_t, T>,
            std::size_t>::value),
        "The View concept requires the return type of the "
        "`size()` member function to be convertible to `size_t`. "
        "This condition is, I'm afraid, not satisfied. Hence, the "
        "error.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_size, T,
        (noexcept(std::declval<T const&>().size())),
        "`T`'s `size()` member function is not noexcept while it "
        "should be for `T` to have a chance at modeling the View "
        "concept. Hence, the error.");

    template <class T>
    using has_operator_access_t =
        decltype(std::declval<T>()[std::declval<std::size_t>()]);

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_operator_access, T,
        (is_detected<has_operator_access_t, T>::value),
        "`T` has no `operator[](size_t)`. Availability of it "
        "is, unfortunately, required by the View concept. Hence, "
        "the error.");

    template <class T>
    using has_member_map_t = decltype(view_adaptor_core_access::map(
        std::declval<T>(), std::declval<std::size_t>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_member_map, T,
        (is_detected<has_member_map_t, T>::value),
        "`T` is required to have a member function `map(size_t)` "
        "accessible through "
        "`boost::static_views::view_adaptor_core_access`. Either "
        "define a it publicly or befriend the "
        "`view_adaptor_core_access` struct.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_correct_return_type_map, T,
        (std::is_convertible<detected_t<has_member_map_t, T>,
            std::size_t>::value),
        "`T`'s `map(size_t)` should have return type convertible to "
        "size_t.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_map, T,
        (noexcept(view_adaptor_core_access::map(
            std::declval<T>(), std::declval<std::size_t>()))),
        "`T`'s `map(size_t)` member function is not noexcept while "
        "it "
        "should be. Hence, the error.");

    template <class T>
    using has_unsafe_at_t = decltype(
        std::declval<T>().unsafe_at(std::declval<std::size_t>()));

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Has_unsafe_at, T,
        (is_detected<has_unsafe_at_t, T>::value),
        "`T` is required to have a member function "
        "`unsafe_at(size_t)`.");

    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_noexcept_unsafe_at, T,
        (noexcept(std::declval<T>().unsafe_at(
            std::declval<std::size_t>()))),
        "`T`'s `unsafe_at(size_t)` member function is not noexcept "
        "while it should be. Hence, the error.");

    // clang-format off
    using View_impl =
        all_<
            Derives_from_view_base,
            Is_move_constructible,
            and_<
                Has_static_extent,
                Has_correct_return_type_extent,
                Is_noexcept_extent
            >,
            and_<
                Has_member_size,
                Has_correct_return_type_size,
                Is_noexcept_size
            >,
            Has_operator_access
        >;
    // clang-format on

    struct View : View_impl {

        using View_impl::test;

        template <class T>
        static constexpr auto check() noexcept -> bool
        {
            constexpr bool x = test<T>();
            static_assert(
                x, "I'm sorry, but `T` must model the View concept!");
            return View_impl::check<T>();
        }

      private:
        // clang-format off
        template <class T,
            class = std::enable_if_t<and_<Has_unsafe_at,
                Is_noexcept_unsafe_at>::test<T>()>,
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
            class = std::enable_if_t<!and_<Has_unsafe_at,
                Is_noexcept_unsafe_at>::test<T>()>>
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
        template <class T,
            class = std::enable_if_t<
                test<std::remove_cv_t<std::remove_reference_t<T>>>()>>
        static BOOST_STATIC_VIEWS_CONSTEXPR
        auto unsafe_at(T&& xs, std::size_t const i)
        BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
        (
            unsafe_at_impl(std::forward<T>(xs), i)
        );
        // clang-format on
    };
    // clang-format on

} // namespace view

using view::View;

} // namespace concepts

/// \brief
/// \verbatim embed:rst:leading-slashes
/// Base class to that helps with modeling the :ref:`View
/// <view-concept>` concept. \endverbatim
template <class Derived, class Wrapper>
struct view_adaptor_base : view_base {

  private:
    using derived_type = Derived;
    using wrapper_type = Wrapper;
    using view_type    = typename Wrapper::type;

    template <class Dummy>
    struct traits {
        template <class T>
        static constexpr auto has_map() noexcept -> bool
        {
            using namespace concepts;
            return and_<view::Has_member_map,
                view::Has_correct_return_type_map,
                view::Is_noexcept_map>::test<T>();
        }
    };

  protected:
    using view_adaptor_base_type =
        view_adaptor_base<Derived, Wrapper>;

  public:
    /// \name Constructors
    ///
    /// \brief Constructs an adaptor of \p view.
    /// \details Here, \p view is a wrapper around the actual view
    /// created with #make_wrapper(T&&) function.
    explicit BOOST_STATIC_VIEWS_CONSTEXPR view_adaptor_base(
        wrapper_type&& view)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<wrapper_type>::value)
        : _xs{std::move(view)}
    {
    }

    /// \name Copy and move constructors/assignments
    ///
    /// Defines default copy and move constructors and assignments,
    /// i.e. is copy/move-constructible/assignable if \p View is.
    /// \{
    BOOST_STATIC_VIEWS_CONSTEXPR
    view_adaptor_base(view_adaptor_base const&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_adaptor_base(view_adaptor_base&&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_adaptor_base& operator=(view_adaptor_base const&) = default;

    BOOST_STATIC_VIEWS_CONSTEXPR
    view_adaptor_base& operator=(view_adaptor_base&&) = default;
    /// \}

    /// \brief Returns the underlying view.
    /// \{
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() const & noexcept
    {
        static_assert(noexcept(_xs.get()),
            "[INTERNAL] view_adaptor_base<> assumes that "
            "`wrapper::get() const&` is noexcept.");
        return _xs.get();
    }

    // clang-format off
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<wrapper_type&&>().get()))
    {
        return std::move(_xs).get();
    }
    // clang-format on
    /// \}

    /// \brief
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``capacity`` function required
    /// by the :ref:`view concept <view-concept>`. \endverbatim

    /// Just calls `capacity()` on the underlying view.
    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return view_type::extent();
    }

    /// \brief
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``size`` function required by
    /// the :ref:`view concept <view-concept>`. \endverbatim

    /// Just calls `size()` on the underlying view.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
    {
        static_assert(noexcept(parent()),
            "[INTERNAL] view_adaptor_base<> assumes that its "
            "own `parent() const&` member function does not throw.");
        return parent().size();
    }

    /// \name Element access
    /// \{

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(std::size_t const i) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            concepts::View::unsafe_at(
                std::declval<view_type const&>(),
                std::declval<std::size_t>())))
    {
        static_assert(noexcept(parent()),
            "[INTERNAL] view_adaptor_base<> assumes that its "
            "own `parent() const&` member function does not throw.");
        auto const x = view_adaptor_core_access::map(derived(), i);
        return concepts::View::unsafe_at(parent(), x);
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(std::size_t const i) &
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            concepts::View::unsafe_at(
                std::declval<view_type &>(),
                std::declval<std::size_t>())))
    {
        static_assert(noexcept(parent()),
            "[INTERNAL] view_adaptor_base<> assumes that its "
            "own `parent() const&` member function does not throw.");
        auto const x = view_adaptor_core_access::map(derived(), i);
        return concepts::View::unsafe_at(parent(), x);
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(std::size_t const i) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            concepts::View::unsafe_at(
                std::declval<view_type &&>(),
                std::declval<std::size_t>())))
    {
        static_assert(noexcept(parent()),
            "[INTERNAL] view_adaptor_base<> assumes that its "
            "own `parent() const&` member function does not throw.");
        auto const x = view_adaptor_core_access::map(derived(), i);
        return concepts::View::unsafe_at(
            std::forward<view_adaptor_base>(*this).parent(), x);
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) const&
    {
        auto const x = view_adaptor_core_access::map(derived(), i);
        return (i < derived().size())
            ? parent()[x]
            : (make_out_of_bound_error("`i < size()` not satisfied."),
                   parent()[x]);
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) &
    {
        auto const x = view_adaptor_core_access::map(derived(), i);
        return (i < derived().size())
            ? parent()[x]
            : (make_out_of_bound_error("`i < size()` not satisfied."),
                   parent()[x]);
    }
    // clang-format on

    // clang-format off
    template <class Dummy = void,
        class = std::enable_if_t<traits<Dummy>::template
            has_map<derived_type>()>>
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) &&
    {
        auto const x = view_adaptor_core_access::map(derived(), i);
        return (i < derived().size())
            ? std::forward<view_adaptor_base>(*this).parent()[x]
            : (make_out_of_bound_error("`i < size()` not satisfied."),
                std::forward<view_adaptor_base>(*this).parent()[x]);
    }
    // clang-format on
    /// \}

  private:
    wrapper_type _xs;

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto derived() const noexcept
        -> derived_type const&
    {
        return *static_cast<derived_type const*>(this);
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_VIEW_BASE_HPP
