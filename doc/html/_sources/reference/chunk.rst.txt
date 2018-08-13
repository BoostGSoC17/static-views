<boost/static_views/chunk.hpp>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Synopsis:

.. code-block:: cpp

   namespace boost {
   namespace static_views {

     // Exposition only
     template <class V>
       requires (View<remove_cvref<V>>)
     constexpr auto make_chunk_view(V&&, typename V::index_type)
                                             noexcept(whenever possible);

     // Exposition only
     template <class V, class Int, Int N>
       requires (View<remove_cvref<V>>)
     constexpr auto make_chunk_view(V&&, std::integral_constant<Int, N>)
                                             noexcept(whenever possible);

     // Exposition only
     template <class Int, Int N>
     constexpr auto make_chunk_view(std::integral_constant<Int, N>) noexcept;

     // Exposition only
     template <class Int>
       requires (Integral<Int>)
     constexpr auto make_chunk_view(Int) noexcept;


     constexpr auto chunk = [] (auto&&... args) {
       return make_chunk_view(std::forward<decltype(args)>(args)...);
     };

   } // namespace static_views
   } // namespace boost

.. cpp:namespace:: boost::static_views

.. cpp:function:: template <class V> \
                  constexpr auto make_chunk_view(V&& xs, typename V::index_type n)

   .. note:: This function is not real and is provided here for exposition
             pusposes only.

   Creates a view of ``xs`` where each element is a chunk of ``n`` elements.


   :param xs: The view to adapt. It must model the :ref:`view-concept` concept.

     * If ``xs`` is an lvalue reference, then the returned view only
       stores a pointer to ``xs``.
     * If ``xs`` is an rvalue reference, then the ownership of ``xs`` is
       transformed to the returned view using ``xs``' move constructor.
   :param n: The number of elements in each chunk.
     
     .. warning:: ``xs.size()`` must be divisible by ``n``!

   :return: Chunk view of ``xs``.


.. cpp:function:: template <class V, class Int, Int N> \
                  constexpr auto make_chunk_view(V&& xs, std::integral_constan<Int, N> n)

   .. note:: This function is not real and is provided here for exposition
             pusposes only.

   Creates a view of ``xs`` where each element is a chunk of ``n``
   elements. It is very similar to the above function except that number
   of elements in a chunk is now a compile-time rather than a run-time
   constant.


.. cpp:function:: template <class Int> \
                  constexpr auto make_chunk_view(Int n)

   .. note:: This function is not real and is provided here for exposition
             pusposes only.

   This is a curried version. Its main purpose is to be used in pipes.

   :param n: The number of elements in each chunk.
   :return:  The function :cpp:expr:`[n] (auto xs) { make_chunk_view(xs,
             n); };`. The lambda here is shown for exposition only; the
             actual implementation is more efficient.


.. cpp:var:: constexpr auto chunk = [] (auto&&... args) { \
                 return make_chunk_view(std::forward<decltype(args)>(args)...); \
             };

   A wrapper function with the following signatures:
  
     * ``View₁, ℕ -> View₂``  [#f1]_
     * ``ℕ -> (View₁ -> View₂)`` [#f2]_

   where ``ℕ`` is either ``View₁::index_type`` or a ``std::integral_constant``.

   Example usage:
   
   .. literalinclude:: ../../example/using_chunk_view.cpp
      :language: cpp
     

.. [#f1] I.e. given a view (``View₁``) and a count (``ℕ``), returns a new view
         (``View₂``).

.. [#f2] I.e. given a count (``ℕ``) returns a function mapping one view to
         another (``View₁ -> View₂``).
