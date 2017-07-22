//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/take.hpp
///
/// \brief Implementation of #boost::static_views::take.


#ifndef BOOST_STATIC_VIEWS_TAKE_HPP
#define BOOST_STATIC_VIEWS_TAKE_HPP


#include <type_traits>
#include <algorithm>

#include <boost/config.hpp>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/errors.hpp>
#include <boost/static_views/view_base.hpp>
#include <boost/static_views/algorithm_base.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
    template <class View>
    struct take_impl : view_adaptor_base<take_impl<View>, View> {

        /// \brief Constructs a view of \p xs consisting of at most \p e
        /// elements of \p xs.

        /// \tparam View
        /// \verbatim embed:rst:leading-slashes
        /// Wrapper around a view, i.e. ``typename View::type`` is a view and
        /// must model the :ref:`view <view-concept>` concept.
        /// \endverbatim
        /// \param xs Rvalue reference to a wrapper around a view.
        /// \param e  Number of elements to take. The resulting view will
        ///           contain \f$min(\text{xs.get}()\text{.size}(), \text{e})\f$
        ///           elements.
        /// 
        /// \verbatim embed:rst:leading-slashes
        /// .. note::
        ///   It's annoying to have to specify the View template parameter
        ///   all the time. For this reason a :cpp:var:`take` factory
        ///   function is provided. Use it instead to construct take views.
        /// \endverbatim
        explicit BOOST_STATIC_VIEWS_CONSTEXPR take_impl(View&& xs,
            std::size_t const n) 
            BOOST_STATIC_VIEWS_NOEXCEPT_IF(utils::all(
                std::is_nothrow_constructible<typename 
                    take_impl::view_adaptor_base_type, View&&>::value,
                // This is formally wrong, but come on, std::min(size_t, size_t)
                // _should_ be noexcept.
                // noexcept(std::min(std::declval<std::size_t>(),
                //    std::declval<std::size_t>())),
                take_impl::is_noexcept_parent_size()
            ))
            : take_impl::view_adaptor_base_type{ std::move(xs) }
            , _n{ std::min(this->parent().size(), n) }
        {
        }

        /// \brief Returns the number of elements viewed.

        /// \verbatim embed:rst:leading-slashes
        /// This function is required by the :ref:`view <view-concept>`
        /// concept. It never throws.
        /// \endverbatim
        BOOST_STATIC_VIEWS_CONSTEXPR auto size() const noexcept
        {
            return _n; 
        }

        /// \brief "Maps" index \p i to the corresponding index in the parent
        /// view.

        /// Let `xs` be of type #take_impl. The following relation then holds
        /// \f[
        /// \text{xs}[i] = \text{xs.parent}()[\text{xs.map}(i)]\;,
        ///     \forall i \in \{0, 1, \dots, \text{xs.size}()-1\}.
        /// \f]
        /// 
        /// If the condition `i < size()` is not satisfied, this function
        /// throws an #out_of_bound exception.
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR auto map(std::size_t const i) const
        {
            return i < size()
                ? i
                : ((void)make_out_of_bound_error(
                       "`i < size()` not satisfied."), i);
        }

    private:
        // friend struct BOOST_STATIC_VIEWS_NAMESPACE::view_adaptor_core_access;
        std::size_t _n;

        static constexpr auto is_noexcept_parent_size() noexcept
        {
            return noexcept(std::declval<take_impl const&>().parent().size());
        }
    };

    struct make_take_impl {
        template <class View>
        constexpr auto operator()(View&& xs, std::size_t const n) const 
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(noexcept(
            take_impl<std::decay_t<View>>{std::forward<View>(xs), n}
        ))
        {
            return take_impl<std::decay_t<View>>{std::forward<View>(xs), n};
        }
    };
} // end namespace detail


/// \brief A functor for creating "take views"

/// \f[
/// \text{take} : \mathbb{N} \to \text{View} \to \text{View}
/// \f]
///
/// \verbatim embed:rst:leading-slashes
/// Given a count ``n`` and a view ``xs``, creates a view of the first 
/// :math:`min(\text{n}, \text{xs.size}())` elements of ``xs``.
/// Type of ``xs`` may be anything as long as it models the :ref:`view
/// <view-concept>` concept. The exact type of the returned view is an
/// implementation detail. What's important is that it also models the
/// :ref:`view <view-concept>` concept.
/// 
/// .. note::
///   Haskell notation is used here, i.e. the function is curried and
///   :math:`\text{take}(n) : \text{View} \to \text{View}` models the
///   :ref:`algorithm <algorithm-concept>` concept.
///
/// \endverbatim
BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(detail::make_take_impl, take)


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_TAKE_HPP
