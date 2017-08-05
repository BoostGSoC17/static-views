
.. _reference:

*********************************************************************
                              Reference
*********************************************************************


.. role:: cpp(code)
  :language: c++

.. contents::
  :local:


.. default-domain:: cpp

.. highlight:: c++


.. _concepts:

Concepts
========

.. _sequence-concept:

Sequence
  Is a homogeneous collection of which provides random access to elements.
  If the number of elements in the collection is known at compile-time
  (i.e. is part of the type), the sequence is called *static*.

  To make a type ``T`` model the Sequence concept one needs to specialise
  :cpp:class:`sequence_traits <sequence_traits>` for this type. To check
  whether a type ``T`` models the Sequence or StaticSequence concepts the
  following constructs are provided:

  .. code-block:: cpp

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

  ``test()`` member functions allow one to test whether a type ``T``
  models a certain concept. They never fail, no matter. ``check()`` member
  functions, however, do fail if ``T`` doesn't model the concept. They
  use ``static_assert`` to produce nice messages explaining the problem.
  ``check()`` member functions are meant to be used to enforce a
  particular concept.

.. _view-concept:

View
  (Dummy definition) Is a proxy that provides access into another :ref:`view <view-concept>` or
  :ref:`sequence <sequence-concept>`.


.. _views-and-adaptors:

Views and Adaptors
==================

.. _raw-view:

Raw view
^^^^^^^^

The simplest view there is is the *raw view*. It is a non-owning wrapper
around a sequence which models the :ref:`view <view-concept>` concept.
Here's a synopsis:

  .. code-block:: cpp

    struct ... {
        ... // copy and move constructors and assignment operators

        static constexpr auto extent() noexcept ->std::ptrdiff_t;

        constexpr auto size() const noexcept ->std::size_t;

        constexpr decltype(auto) unsafe_at(std::size_t const i) const
          noexcept( whenever possible );

        constexpr decltype(auto) operator[](std::size_t const i) const;
    };

The exact type, as well as constructors, is an implementation detail.
For creation of raw views the following functor should be used.

.. doxygenvariable:: raw_view
  :project: static-views

:doc:`Here's <examples/using_raw_view>` an example:

.. literalinclude:: ../example/using_raw_view.cpp
   :lines: 10-31



.. _drop-view:

Drop view
^^^^^^^^^

.. doxygenvariable:: drop
  :project: static-views

So if you created a variable :cpp:`auto v = drop(n)(xs)`, the
following relation holds:

.. math::

   \text{v}[i] = \text{xs}[i + n] \;, \forall i \in \mathbb{N} \,.

Here's a synopsis of ``v``'s type:

.. code-block:: cpp

  struct ... {
      ... // copy and move constructors and assignment operators

      static
      constexpr auto extent() noexcept ->std::ptrdiff_t;

      constexpr auto size() const noexcept ->std::size_t;

      constexpr auto unsafe_at(std::size_t const) const& noexcept(whenever possible);
      constexpr auto unsafe_at(std::size_t const) &&     noexcept(whenever possible);

      constexpr auto operator[](std::size_t const) const&;
      constexpr auto operator[](std::size_t const) &&;

      constexpr auto map(std::size_t const) const noexcept ->std::size_t;

      constexpr auto parent() const& noexcept;
      constexpr auto parent() &&     noexcept;
  };

There are two non-standard functions: 

* .. cpp:function:: constexpr std::size_t map(std::size_t const i) noexcept

    Given an index ``i`` returns the corresponding index in the parent
    view. Let :math:`v` be a drop view, then :math:`\forall i\in
    \mathbb{N}:` :math:`v[i] = v.`:cpp:func:`parent() <parent>`:math:`[
    v.`:cpp:func:`map( <map>`:math:`i`:cpp:func:`) <map>`:math:`]`.

* .. cpp:function:: constexpr auto parent() noexcept

    Returns a reference to the parent view. In the :cpp:`auto v =
    drop(n)(xs)` example, :cpp:func:`parent()` returns ``xs``.

:doc:`Here's <examples/using_drop_view>` an example of usage:

.. literalinclude:: ../example/using_drop_view.cpp
   :lines: 11-45

Utilities
=========

.. doxygenvariable:: make_wrapper
   :project: static-views

.. doxygenstruct:: sequence_traits
   :project: static-views

Details
=======

.. doxygenstruct:: detail::wrapper
  :project: static-views
  :members:
  :undoc-members:

.. doxygenstruct:: detail::raw_view_impl
  :project: static-views
  :members:
  :undoc-members:

.. doxygenstruct:: detail::drop_impl
  :project: static-views
  :members:
  :undoc-members:
