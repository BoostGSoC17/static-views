
<boost/static_views/errors.hpp>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Synopsis:

.. code-block:: cpp

   namespace boost {
   namespace static_views {

     #if defined(BOOST_STATIC_VIEWS_THROW_ON_FAILURES)
     class assertion_failure : public virtual std::runtime_error {
       /* implementation details */
     };
     #endif

     class full_bucket : public virtual std::runtime_error {
       public:
         full_bucket();
         full_bucket(char const*);
         full_bucket(std::string const&);
     };

     class out_of_bound : public virtual std::runtime_error {
       public:
         out_of_bound();
         out_of_bound(char const*);
         out_of_bound(std::string const&);
     };

   } // namespace boost
   } // namespace static_views


.. cpp:namespace:: boost::static_views


.. cpp:class:: assertion_failure

   Exception type that is thrown in when an assertion failure occurs if
   :c:macro:`BOOST_STATIC_VIEWS_THROW_ON_FAILURES` is defined.

.. cpp:class:: full_bucket

   Exception type that is thrown when an insertion into a full bucket is
   attempted.

.. cpp:class:: out_of_bound

   Exception type that is thrown when an out of bounds access is
   attempted.


