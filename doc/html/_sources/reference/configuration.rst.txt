.. _configuration:

Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. cpp:namespace:: boost::static_views

.. c:macro:: BOOST_STATIC_VIEWS_USE_BOOST

   If defined, `Boost.Config`_ will be used to detect compiler-specific
   functionality.


Specifying the error handling policy
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

The following macros allow one to configure how assertion failures are
handled. The default policy is
:c:macro:`BOOST_STATIC_VIEWS_THROW_ON_FAILURES`.

.. c:macro:: BOOST_STATIC_VIEWS_DISABLE_CHECKS

   If defined, assertions are replaced by calls to |builtin_assume|_
   (if the compiler supports it). Use this macro to achieve the best
   performance.

.. c:macro:: BOOST_STATIC_VIEWS_THROW_ON_FAILURES

   If defined, an exception of type :cpp:class:`assertion_failure` will be
   thrown in case of an assertion failure. Use this macro during
   development because :cpp:func:`assertion_failure::what()` provides some
   useful information about the location of the failure.

.. c:macro:: BOOST_STATIC_VIEWS_TERMINATE_ON_FAILURES

   If defined, |std::terminate|_ will be called on assertion failures.
   Use this macro in environments that do not support exceptions.


.. |builtin_assume| replace:: ``__builtin_assume``
.. _builtin_assume: https://clang.llvm.org/docs/LanguageExtensions.html#builtin-assume

.. |std::terminate| replace:: ``std::terminate``
.. _std::terminate: https://en.cppreference.com/w/cpp/error/terminate

.. _Boost.Config: http://www.boost.org/doc/libs/develop/libs/config/doc/html/index.html
