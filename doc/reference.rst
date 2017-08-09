
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
  Is a homogeneous collection which provides random (indexed) access to
  elements.  If the number of elements in the collection is known at
  compile-time (i.e. is part of the type), the sequence is called
  *static*, otherwise -- *dynamic*.

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

  ``test()`` member functions allow one to test whether a type ``T``
  models a certain concept. So ``Sequence::test<T>()`` tells you whether
  ``T`` models the Sequence concept. ``test()``'s will compile with any
  ``T``.  ``check()``'s member functions, however, will fail to compile if
  ``T`` doesn't model the concept. They will trigger ``static_assert``
  failures to produce nice messages explaining the problem. So use
  ``test()`` with |enable if|_ or other SFINAE constructs and
  ``check()`` inside a function body when you need to enforce a particular
  concept.

  .. |enable if| replace:: ``std::enable_if``
  .. _enable if: http://en.cppreference.com/w/cpp/types/enable_if


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


.. ==================================================================


.. _drop-view:

Drop view
^^^^^^^^^

.. doxygenvariable:: drop
  :project: static-views

.. doxygenvariable:: drop_exactly
  :project: static-views

So if you created a variable :cpp:`auto v = drop(n)(xs)`, the
following relation holds:

.. math::

   \mathtt{v[}i\mathtt{]} = \mathtt{xs[}i + n\mathtt{]} \;,
       \forall i \in \mathbb{N} \,.

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

.. cpp:namespace-push:: drop_view

* .. cpp:function:: constexpr std::size_t map(std::size_t const i) noexcept

    Given an index ``i`` returns the corresponding index in the parent
    view. Let ``v`` be a drop view, then :math:`\forall i\in
    \{0,\dots,\mathtt{v.size()}-1\}: \mathtt{v[}i\mathtt{]} =
    \mathtt{v.parent()[} \mathtt{v.map(} i \mathtt{)]}` For :math:`i \geq
    \mathtt{v.size()}`, the behavior of this function is undefined.

* .. cpp:function:: constexpr auto parent() noexcept

    Returns a reference to the parent view. In the :cpp:`auto v =
    drop(n)(xs)` example, :cpp:func:`parent()` returns ``xs``.

.. cpp:namespace-pop::

:doc:`Here's <examples/using_drop_view>` an example of usage:

.. literalinclude:: ../example/using_drop_view.cpp
   :lines: 11-45


.. ==================================================================


.. _take-view:

Take view
^^^^^^^^^

.. doxygenvariable:: take
  :project: static-views

So if you created a variable :cpp:`auto v = take(n)(xs)`, the
following relation holds:

.. math::

   \mathtt{v[}i\mathtt{]} = \mathtt{xs[}i\mathtt{]} \;,
       \forall i \in \{0,\dots,\mathtt{n}-1\} \,,

I.e. for :math:`i \leq \mathtt{n}` take view behaves just like ``xs``
itself.

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

Like most of the views provided by the library, take view has
:cpp:func:`map() <take_view::map>` and :cpp:func:`parent()
<take_view::parent>` member functions which are not, strictly speaking,
required by the :ref:`view <view-concept>` concept.

.. cpp:namespace-push:: take_view

* .. cpp:function:: constexpr std::size_t map(std::size_t const i) noexcept

    Given an index ``i`` returns the corresponding index in the parent
    view. Let ``v`` be a take view, then :math:`\forall i\in
    \{0,\dots,\mathtt{v.size()} - 1\}: \mathtt{v[}i\mathtt{]} =
    \mathtt{v.parent()[v.map(}i\mathtt{)]}`. For :math:`i \geq
    \mathtt{v.size()}` the behavior of this function is undefined.

* .. cpp:function:: constexpr auto parent() noexcept

    Returns a eference to the parent view. In the :cpp:`auto v =
    take(n)(xs)` example, :cpp:func:`parent()` returns ``xs``.

.. cpp:namespace-pop::

:doc:`Here's <examples/using_take_view>` an example of usage:

.. literalinclude:: ../example/using_take_view.cpp
   :lines: 11-44


.. ==================================================================


.. _slice-view:

Slice view
^^^^^^^^^^

.. doxygenvariable:: slice
  :project: static-views

.. cpp:namespace-push:: slice_view

Slice view is almost equivalent to a ":ref:`take <take-view>` view of a
:ref:`drop <drop-view>` view", i.e. ``slice(b, e)(xs)`` is the same as
``take_exactly(e)(drop_exactly(b)(xs))``. The *almost* comes from the
:cpp:func:`map() <map>` and :cpp:func:`parent() <parent>` member
functions. Slice view is a *single view* and calling :cpp:func:`parent`
will thus return ``xs``. The "take view of view view" is a *stack of two
views* -- calling :cpp:func:`parent <take::parent>` will return
``drop_exactly(b)(xs)``.


Here's a synopsis of slice view:

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

* .. cpp:function:: constexpr std::size_t map(std::size_t const i) noexcept

    Given an index ``i`` returns the corresponding index in the parent
    view. Let ``v`` be a slice view, then :math:`\forall i\in
    \{0,\dots,\mathtt{v.size()} - 1\}: \mathtt{v[}i\mathtt{]} =
    \mathtt{v.parent()[v.map(}i\mathtt{)]}`. For :math:`i \geq
    \mathtt{v.size()}` the behavior of this function is undefined.

* .. cpp:function:: constexpr auto parent() noexcept

    Returns a reference to the parent view. In the :cpp:`auto v =
    slice(b, e)(xs)` example, :cpp:func:`parent()` returns ``xs``.


.. cpp:namespace-pop::

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
