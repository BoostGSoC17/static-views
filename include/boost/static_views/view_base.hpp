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

#include <type_traits>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/detail/wrapper.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

/// \brief Base class for all the views.

/// \verbatim embed:rst:leading-slashes
/// By deriving from :cpp:class:`view_base`, you tell StaticView that
/// the derived class models the :ref:`view <view-concept>` concept.
/// \endverbatim
struct view_base {
};


template <class Derived, class View>
struct view_adaptor_base;

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

  private:
    template <class View>
    static BOOST_STATIC_VIEWS_CONSTEXPR auto map(
        View const& xs, std::size_t const i)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(xs.map(i)))
    {
        return xs.map(i);
    }
};

namespace detail {
namespace concepts {
namespace view {
    template <class T>
    using static_capacity_t = decltype(T::capacity());

    template <class T>
    using member_size_t = decltype(std::declval<T const&>().size());

    template <class T>
    using operator_access_t =
        decltype(std::declval<T>()[std::declval<std::size_t>()]);

    template <class T>
    constexpr auto check_copy_or_move() noexcept -> bool
    {
        return std::is_move_constructible<std::remove_cv_t<T>>::value
            || std::is_copy_constructible<std::remove_cv_t<T>>::value;
    }

    template <class T>
    constexpr auto assert_copy_or_move() noexcept -> bool
    {
        constexpr bool x = check_copy_or_move<T>();
        static_assert(x,
            "`T` is neither move nor copy constructible. "
            "It is, however, required by the View concept that "
            "`T` be either copy or move constructible");
        return x;
    }

    template <class T>
    constexpr auto check_derive() noexcept -> bool
    {
        return std::is_base_of<view_base, std::remove_cv_t<T>>::value;
    }

    template <class T>
    constexpr auto assert_derive() noexcept -> bool
    {
        constexpr bool x = check_derive<T>();
        static_assert(x,
            "It is required by the View concept that `T` derive "
            "from boost::static_views::view_base.");
        return x;
    }

    template <class T>
    constexpr auto check_static_capacity() noexcept -> bool
    {
        return std::is_convertible<detected_t<static_capacity_t, T>,
            std::size_t>::value;
    }

    template <class T>
    constexpr auto assert_static_capacity() noexcept -> bool
    {
        constexpr bool x = check_static_capacity<T>();
        static_assert(x,
            "It is required by the View concept for `T` to have a "
            "static constexpr non-throwing member function "
            "`capacity()` with return type convertible to size_t.");
        return x;
    }

    template <class T>
    constexpr auto check_member_size() noexcept -> bool
    {
        return std::is_convertible<detected_t<member_size_t, T>,
            std::size_t>::value;
    }

    template <class T>
    constexpr auto assert_member_size() noexcept -> bool
    {
        constexpr bool x = check_member_size<T>();
        static_assert(x,
            "It is required by the View concept for `T` to have a "
            "constexpr non-throwing member function `size() const` "
            "with return type convertible to size_t.");
        return x;
    }

    template <class T>
    constexpr auto check_operator_access() noexcept -> bool
    {
        return is_detected<operator_access_t, T>::value;
    }

    template <class T>
    constexpr auto assert_operator_access() noexcept -> bool
    {
        constexpr bool x = check_operator_access<T>();
        static_assert(x,
            "It is required by the View concept for `T` to have a "
            "constexpr `operator[](size_t)` taking an index as "
            "argument.");
        return x;
    }
} // namespace view

template <class T>
constexpr auto is_View() noexcept -> bool
{
    constexpr bool x = check(&view::check_derive<T>,
        &view::check_copy_or_move<T>,
        &view::check_static_capacity<T>,
        &view::check_member_size<T>,
        &view::check_operator_access<T>);
    return x;
}

template <class T>
constexpr auto assert_View() noexcept -> bool
{
    constexpr bool x = check(&view::assert_derive<T>,
        &view::assert_copy_or_move<T>,
        &view::assert_static_capacity<T>,
        &view::assert_member_size<T>,
        &view::assert_operator_access<T>);
    return x;
}

} // namespace concepts
} // namespace detail

/// \brief
/// \verbatim embed:rst:leading-slashes
/// Base class to that helps with modeling the :ref:`View
/// <view-concept>` concept. \endverbatim
template <class Derived, class View>
struct view_adaptor_base : view_base {

