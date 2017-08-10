.. _sequence-concept:

Sequence
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Is a homogeneous collection which provides random (indexed) access to
elements.  If the number of elements in the collection is known at
compile-time (i.e. is part of the type), the sequence is called *static*,
otherwise -- *dynamic*.

C-style arrays, ``std::array``, and ``std::tuple`` (of the same type,
obviously) model the StaticSequence out of the box. To make another type
``T`` model the Sequence concept one needs to specialise
:cpp:class:`sequence_traits <sequence_traits>` for this type.  To check
whether a type ``T`` models the *Sequence* or *StaticSequence* concepts
the following constructs are provided:

.. code-block:: cpp

  namespace boost {
  namespace static_views {
  namespace concepts {
  
    struct Sequence {
      template <class T>
      static constexpr auto test() noexcept -> bool;

      template <class T>
      static constexpr auto check() noexcept -> bool;
    };

    struct StaticSequence : Sequence {
      template <class T>
      static constexpr auto test() noexcept -> bool;

      template <class T>
      static constexpr auto check() noexcept -> bool;
    };

  } // namespace concepts
  } // namespace static_views
  } // namespace boost

``test()`` member functions allow one to test whether a type ``T`` models
a certain concept. So ``Sequence::test<T>()`` tells you whether ``T``
models the Sequence concept. ``test()``'s will compile with any ``T``.
``check()``'s member functions, however, will fail to compile if ``T``
doesn't model the concept. They will trigger ``static_assert`` failures to
produce nice messages explaining the problem. So use ``test()`` with
|enable if|_ or other SFINAE constructs and ``check()`` inside a function
body when you need to enforce a particular concept.

.. |enable if| replace:: ``std::enable_if``
.. _enable if: http://en.cppreference.com/w/cpp/types/enable_if

