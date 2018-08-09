<boost/static_views/transform.hpp>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Synopsis:

.. code-block:: cpp

   namespace boost {
   namespace static_views {

     // Exposition only
     template <class V, class F>
       requires (View<remove_cvref<V>>
              && Invocable<F, typename remove_cvref<V>::reference>)
     constexpr auto make_transform_view(V&&, F&&) noexcept(whenever possible);

     // Exposition only
     template <class F>
     constexpr auto make_transform_view(F&&) noexcept(whenever possible);


     constexpr auto transform = [] (auto&&... args) {
       return make_transform_view(std::forward<decltype(args)>(args)...);
     };

   } // namespace static_views
   } // namespace boost

.. cpp:namespace:: boost::static_views

.. cpp:function:: template <class V, class F> \
                  constexpr auto make_transform_view(V&& xs, F&& fn)

   .. note:: This function is not real and is provided here for exposition
             pusposes only.

   Creates a view of ``xs`` where each element ``x`` is transformed to
   ``invoke(fn, x)``.

   :param xs: The view to adapt. It must model the :ref:`view-concept` concept.

     * If ``xs`` is an lvalue reference, then the returned view only
       stores a pointer to ``xs``.
     * If ``xs`` is an rvalue reference, then the ownership of ``xs`` is
       transformed to the returned view using ``xs``' move constructor.
   :param fn: The function to apply to elements of ``xs``. It must be
              Invocable with :cpp:expr:`typename V::reference`.
   :return: Transform view of ``xs``.

.. cpp:function:: template <class F> \
                  constexpr auto make_transform_view(F&& fn)

   .. note:: This function is not real and is provided here for exposition
             pusposes only.

   This is a flipped (arguments are in reverse order) curried version of
   the above function. Its main purpose is to be used in pipes.

   :param fn: The function to apply to elements of the view.
   :return:   The function :cpp:expr:`[fn] (auto xs) {
              make_transform_view(xs, fn); };`.  The lambda here is shown for
              exposition only; the actual implementation is more efficient.

.. cpp:var:: constexpr auto transform = [] (auto&&... args) { \
                 return make_transform_view(std::forward<decltype(args)>(args)...); \
             };

   A wrapper function with the following signatures:
  
     * ``View₁, (View₁::reference -> View₂::reference)    -> View₂``  [#f1]_
     * ``(View₁::reference -> View₂::reference) -> (View₁ -> View₂)`` [#f2]_

   Example usage:
   
   .. literalinclude:: ../../example/using_transform_view.cpp
      :language: cpp
     


.. [#f1] I.e. given a view (``View₁``) and a function working on elements
         of it (``View₁::reference -> View₂::reference``), returns a new view
         (``View₂``).

.. [#f2] I.e. given a function working on elements of a view
         (``View₁::reference -> View₂::reference``), returns a function mapping
         one view to another (``View₁ -> View₂``).
