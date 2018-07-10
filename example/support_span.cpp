//          Copyright Tom Westerhout 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <gsl/span>

#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/sequence_traits.hpp>

namespace boost {
namespace static_views {
    template <class T, std::ptrdiff_t Extent>
    struct sequence_traits<gsl::span<T, Extent>>
        // Default implementation for functions. Now we only need to
        // override stuff we aren't happy with
        : sequence_traits_default<sequence_traits<gsl::span<T, Extent>>,
              gsl::span<T, Extent>> {

      private:
        using base =
            sequence_traits_default<sequence_traits<gsl::span<T, Extent>>,
                gsl::span<T, Extent>>;

      public:
        using value_type = T;
        using reference  = std::add_lvalue_reference_t<value_type>;
        using typename base::size_type;

        static constexpr auto extent() noexcept -> std::ptrdiff_t
        {
            return Extent == gsl::dynamic_extent ? dynamic_extent : Extent;
        }

        template <class S>
        static constexpr auto size(S& xs) noexcept
        {
            return static_cast<size_type>(xs.size());
        }

        // Although sequence_traits_default provides an at() member function
        // when operator[] is available, span's operator[] is not
        // noexcept so it makes little sense.
        // Using a template here is a trick: this function can now be used for
        // const and/or volatile qualified versions of the sequence.
        template <class S>
        static constexpr decltype(auto) at(
            S& xs, typename base::index_type const i) noexcept
        {
            // Notice the absence of bound checks here! raw_view takes care of
            // that.
            return xs.data()[i];
        }
    };

    template <class T, std::ptrdiff_t Extent>
    struct sequence_traits<gsl::span<T, Extent> const>
        : sequence_traits<gsl::span<T, Extent>> {

        using value_type = T const;
        using reference  = std::add_lvalue_reference_t<value_type>;
    };

    template <class T, std::ptrdiff_t Extent>
    struct sequence_traits<gsl::span<T, Extent> volatile>
        : sequence_traits<gsl::span<T, Extent>> {

        using value_type = std::add_volatile_t<T>;
        using reference  = std::add_lvalue_reference_t<value_type>;
    };

    template <class T, std::ptrdiff_t Extent>
    struct sequence_traits<gsl::span<T, Extent> const volatile>
        : sequence_traits<gsl::span<T, Extent>> {

        using value_type = std::add_cv_t<T>;
        using reference  = std::add_lvalue_reference_t<value_type>;
    };
} // namespace static_views
} // namespace boost

int main()
{
    static constexpr int                     data[] = {1, 2, 3, 4, 5};
    static constexpr gsl::span<int const>    span_1 = {data};
    static constexpr gsl::span<int const, 5> span_2 = {data};

    // gsl::span<int const> is indeed a Sequence
    static_assert(boost::static_views::Sequence<gsl::span<int const>>, "");
    // gsl::span<int const, 5> is indeed a StaticSequence
    static_assert(boost::static_views::StaticSequence<gsl::span<int const, 5>>, "");

    // We can now create raw views of them
    BOOST_STATIC_VIEWS_UNUSED constexpr auto xs_1 = boost::static_views::raw_view(span_1);
    BOOST_STATIC_VIEWS_UNUSED constexpr auto xs_2 = boost::static_views::raw_view(span_2);

    // TODO: This currently does not work, because size() member function is not
    // marked volatile and I'm yet to find an elegant way to achieve it :(
    // static_assert(boost::static_views::Sequence<gsl::span<int const> volatile>, "");

    // This works though
    static_assert(boost::static_views::Sequence<gsl::span<int const> const>, "");
}
