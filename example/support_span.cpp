//          Copyright Tom Westerhout 2017.
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
        : sequence_traits_default<
              sequence_traits<gsl::span<T, Extent>>,
              gsl::span<T, Extent>> {

        static constexpr auto extent() noexcept -> std::ptrdiff_t
        {
            return gsl::span<T, Extent>::extent;
        }

        // Although sequence_traits_default can provides at()
        // when operator[] is available, span's operator[] is not
        // noexcept so it makes little sense.
        static constexpr auto at(gsl::span<T, Extent> const& xs,
            std::size_t const i) noexcept
        {
            // Notice the absence of bound checks here! raw_view
            // takes care of that.
            return xs.data()[i];
        }
    };
} // namespace static_views
} // namespace boost

int main()
{
    static constexpr int                     data[] = {1, 2, 3, 4, 5};
    static constexpr gsl::span<int const>    span_1 = {data};
    static constexpr gsl::span<int const, 5> span_2 = {data};

    // gsl::span<int const> is indeed a Sequence
    boost::static_views::concepts::Sequence::check<
        gsl::span<int const>>();

    // gsl::span<int const, 5> is indeed a StaticSequence
    boost::static_views::concepts::StaticSequence::check<
        gsl::span<int const, 5>>();

    // We can now create raw views of them
    constexpr auto xs_1 = boost::static_views::raw_view(span_1);
    constexpr auto xs_2 = boost::static_views::raw_view(span_2);
}
