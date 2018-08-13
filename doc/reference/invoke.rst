<boost/static_views/detail/invoke.hpp>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. cpp:function:: template <class Function, class... Arguments> \
                    constexpr decltype(auto) \
                      invoke(Function&& fn, Arguments&&... args)

   Invokes the `Callable`_ object ``fn`` with the parameters ``args``.
   This is a ``constexpr`` (and ``C++14``-compatible) version of
   |std::invoke|_. It is ``noexcept`` whenever possible, i.e. if the
   object being invoked does not throw, then this function will be marked
   ``noexcept``.

   Example usage (requires ``C++17`` for |std::invoke|_):

   .. literalinclude:: ../../example/reference/invoke.cpp
      :language: cpp

.. _Callable: https://en.cppreference.com/w/cpp/named_req/Callable

.. |std::invoke| replace:: ``std::invoke``
.. _std::invoke: https://en.cppreference.com/w/cpp/utility/functional/invoke
