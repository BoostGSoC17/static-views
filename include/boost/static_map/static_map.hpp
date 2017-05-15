#include <functional>


/// \file static_map.hpp 
/// \detail
///
/// General
/// -------
///
/// *First layer* constructs a node to contain the data user wants
/// with the specified alignment, layout etc.
///
/// *Second layer* adds indices. Suppose we have N nodes. Let IS1 be an indexing
/// strategy. It adds two things. A `std::size_t orderIS1[N]` data member is
/// added to the container as a whole such that
/// `_underlying_C_array_[orderIS1[i]]` allows to obtain the i'th
/// element as seen by IS1. Each node gets an `std::size_t indexIS1` 
/// member so that `_underlying_C_array[orderIS1[i]].indexIS1 == i`.
/// The indexIS1 data member is required to be able to switch between
/// indexing strategies in O(1). Like in the example you gave about
/// files sorted by size. This way, we can add as many layers as we like.
///
/// The idea is that finally we have:
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// node _underlying_C_array_[N]
///     +---------+----+----+-----+
///     | data0   | i0 | i1 | ... |
///     +---------+----+----+-----+
///     | data1   | i0 | i1 | ... |
///     +---------+----+----+-----+
///     | data2   | i0 | i1 | ... |
///     +---------+----+----+-----+
///     |           ...           |
///     +---------+----+----+-----+
///     | dataN-1 | i0 | i1 | ... |
///     +---------+----+----+-----+
///
/// std::size_t _i0_order_[N]
///     +-------+-------+-----+
///     | index | index | ... |     Indexing Strategy 0
///     +-------+-------+-----+
/// std::size_t _i1_order_[N]
///     +-------+-------+-----+
///     | index | index | ... |     Indexing Strategy 1
///     +-------+-------+-----+
/// ...
/// 
/// std::size_t _i?_order_[N]
///     +-------+-------+-----+
///     | index | index | ... |
///     +-------+-------+-----+
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
/// And I think we can implement whatever we want with that.
/// 
///
/// Error handling
/// --------------
/// I don't think it makes much sense to specify error handling on a per 
/// function basis. Being able to chose the strategy at container creation
/// (as a template argument) should suffice.
///
/// This only applies to functions that would "want" to return references
/// to objects.
///
/// Here's a general layout of an error policy
/// ~~~~~~~~~~~~~~{cpp}
/// struct error_handling_policy {
///     using reference = /* what to return when a
///                          reference is "desired" */;
///     
///     // Called if a reference had
///     // been successfully calculated.
///     constexpr auto success(/*some type*/ &) -> reference;
///
///     // Called if an error had occured 
///     // during calculation. msg contains 
///     // the error message.
///     constexpr auto success(char const* msg) -> reference;
/// };
/// ~~~~~~~~~~~~~~
///
/// For now, I've provided two such policies: boost::use_exceptions that
/// throws upon errors, and \ref boost::use_nullptrs that returns 
/// pointers rather than references and `nullptr` upon errors.
///


namespace boost {

    template <class T, class E>
    struct use_exceptions {
        static_assert( std::is_same<std::remove_reference_t<T>, T>::value
                     , "T shouldn't have a reference qualifier." );
        using reference = T&;
        
        constexpr auto success(T& x) noexcept -> reference { return x; }
        constexpr auto error(char const* msg) -> reference { throw E{msg}; }
    };

    template <class T>
    struct use_nullptrs {
        static_assert( std::is_same<std::remove_reference_t<T>, T>::value
                     , "T shouldn't have a reference qualifier." );
        using reference = T*;
        
        constexpr auto success(T& x) noexcept -> reference { return &x; }
        constexpr auto error(char const*) -> reference { return nullptr; }
    };
}