    // Make sure Derived is not some strange reference or something.
    static_assert(std::is_same<Derived, std::decay_t<Derived>>::value,
        "`boost::static_views::view_adaptor_base<Derived, View>` "
        "requires the `Derived` template argument to be an "
        "unqualified type.");
    // Make sure View is not some strange reference or something.
    static_assert(std::is_same<View, std::decay_t<View>>::value,
        "`boost::static_views::view_adaptor_base<Derived, View>` "
        "requires the `View` template argument to be an "
        "unqualified type.");
    // Make sure View is actually a wrapper.
    static_assert(detail::is_wrapper<View>::value,
        "`boost::static_views::view_adaptor_base<Derived, View>` "
        "requires the `View` template argument to be a wrapper "
        "type (i.e. `View = wrapper<T>` for some type `T`).");
    // View is stored as a member variable, so we need at least
    // move-constructor
    static_assert(detail::concepts::is_View<
        typename View::type>(),
        "`boost::static_views::view_adaptor_base<Derived, View>`, "
        "mind the __View__! In other words, `View::type` must "
        "model the View concept.");

  private:
    using derived_type = Derived;
    using wrapper_type = View;
    using view_type    = typename View::type;

  protected:
    using view_adaptor_base_type = view_adaptor_base<Derived, View>;

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

    /// \brief
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``capacity`` function required
    /// by the :ref:`view concept <view-concept>`. \endverbatim

    /// Just calls `capacity()` on the underlying view.
    static constexpr auto capacity() noexcept
    {
        static_assert(noexcept(view_type::capacity()),
            "view_adaptor_base<> assumes that view_type::capacity() "
            "never throws.");
        return view_type::capacity();
    }

    /// \brief
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``size`` function required by
    /// the :ref:`view concept <view-concept>`. \endverbatim

    /// Just calls `size()` on the underlying view.
    BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
    {
        static_assert(noexcept(this->parent()),
            "view_adaptor_base<> assumes that its own parent() "
            "member function does not throw.");
        static_assert(
            noexcept(std::declval<decltype(this->parent())>().size()),
            "view_adaptor_base<> assumes that its parent has a "
            "non-throwing size() member function.");
        return parent().size();
    }

    /// \brief Returns the underlying view.
    /// \{
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO parent() const & noexcept
    {
        static_assert(noexcept(_xs.get()), "view_adaptor_base<> "
                                           "assumes that "
                                           "`wrapper::get() const&` "
                                           "is noexcept.");
        return _xs.get();
    }

    /*
    #if defined(DOXYGEN_IN_HOUSE)
        BOOST_STATIC_VIEWS_CONSTEXPR underlying_view_type& parent() &
    #else
        BOOST_STATIC_VIEWS_CONSTEXPR decltype(auto) parent() &
    #endif
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
                std::declval<wrapper_type &>().get()
            ))
        {
            return _xs.get();
        }
    */

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

    /// \name Element access
    /// \{
    // clang-format off
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<view_adaptor_base const&>()
                         .parent()[view_adaptor_core_access::map(
                             std::declval<derived_type const&>(),
                             std::declval<std::size_t>())]))
    {
        return parent()[view_adaptor_core_access::map(derived(), i)];
    }
    // clang-format on

    // clang-format off
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) &
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<view_adaptor_base&>()
                         .parent()[view_adaptor_core_access::map(
                             std::declval<derived_type const&>(),
                             std::declval<std::size_t>())]))
    {
        return parent()[view_adaptor_core_access::map(derived(), i)];
    }
    // clang-format on

    // clang-format off
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(std::declval<view_adaptor_base&&>()
                         .parent()[view_adaptor_core_access::map(
                             std::declval<derived_type const&>(),
                             std::declval<std::size_t>())]))
    {
        return std::forward<view_adaptor_base>(*this).parent()[
            view_adaptor_core_access::map(derived(), i)];
    }
    // clang-format on
    /// \}

  private:
    View _xs;

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto derived() const noexcept
        -> derived_type const&
    {
        return *static_cast<derived_type const*>(this);
    }
};


namespace detail {

struct dummy_view : view_base {

    static constexpr int x = 12345;

    constexpr dummy_view() noexcept
    {
    }

    constexpr dummy_view(dummy_view const&) noexcept
    {
    }

    constexpr dummy_view(dummy_view &&) noexcept
    {
    }

    constexpr dummy_view& operator=(dummy_view const&) noexcept
    {
        return *this;
    }

    constexpr dummy_view& operator=(dummy_view &&) noexcept
    {
        return *this;
    }

    static constexpr auto capacity() noexcept -> std::size_t
    {
        return 1;
    }

    constexpr auto size() const noexcept -> std::size_t
    {
        return 1;
    }

    constexpr auto operator[](std::size_t const) -> int const&
    {
        return x;
    }

    constexpr auto operator[](std::size_t const) const -> int const&
    {
        return x;
    }
};

constexpr int dummy_view::x;

} // namespace detail

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_VIEW_BASE_HPP
