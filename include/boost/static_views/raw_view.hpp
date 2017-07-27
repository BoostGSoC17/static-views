//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/// \file boost/static_views/raw_view.hpp
///
/// \brief Implementation of #boost::static_views::raw_view.

#ifndef BOOST_STATIC_VIEWS_RAW_VIEW_HPP
#define BOOST_STATIC_VIEWS_RAW_VIEW_HPP

#include <type_traits>
#include <boost/static_views/detail/config.hpp>
#include <boost/static_views/detail/utils.hpp>
#include <boost/static_views/errors.hpp>
#include <boost/static_views/sequence_traits.hpp>
#include <boost/static_views/view_base.hpp>

BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {

template <class Sequence,
    class = std::enable_if_t<concepts::Sequence::test<
        std::remove_cv_t<std::remove_reference_t<Sequence>>>()>>
struct raw_view_impl : view_base {

    /// \brief Constructs a view of \p xs.

    /// \tparam Sequence
    /// \verbatim embed:rst:leading-slashes
    /// Must model the :ref:`sequence <sequence-concept>` concept.
    /// \endverbatim
    /// \param xs Reference to the sequence.
    ///
    /// \verbatim embed:rst:leading-slashes
    /// .. note::
    ///   It's annoying to have to specify Sequence template parameter
    ///   all the time. For this reason a :cpp:var:`raw_view` factory
    ///   function is provided. Use it instead to construct raw views
    ///   of data.
    /// \endverbatim
    explicit BOOST_STATIC_VIEWS_CONSTEXPR raw_view_impl(
        Sequence& xs) noexcept
        : _xs{&xs}
    {
    }

    /// \brief Copy constructor.
    BOOST_STATIC_VIEWS_CONSTEXPR
    raw_view_impl(raw_view_impl const&) noexcept = default;

    /// \brief Move constructor.
    BOOST_STATIC_VIEWS_CONSTEXPR
    raw_view_impl(raw_view_impl&&) noexcept = default;

    /// \brief Copy assignment operator.
    BOOST_STATIC_VIEWS_CONSTEXPR
    raw_view_impl& operator=(raw_view_impl const&) noexcept = default;

    /// \brief Move assignment operator.
    BOOST_STATIC_VIEWS_CONSTEXPR
    raw_view_impl& operator=(raw_view_impl&&) noexcept = default;

    /// \brief Returns the size of the sequence.

    /// \snippet raw_view.hpp raw_view_impl::capacity() implementation
    ///
    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It returns the maximum possible number of elements
    /// this view can have. In this case, just the number of elements
    /// in the sequence.
    ///
    /// .. note::
    ///   This function is marked `static constexpr` and may thus be
    ///   freely used in constant expressions.
    /// \endverbatim
    static constexpr auto capacity() noexcept
    {
        //! [raw_view_impl::capacity() implementation]
        return sequence_traits<sequence_type>::size();
        //! [raw_view_impl::capacity() implementation]
    }

    /// \brief Returns the size of the sequence.

    /// \snippet raw_view.hpp raw_view_impl::size() implementation
    ///
    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It's behavior is equivalent to :cpp:func:`capacity()
    /// <detail::raw_view_impl::capacity()>` function, i.e. it returns
    /// the number of elements in the sequence.
    /// \endverbatim
    constexpr auto size() const noexcept
    {
        //! [raw_view_impl::size() implementation]
        return sequence_traits<sequence_type>::size();
        //! [raw_view_impl::size() implementation]
    }

    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO unsafe_at(
        std::size_t const i) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(
            noexcept(sequence_traits<sequence_type>::at(
                std::declval<Sequence&>(),
                std::declval<std::size_t>())))
    {
        return sequence_traits<sequence_type>::at(*_xs, i);
    }

    /// \brief Element access

    /// \verbatim embed:rst:leading-slashes
    /// This function is required by the :ref:`view <view-concept>`
    /// concept. It provides access to element at index `i`. Return
    /// type is determined by the corresponding specialisation of
    /// :cpp:class:`sequence_traits`. It may be a reference to
    /// element, or a value, or something else. Behavior of this
    /// function is well-described by the following code snippet:
    ///
    /// .. code-block:: python
    ///
    ///   if i < sequence.size():
    ///       return sequence[i]
    ///   else:
    ///       raise out_of_bound
    ///
    /// \endverbatim
    BOOST_STATIC_VIEWS_FORCEINLINE
    BOOST_STATIC_VIEWS_CONSTEXPR
    BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](
        std::size_t const i) const
    {
        return (i < size()) ? unsafe_at(i)
                            : (make_out_of_bound_error(
                                   "`i < size()` not satisfied."),
                                  unsafe_at(i));
    }

    /*
    #if defined(DOXYGEN_IN_HOUSE)
            constexpr element_type& operator[](std::size_t const i)
    #else
            BOOST_STATIC_VIEWS_FORCEINLINE
            constexpr decltype(auto) operator[](std::size_t const i)
    #endif
            {
                return BOOST_LIKELY(i < size())
                    ? sequence_traits<sequence_type>::at(_xs, i)
                    : ( make_out_of_bound_error( \
                            "Index `i` exceeds size of sequence."),
                        sequence_traits<sequence_type>::at(_xs, i) );
            }
    */

  private:
    using sequence_type =
        std::remove_const_t<std::remove_reference_t<Sequence>>;
    Sequence* _xs;
};

/// \cond
struct make_raw_view {

  private:
    BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_construction_noexcept, T,
        (noexcept(raw_view_impl<T>{std::declval<T&>()})), "");

  public:
    // clang-format off
    template <class Sequence>
    BOOST_STATIC_VIEWS_CONSTEXPR
    auto operator()(Sequence& sequence) const
        BOOST_STATIC_VIEWS_NOEXCEPT_IF(concepts::and_<
            concepts::Sequence, Is_construction_noexcept>
                ::test<std::remove_cv_t<Sequence>>())
    {
        concepts::Sequence::check<std::remove_cv_t<Sequence>>();
        return raw_view_impl<Sequence>{sequence};
    }
    // clang-format on
};
/// \endcond

} // namespace detail

/// \brief A functor for creating raw views of sequences.

/// \f[
/// \text{raw_view} : \text{Sequence} \to \text{View}
/// \f]
///
/// \verbatim embed:rst:leading-slashes
/// Here, ``Sequence`` is any type that models the :ref:`sequence
/// <sequence-concept>` concept. The exact type of view created is an
/// implementation detail. What's important is that it models the
/// :ref:`view <view-concept>` concept. \endverbatim
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_raw_view, raw_view)

BOOST_STATIC_VIEWS_END_NAMESPACE

#endif // BOOST_STATIC_VIEWS_RAW_VIEW_HPP