/*
namespace boost {






namespace detail {




/// This is meant to be the base class for all the containers we might want
/// to implement (`static_map`, `static_set`, `static_multimap` etc.).
/// These containers'd publicly inherit from node_array to gain the basic
/// functionality.
///
/// As the name suggests the idea is to use a simple C-style array to hold
/// nodes. For `static_map` these nodes would be key-value pairs, for 
/// `static_set` -- just keys, etc. At first, it seems that by using an 
/// array of some type `Node` we move the burden of hashing, searching etc.
/// away from `node_array`. The trick here is to allow hash function passed
/// to `node_array` be different from the used defined one.
/// 
/// For example, consider a `static_map` container. User supplied hash function
/// knows how to hash `Key`s, not `Node`s. We can, however, create a simple
/// utility class
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{cpp}
/// template <class Hasher>
/// struct map_hasher_adaptor {
///     Hasher _hf;
///
///     template<class Key, class Value>
///     constexpr auto operator()(std::pair<Key const, Value> const& node)
///         -> std::size_t
///     { return _hf(node.first); }
/// };
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// which we then supply to `node_array`. Now `node_array` can hash nodes
/// knowing nothing about them. The same technique can be applied to comparison
/// function:
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{cpp}
/// template <class Pred>
/// struct map_equal_adaptor {
///     Pred _eq;
///
///     template<class Key, class Value>
///     constexpr auto operator()( Key const& key
///                              , std::pair<Key const, Value> const& node)
///         -> bool
///     { return _eq(key, node.first); }
/// };
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// We teach `node_array` how to compare keys to key-value pairs, because
/// users supply keys, not nodes.
/// 
template < class Node
         , std::size_t Size
         , class Pred
         , class Hash
         >
class node_array {

public:
    using value_type                      = Node;
    using reference                       = value_type &;
    using const_reference                 = value_type const&;
    using iterator                        = value_type *;
    using const_iterator                  = value_type const*;
    using size_type                       = std::size_t;
    using difference_type                 = std::ptrdiff_t;
    using hasher                          = Hash;

private:
    using node_equal                      = Pred; 
    template<bool... Bs> struct _all;

    value_type   _nodes[Size];
    node_equal   _eq;
    hasher       _hf;

public:
    template <class... T>
    constexpr node_array(T&&...)
        noexcept(_all<std::is_nothrow_constructible<value_type, 
            T>::value ...>::value);

	constexpr node_array(const node_array &)
        noexcept( std::is_nothrow_copy_constructible<value_type>::value
               && std::is_nothrow_copy_constructible<node_equal>::value
               && std::is_nothrow_copy_constructible<hasher>::value );

    constexpr node_array(node_array &&)
        noexcept( std::is_nothrow_move_constructible<value_type>::value
               && std::is_nothrow_move_constructible<node_equal>::value
               && std::is_nothrow_move_constructible<hasher>::value );

	constexpr auto   begin()       noexcept -> iterator;
	constexpr auto   end()         noexcept -> iterator;
	constexpr auto  cbegin() const noexcept -> const_iterator;
	constexpr auto  cend()   const noexcept -> const_iterator;

protected:
    template <class Key>
    constexpr auto _find(const Key &) 
    // and maybe some more specifications
        noexcept( noexcept(std::declval<hasher>()(std::declval<Key>())) 
               && noexcept(std::declval<node_equal>()(std::declval<Key>(),
                      std::declval<value_type>())) )
        -> iterator;

    template <class Key>
    constexpr auto _find(const Key &) const 
    // and maybe some more specifications
        noexcept( noexcept(std::declval<hasher>()(std::declval<Key>())) 
               && noexcept(std::declval<node_equal>()(std::declval<Key>(),
                      std::declval<value_type>())) )
        -> const_iterator;
};



} // end namespace detail



namespace detail {

    template <class Hasher>
    struct map_hasher_adaptor {
    private:
        Hasher _hf;
    public:
        constexpr map_hasher_adaptor(Hasher const&)
            noexcept(std::is_nothrow_copy_constructible<Hasher>::value);
        constexpr map_hasher_adaptor(Hasher &&)
            noexcept(std::is_nothrow_move_constructible<Hasher>::value);

        template <class Key, class Value>
        constexpr auto operator()(std::pair<Key const, Value> const&) 
            noexcept(noexcept(std::declval<Hasher>()(std::declval<Key>())))
            -> std::size_t;
    };

    template <class Pred>
    struct map_equal_adaptor {
    private:
        Pred _eq;
    public:
        constexpr map_equal_adaptor(Pred const&)
            noexcept(std::is_nothrow_copy_constructible<Pred>::value);
        constexpr map_equal_adaptor(Pred &&)
            noexcept(std::is_nothrow_move_constructible<Pred>::value);

        template <class Key, class Value>
        constexpr auto operator()( Key const&
                                 , std::pair<Key const, Value> const&) 
            noexcept(noexcept(std::declval<Pred>()( std::declval<Key>()
                                                  , std::declval<Key>() )))
            -> bool;
    };
}


template < class Key
         , class Type
         , std::size_t Size
         , class Pred
         , class Hash
         >
class static_map
    : public detail::node_array< std::pair<Key const, Type>
                               , Size
                               , detail::map_equal_adaptor<Pred>
                               , detail::map_hasher_adaptor<Hash>
                               > {

private:
    using _parent_type = detail::node_array< std::pair<Key const, Type>, Size,
        detail::map_equal_adaptor<Pred>, detail::map_hasher_adaptor<Hash> >;
    using _this_type = static_map<Key, Type, Size, Pred, Hash>;

public:
    using key_type                   = Key;
    using mapped_type                = Type;
    using key_equal                  = Pred;
    using hasher                     = Hash;
    using value_type                 = typename _parent_type::value_type;
    using reference                  = typename _parent_type::reference;
    using const_reference            = typename _parent_type::const_reference;
    using iterator                   = typename _parent_type::iterator;
    using const_iterator             = typename _parent_type::const_iterator;
    using size_type                  = typename _parent_type::size_type;
    using difference_type            = typename _parent_type::difference_type;

public:

    constexpr static_map(value_type const (&)[Size])
        noexcept(std::is_nothrow_copy_constructible<value_type>::value);

	constexpr static_map(static_map const&)
        noexcept(std::is_nothrow_copy_constructible<_parent_type>::value);
	constexpr static_map(static_map &&)
        noexcept(std::is_nothrow_move_constructible<_parent_type>::value);

    constexpr auto hash_function()
        noexcept(std::is_nothrow_copy_constructible<hasher>::value) 
        -> hasher;
    constexpr auto key_eq()
        noexcept(std::is_nothrow_copy_constructible<key_equal>::value) 
        -> key_equal;

    constexpr auto size() const noexcept -> size_type;

	constexpr auto   begin()       noexcept -> iterator;
	constexpr auto   end()         noexcept -> iterator;
	constexpr auto  cbegin() const noexcept -> const_iterator;
	constexpr auto  cend()   const noexcept -> const_iterator;

    constexpr auto find(key_type const&) const 
        noexcept(noexcept(std::declval<_parent_type const>()._find( 
            std::declval<key_type>())))
        -> const_iterator;
    constexpr auto find(key_type const&)       
        noexcept(noexcept(std::declval<_parent_type const>()._find( 
            std::declval<key_type>())))
        -> iterator;
    constexpr auto count(key_type const&) const 
        noexcept(noexcept(std::declval<static_map>().find( 
            std::declval<key_type>())))
        -> size_type;

    constexpr auto operator[](key_type const&) const -> mapped_type const&;
    constexpr auto operator[](key_type const&)       -> mapped_type &;
    constexpr auto at(key_type const&) const -> mapped_type const&;
    constexpr auto at(key_type const&)       -> mapped_type &;
};


} // end namespace boost

*/
