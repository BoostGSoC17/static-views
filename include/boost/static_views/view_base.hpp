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

#include "concepts.hpp"
#include "detail/config.hpp"
#include "detail/utils.hpp"
#include "detail/wrapper.hpp"
#include "errors.hpp"
#include "iterator.hpp"
#include <type_traits>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

#if 0
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
#endif

// clang-format off
template <class T
    BOOST_STATIC_VIEWS_REQUIRES(View<T>)
BOOST_STATIC_VIEWS_CONSTEXPR auto begin(T const& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    view_iterator<T const>{&xs, 0}
);

template <class T
    BOOST_STATIC_VIEWS_REQUIRES(View<T>)
BOOST_STATIC_VIEWS_CONSTEXPR auto begin(T& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    view_iterator<T>{&xs, 0}
);

template <class T
    BOOST_STATIC_VIEWS_REQUIRES(View<T>)
BOOST_STATIC_VIEWS_CONSTEXPR auto end(T const& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    view_iterator<T const>{&xs, xs.size()}
);

template <class T
    BOOST_STATIC_VIEWS_REQUIRES(View<T>)
BOOST_STATIC_VIEWS_CONSTEXPR auto end(T& xs)
BOOST_STATIC_VIEWS_DECLTYPE_NOEXCEPT_RETURN
(
    view_iterator<T>{&xs, xs.size()}
);
// clang-format on

/// \brief
/// \verbatim embed:rst:leading-slashes
/// Base class to that helps with modeling the :ref:`View
/// <view-concept>` concept. \endverbatim
template <class Derived, class Wrapper>
struct view_adaptor_base : private Wrapper {

  private:
    using derived_type = Derived;
    using wrapper_type = Wrapper;
    using view_type    = typename Wrapper::value_type;


    template <class Dummy>
    struct traits {
        using derived = derived_type;
    };

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR auto derived() const noexcept
        -> derived_type const&
    {
        return *static_cast<derived_type const*>(this);
    }

  protected:
    using view_adaptor_base_type =
        view_adaptor_base<Derived, Wrapper>;

  public:
    using value_type      = typename view_type::value_type;
    using reference       = typename view_type::reference;
    using size_type       = typename view_type::size_type;
    using index_type      = typename view_type::index_type;

    /// \name Constructors
    ///
    /// \brief Constructs an adaptor of \p view.
    /// \details Here, \p view is a wrapper around the actual view
    /// created with #make_wrapper(T&&) function.
    BOOST_STATIC_VIEWS_CONSTEXPR
    view_adaptor_base(wrapper_type&& view)
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            std::is_nothrow_move_constructible<wrapper_type>::value)
        : wrapper_type{std::move(view)}
    {
    }

    /// \name Copy and move constructors/assignments
    ///
    /// Defines default copy and move constructors and assignments,
    /// i.e. is copy/move-constructible/assignable if \p View is.
    /// \{
    view_adaptor_base(view_adaptor_base const&) = default;
    view_adaptor_base(view_adaptor_base&&)      = default;
    view_adaptor_base& operator=(view_adaptor_base const&) = default;
    view_adaptor_base& operator=(view_adaptor_base&&) = default;
    /// \}

    /// \brief Returns the underlying view.
    /// \{
    BOOST_STATIC_VIEWS_CONSTEXPR auto const& parent() const& noexcept
    {
        static_assert(
            noexcept(this->get()), BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return this->get();
    }

    BOOST_STATIC_VIEWS_CONSTEXPR auto parent()
        && BOOST_STATIC_VIEWS_NOEXCEPT_IF(
               std::is_nothrow_move_constructible<
                   wrapper_type>::value)
    {
        return static_cast<wrapper_type&&>(*this).get();
    }
    /// \}

    /// \brief
    /// \verbatim embed:rst:leading-slashes
    /// Default implementation of the ``capacity`` function required
    /// by the :ref:`view concept <view-concept>`. \endverbatim

    static constexpr auto extent() noexcept
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
        static_assert(
            noexcept(parent()), BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return parent().size();
    }

    /// \name Element access
    /// \{

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(HasMap<typename traits<Dummy>::derived>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(index_type const i) const&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_type const&>().unsafe_at(
                std::declval<derived_type const&>().map(i))))
    // clang-format on
    {
        static_assert(noexcept(parent()) && noexcept(derived()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return parent().unsafe_at(derived().map(i));
    }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(HasMap<typename traits<Dummy>::derived>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(index_type const i) &&
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            std::declval<view_type&&>().unsafe_at(
                std::declval<derived_type const&>().map(i))))
    // clang-format on
    {
        static_assert(noexcept(parent()) && noexcept(derived()),
            BOOST_STATIC_VIEWS_BUG_MESSAGE);
        return static_cast<wrapper_type&&>(*this).get().unsafe_at(
            derived().map(i));
    }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(HasMap<typename traits<Dummy>::derived>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](index_type const i) const&
    // clang-format on
    {
        if (BOOST_STATIC_VIEWS_UNLIKELY(
                i >= static_cast<index_type>(derived().size()))) {
            make_out_of_bound_error(
                "Precondition `i < size()` not satisfied in "
                "boost::static_views::view_base::operator[].");
            BOOST_STATIC_VIEWS_UNREACHABLE;
        }
        return parent()[derived().map(i)];
    }

    // clang-format off
    template <class Dummy = void
        BOOST_STATIC_VIEWS_REQUIRES(HasMap<typename traits<Dummy>::derived>)
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](index_type const i) &&
    // clang-format on
    {
        if (BOOST_STATIC_VIEWS_UNLIKELY(
                i >= static_cast<index_type>(derived().size()))) {
            make_out_of_bound_error(
                "Precondition `i < size()` not satisfied in "
                "boost::static_views::view_base::operator[].");
            BOOST_STATIC_VIEWS_UNREACHABLE;
        }
        return static_cast<wrapper_type&&>(*this)
            .get()[derived().map(i)];
    }
    /// \}

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto begin() const BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        noexcept(::BOOST_STATIC_VIEWS_NAMESPACE::begin(
            std::declval<derived_type const&>())))
    {
        return ::BOOST_STATIC_VIEWS_NAMESPACE::begin(derived());
    }

    BOOST_STATIC_VIEWS_CONSTEXPR
    auto end() const BOOST_STATIC_VIEWS_NOEXCEPT_IF(
        noexcept(::BOOST_STATIC_VIEWS_NAMESPACE::end(
            std::declval<derived_type const&>())))
    {
        return ::BOOST_STATIC_VIEWS_NAMESPACE::end(derived());
    }
};

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_VIEW_BASE_HPP
