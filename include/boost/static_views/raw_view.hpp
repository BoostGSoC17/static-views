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
#include <boost/static_views/errors.hpp>
#include <boost/static_views/view_base.hpp>
#include <boost/static_views/sequence_traits.hpp>


BOOST_STATIC_VIEWS_BEGIN_NAMESPACE

namespace detail {
    template <class Sequence>
    struct raw_view_impl : view_base {

        /// \brief Constructs a view of sequece \p xs.

        /// \tparam Sequence Must model the sequence (TODO: link to concept
        /// definition) concept.
        /// \param xs Reference to the sequence.
        explicit BOOST_STATIC_VIEWS_CONSTEXPR raw_view_impl(Sequence& xs)
            noexcept
            : _xs{ xs }
        {
        }

        /// \name Copy and move constructors/assignments.
        /// 
        /// \{
        /// Default copy and move constructors and assignment operators. They
        /// are all \f$ \mathcal{O}(1) \f$.
        BOOST_STATIC_VIEWS_CONSTEXPR
        raw_view_impl(raw_view_impl const&) = default;

        BOOST_STATIC_VIEWS_CONSTEXPR
        raw_view_impl(raw_view_impl &&) = default;

        BOOST_STATIC_VIEWS_CONSTEXPR
        raw_view_impl& operator=(raw_view_impl const&) = default;

        BOOST_STATIC_VIEWS_CONSTEXPR
        raw_view_impl& operator=(raw_view_impl &&) = default;
        /// \}

        /// \brief Returns the size of the sequence it's viewing.

        /// \snippet raw_view.hpp raw_view_impl::capacity() implementation
        /// 
        /// This function is required by the View concept. It returns the
        /// maximum possible number of elements this view can have. In this
        /// case, just the number of elements in the sequence.
        ///
        /// \note This function is marked `static constexpr` and may thus be
        /// freely used in constant expressions.
        ///
        /// \sa size()
        static constexpr auto capacity() noexcept
        {
            //! [raw_view_impl::capacity() implementation]
            return sequence_traits<sequence_type>::size(); 
            //! [raw_view_impl::capacity() implementation]
        }

        /// \brief Returns the size of the sequence it's viewing.

        /// \snippet raw_view.hpp raw_view_impl::size() implementation
        /// This function is required by the View concept. It's behavior is
        /// equivalent to #capacity() function, i.e. it returns the number of
        /// elements in the sequence.
        ///
        /// \sa capacity()
        constexpr auto size() const noexcept
        { 
            //! [raw_view_impl::size() implementation]
            return sequence_traits<sequence_type>::size(); 
            //! [raw_view_impl::size() implementation]
        }


        /// \brief Element access

        /// \details This function is required by the View concept. It provides
        /// access to element at index \p i. Return type is determined by the
        /// corresponding specialisation of #sequence_traits. It may be a
        /// reference to element, or a value, or something else. Behavior of
        /// this function is well-described by the following code snippet:
        /// \code{.py}
        /// if i < sequence.size():
        ///     return sequence[i]
        /// else:
        ///     raise out_of_bound
        /// \endcode
        BOOST_FORCEINLINE
        BOOST_STATIC_VIEWS_CONSTEXPR
        BOOST_STATIC_VIEWS_DECLTYPE_AUTO operator[](std::size_t const i) const
        {
            return BOOST_LIKELY(i < size())
                ? sequence_traits<sequence_type>::at(_xs, i)
                : ( make_out_of_bound_error(
                        "Index `i` exceeds size of sequence."),
                    sequence_traits<sequence_type>::at(_xs, i) );
        }

/*
#if defined(DOXYGEN_IN_HOUSE)
        constexpr element_type& operator[](std::size_t const i)
#else
        BOOST_FORCEINLINE
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
        using sequence_type = std::remove_const_t<
                              std::remove_reference_t<
                                  Sequence >>;
        Sequence& _xs;
    };

    /// \cond
    struct make_raw_view {
        template <class Sequence>
        BOOST_STATIC_VIEWS_CONSTEXPR auto operator()(Sequence& sequence) const
        BOOST_STATIC_VIEWS_AUTO_RETURN_NOEXCEPT
        (
            raw_view_impl<Sequence>(sequence)
        )
    };
    /// \endcond
} // end namespace detail


/// \brief A functor for creating raw views of sequences.

/// `raw_view : Sequence --> View`
/// 
/// *Raw view* is the simplest view there is. It does no manipulations and is
/// just wrapper around a sequence type to implement the View concept. It should
/// work for any sequence for which there exists a specialisation of
/// #boost::static_views::sequence_traits.
#if defined(DOXYGEN_IN_HOUSE)
constexpr auto raw_view = [](auto& seq) { return detail::raw_view_impl{seq}; };
#else
BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::make_raw_view, raw_view)
#endif


BOOST_STATIC_VIEWS_END_NAMESPACE


#endif // BOOST_STATIC_VIEWS_RAW_VIEW_HPP
