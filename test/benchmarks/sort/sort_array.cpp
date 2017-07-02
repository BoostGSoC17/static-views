//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <utility>

#include <boost/config.hpp>

#include <boost/static_views/raw_view.hpp>
#include <boost/static_views/view_base.hpp>
#include <boost/static_views/algorithm_base.hpp>

namespace detail {
    template <class Less>
    constexpr
    auto bubble_sort_impl(std::size_t* xs, std::size_t size,
        Less&& less) noexcept
    {
        for (std::size_t i = size - 1; i > 0; --i) {
            for (std::size_t j = 0; j < i; ++j) {
                if (less(xs[j+1], xs[j])) {
                    auto const _temp = xs[j];
                    xs[j]            = xs[j+1];
                    xs[j+1]          = _temp;
                }
            }
        }
    }

    template <std::size_t... Is>
    struct sort_init_impl {
        std::size_t _is[sizeof...(Is)];

        template <class View, class Predicate>
        constexpr
        sort_init_impl(View const& xs, Predicate&& p) noexcept
            : _is{ Is... }
        {
            struct less {
                View      const&  _xs;
                Predicate const&  _p;

                constexpr
                auto operator()(std::size_t const i, 
                    std::size_t const j) -> bool
                {
                    return _p(_xs[i], _xs[j]); 
                }
            };
            bubble_sort_impl(_is, xs.size(), less{xs, p});
        }
    };

    template <class V, class P, std::size_t... Is>
    constexpr
    auto _make_sort_init_impl(V&& xs, P&& p, 
        std::index_sequence<Is...>) noexcept
    {
        return sort_init_impl<Is...>{std::forward<V>(xs), std::forward<P>(p)};
    }

    template <class V, class P>
    constexpr
    auto make_sort_init_impl(V&& xs, P&& p) noexcept
    {
        constexpr
        auto capacity = std::remove_reference_t<V>::capacity();
        return _make_sort_init_impl(std::forward<V>(xs), std::forward<P>(p),
            std::make_index_sequence<capacity>{});
    }

    template <class V>
    struct sort_impl : boost::static_views::view_adaptor_base<sort_impl<V>, V> {
        friend struct boost::static_views::view_adaptor_core_access;

        template <std::size_t... Is>
        constexpr
        sort_impl(V&& xs, sort_init_impl<Is...>&& init) noexcept
            : sort_impl::view_adaptor_base_type{ std::move(xs) }
            , _is{ init._is[Is]... }
        {
        }

    private:
        std::size_t _is[sort_impl::capacity()];

        constexpr
        auto map(std::size_t const i) const
        {
            if (i >= this->size()) {
                throw std::out_of_range{"sort_impl::map(): index `i` "
                    " is out of bounds."};
            }
            return _is[i];
        }
    };

    struct make_sort_impl {
        template <class View, class Predicate>
        constexpr
        auto operator()(View&& xs, Predicate&& p) const noexcept
        {
            auto init = make_sort_init_impl(xs.get(), 
                std::forward<Predicate>(p));
            return detail::sort_impl<View>(std::forward<View>(xs),
                std::move(init));
        }
    };
} // end namespace detail

BOOST_STATIC_VIEWS_INLINE_ALGO_VARIABLE(detail::make_sort_impl, sort)

static constexpr
int random_array[] = {
#   include "sort_array_data.txt"
};

int main()
{
    BOOST_ATTRIBUTE_UNUSED constexpr
    auto sorted_view = 
        sort(std::less<>{})(boost::static_views::raw_view(random_array));
    return 0;
}
