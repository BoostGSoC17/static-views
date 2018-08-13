
<boost/static_views/static_map.hpp>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Synopsis:

.. code-block:: cpp

   namespace boost {
   namespace static_views {

     template </*implementation detail*/>
     class static_map {
       public:
         using value_type    = /*implementation detail*/;
         using reference     = /*implementation detail*/;
         using key_type      = /*implementation detail*/;
         using mapped_type   = /*implementation detail*/;
         using size_type     = /*implementation detail*/;
         using index_type    = /*implementation detail*/;
         using key_equal     = /*implementation detail*/;
         using key_hasher    = /*implementation detail*/;
         using key_getter    = /*implementation detail*/;
         using mapped_getter = /*implementation detail*/;
         using bucket_type   = /*implementation detail*/;

         constexpr static_map(/*implementation detail*/)
             noexcept(whenever possible);

         static_map(static_map const&) = default;
         static_map(static_map&&)      = default;
         static_map& operator=(static_map const&) = default;
         static_map& operator=(static_map&&) = default;

         constexpr auto find(key_type const&) const
             noexcept(whenever possible) -> value_type*;
         constexpr auto count(key_type const&) const
             noexcept(whenever possible) -> size_type;

         constexpr decltype(auto) at(key_type const&) const;
         constexpr decltype(auto) operator[](key_type const&) const;

         constexpr auto size() const noexcept -> size_type;
         static constexpr auto bucket_count() noexcept -> size_type;
         static constexpr auto bucket_size() noexcept -> size_type;

         constexpr auto begin() const noexcept;
         constexpr auto end() const noexcept;

         constexpr auto key_equal_fn() const noexcept;
     };


     template <std::size_t BucketCount, std::size_t BucketSize,
       class View, class GetKey, class GetMapped, class KeyEqual, class Hasher>
     auto make_static_map(View&& xs, GetKey get_key, GetMapped&& get_mapped,
       KeyEqual&& key_equal = KeyEqual{}, Hasher&& hasher = Hasher{});

     
     struct key_not_found_error : public virtual std::exception {
       /*implementation detail*/ 
     };
 
   } // namespace static_views
   } // namespace boost


.. cpp:namespace:: boost::static_views

.. cpp:class:: static_map

   A ``constexpr`` hash table.

   .. cpp:type:: value_type

      Type of elements in the view from which the map was created.

   .. cpp:type:: reference

      Reference to a element in the underlying view.

   .. cpp:type:: key_type

      Type of keys in the map.

   .. cpp:type:: mapped_type

      Type of values in the map.
    
   .. cpp:type:: size_type

      Unsigned integral type that represents size.

   .. cpp:type:: index_type

      Integral type that represents indices.

   .. cpp:type:: key_equal

      :cpp:type:`key_type`:math:`\rightarrow`:cpp:type:`key_type`:math:`\rightarrow`:cpp:expr:`bool`,
      i.e. type of the function that is used to compare keys for equality.

   .. cpp:type:: key_hasher

      A function
      :cpp:type:`key_type`:math:`\rightarrow`:cpp:type:`std::size_t` that
      is used for hashing.

   .. cpp:type:: key_getter

      A function used to extract a :cpp:type:`key_type` from a
      :cpp:type:`value_type`.

   .. cpp:type:: mapped_getter

      A function used to extract a :cpp:type:`mapped_type` from a
      :cpp:type:`value_type`.

   .. cpp:type:: bucket_type

      Type of a single bucket.

   .. cpp:function:: constexpr auto key_equal_fn() const noexcept

      Returns a function that is used to compare keys for equality.

   .. cpp:function:: constexpr value_type* find(key_type const& k) const

      Looks-up the value corresponding to the given key.

      If a key ``x`` is found in the map such that
      :cpp:expr:`invoke(key_equal_fn(), x, k) == true` then a pointer to
      the element is returned, otherwise -- a :cpp:expr:`nullptr`.

      This function is ``noexcept`` if :cpp:type:`key_hasher`,
      :cpp:type:`key_equal`, and :cpp:type:`key_getter` do not throw.

   .. cpp:function:: constexpr size_type count(key_type const& k) const

      Returns the number of times the given key occurs in the map. This
      function returns either zero or one.

      This function is ``noexcept`` if :cpp:type:`key_hasher`,
      :cpp:type:`key_equal`, and :cpp:type:`key_getter` do not throw.

   .. cpp:function:: constexpr decltype(auto) at(key_type const& k) const

      Looks-up the value corresponding to the given key.

      If a key ``x`` is found in the map such that
      :cpp:expr:`invoke(key_equal_fn(), x, k) == true` then a reference to
      the corresponding value is returned. Otherwise, an exception of type
      :cpp:class:`key_not_found_error` is thrown.

   .. cpp:function:: constexpr decltype(auto) operator[](key_type const& k) const

      Looks-up the value corresponding to the given key.

      .. note:: The behaviour of this function is identical to that of
                :cpp:func:`at`, i.e. no new elements are inserted into the
                map.

   .. cpp:function:: constexpr size_type size() const noexcept

      Returns the number of elements in the map.

   .. cpp:function:: static constexpr size_type bucket_count() noexcept

      Returns the number of buckets in the map.

   .. cpp:function:: static constexpr size_type bucket_size() noexcept

      Returns the capacity of a single bucket.


.. cpp:class:: key_not_found_error

   Exception type thrown when a key could not be found. See e.g.
   :cpp:func:`static_map::at` and :cpp:func:`static_map::operator[]`
   functions.

