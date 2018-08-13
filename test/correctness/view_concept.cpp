//          Copyright Tom Westerhout 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "testing.hpp"

#include <utility>

#include <boost/static_views/view_concept.hpp>

struct private_tag {
};

struct A1 {
    constexpr A1(private_tag) {}

    A1(A1 const&) = delete;
    A1(A1&&)      = default;
    A1& operator=(A1 const&) = delete;
    A1& operator=(A1&&) = delete;

    using value_type      = char;
    using reference       = int;
    using const_reference = reference;
    using size_type       = unsigned long long;
    using index_type      = short;
    using difference_type = index_type;

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return 123;
    }
    auto size() const noexcept -> size_type;
    auto operator[](index_type) const -> const_reference;
    auto unsafe_at(index_type) const -> const_reference;
};

struct A2 {
    constexpr A2(private_tag) {}

    A2(A2 const&) = delete;
    A2(A2&&)      = default;
    A2& operator=(A2 const&) = delete;
    A2& operator=(A2&&) = delete;

    using value_type      = char;
    using reference       = int;
    using const_reference = reference;
    using size_type       = unsigned long long;
    using index_type      = short;
    using difference_type = index_type;

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return boost::static_views::dynamic_extent;
    }
    auto size() const /*no noexcept*/ -> size_type;
    auto operator[](index_type) const noexcept -> const_reference;
    auto unsafe_at(index_type) const noexcept -> const_reference;
};

struct A3 {
    constexpr A3(private_tag) {}

    A3(A3 const&) = delete;
    A3(A3&&)      = default;
    A3& operator=(A3 const&) = delete;
    A3& operator=(A3&&) = delete;

    using value_type      = char;
    using reference       = int;
    using const_reference = reference;
    using size_type       = unsigned long long;
    using index_type      = short;
    using difference_type = index_type;

    static constexpr auto extent() noexcept -> std::ptrdiff_t
    {
        return -123;
    }
    auto size() const noexcept -> size_type;
    auto operator[](index_type) const -> const_reference;
    auto operator[](size_type) -> reference;
    auto unsafe_at(index_type) const noexcept -> const_reference;
};

int main(void)
{
    using boost::static_views::is_detected;
    using boost::static_views::detected_t;
    STATIC_ASSERT(boost::static_views::HasExtent<A1>, "");
    STATIC_ASSERT(boost::static_views::HasSizeWithType<A1>, "");
    STATIC_ASSERT((boost::static_views::HasIndexOperator<A1&, A1::index_type>), "");
    STATIC_ASSERT(
        (std::is_same<detected_t<boost::static_views::detail::
                                     has_index_operator_t,
                          A1&, A1::index_type>,
            A1::reference>::value), "");
    STATIC_ASSERT(
        (std::is_same<detected_t<boost::static_views::detail::
                                     has_index_operator_t,
                          A1 const&, A1::index_type>,
            A1::const_reference>::value), "");
    STATIC_ASSERT((boost::static_views::HasIndexOperator<A1 const&, A1::index_type>), "");
    STATIC_ASSERT(boost::static_views::HasIndexOperatorWithType<A1>, "");
    STATIC_ASSERT(boost::static_views::HasUnsafeAtWithType<A1>, "");
    STATIC_ASSERT(boost::static_views::View<A1>,
        "A1 does not model the View concept.");
    STATIC_ASSERT(!boost::static_views::View<A2>,
        "A2 models the View concept but it really shouldn't.");
    STATIC_ASSERT(!boost::static_views::View<A3>,
        "A3 models the View concept but it really shouldn't.");
}
