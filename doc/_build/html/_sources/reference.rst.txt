
.. _reference:

*********************************************************************
                              Reference
*********************************************************************

Concepts
========

.. _sequence-concept:

Sequence
  Is a collection of elements of known (at compile-time) size which
  provides random access to elements. 

  To make a type ``S`` model the Sequence concept one needs to specialise
  :cpp:class:`sequence_traits <boost::static_views::sequence_traits>` for
  this type.

.. _view-concept:

View
  Is a proxy that provides access into another `view <view-concept>`_ or
  `sequence <sequence-concept>`_.
  

Views and Adaptors
==================

.. doxygenstruct:: view_base
  :project: static-views

.. doxygenstruct:: sequence_traits
  :project: static-views
  :members:
  :undoc-members:

.. doxygenstruct:: view_adaptor_core_access
  :project: static-views
  :members:
  :undoc-members:

.. doxygenstruct:: view_adaptor_base
  :project: static-views
  :members:
  :undoc-members:

.. doxygenvariable:: raw_view
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

Utility functions
=================

.. doxygenfunction:: make_wrapper
  :project: static-views

.. doxygenstruct:: is_view
  :project: static-views
