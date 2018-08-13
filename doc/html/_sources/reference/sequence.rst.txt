.. _sequence-concept:

Sequence
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Is a homogeneous collection which provides random (indexed) access to
elements. If the number of elements in the collection is known at
compile-time (i.e. is part of the type), the sequence is called *static*,
otherwise -- *dynamic*.

Uniform interface to sequences is provided through
:cpp:expr:`sequence_traits`:

.. code-block:: cpp

   namespace boost {
   namespace static_views {
     template <class Sequence>
     struct sequence_traits {
       using value_type      = ...;
       using reference       = ...;
       using const_reference = ...;
       using index_type      = ...;
       using size_type       = ...;
       using difference_type = ...;

       static constexpr auto extent() noexcept -> std::ptrdiff_t;
       static constexpr auto size(Sequence const&) noexcept -> size_type;
       static constexpr auto at(Sequence&, index_type) noexcept -> reference;
       static constexpr auto at(Sequence const&, index_type) noexcept -> const_reference;
     };
   } // namespace static_views
   } // namespace boost

``size()`` and ``at()`` member functions are hopefully self-explanatory.
``extent()`` allows one to distinguish between static and dynamic
sequences:

    * for static sequences it should return the size of the sequence;
    * for dynamic sequences it should return :cpp:var:`dynamic_extent`
      indicating that the size cannot be extracted from the type.

For type ``T`` To model the Sequence concept, there should exist a
specialisation of :cpp:class:`sequence_traits` with typedefs and member
functions listed above. To be more precise, here are the Sequence and
StaticSequence concepts definitions written in Concepts TS syntax:

.. code-block:: cpp

   namespace boost {
   namespace static_views {

     template <class T>
     concept bool Sequence = requires(T& a, T const& b,
         typename sequence_traits<T>::index_type i)
     {
       { sequence_traits<T>::extent() } noexcept -> std::ptrdiff_t;
       sequence_traits<T>::extent() >= 0
           || sequence_traits<T>::extent() == dynamic_extent;
       { sequence_traits<T>::size(b)  } noexcept
           -> typename sequence_traits<T>::size_type;
       { sequence_traits<T>::at(a, i) }
           -> typename sequence_traits<T>::reference;
       { sequence_traits<T>::at(b, i) }
           -> typename sequence_traits<T>::const_reference;
     };

     template <class T>
     concept bool StaticSequence = requires()
     {
       Sequence<T>;
       sequence_traits<T>::extent() >= 0;
     };

   } // namespace static_views
   } // namespace boost

C-style arrays, |std array|_ , and |std tuple|_ (of the same type,
obviously) model the StaticSequence concept out of the box.

.. |enable if| replace:: ``std::enable_if``
.. _enable if: http://en.cppreference.com/w/cpp/types/enable_if

.. |std array| replace:: ``std::array``
.. _std array: https://en.cppreference.com/w/cpp/container/array

.. |std tuple| replace:: ``std::tuple``
.. _std tuple: https://en.cppreference.com/w/cpp/utility/tuple
