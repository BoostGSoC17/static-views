
.. _reference:

*********************************************************************
                              Reference
*********************************************************************

.. contents::
  :local:

   

.. _concepts:

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

.. _algorithm-concept:

Algorithm
  Is a function that takes a `view <view-concept>`_ and returns something
  (usually another view).
  

.. _views-and-adaptors:

Views and Adaptors
==================

.. doxygenstruct:: view_base
  :project: static-views

.. doxygenstruct:: algorithm_base
  :project: static-views

.. doxygenstruct:: sequence_traits
  :project: static-views
  :members:
  :undoc-members:

.. doxygenvariable:: raw_view
  :project: static-views

.. doxygenvariable:: drop
  :project: static-views

.. doxygenvariable:: take
  :project: static-views

.. doxygenvariable:: slice
  :project: static-views

.. doxygenvariable:: through
  :project: static-views

.. doxygenvariable:: hashed
  :project: static-views

Errors
======

.. doxygenclass:: out_of_bound
  :project: static-views

.. doxygenclass:: full_bucket
  :project: static-views

Utility functions
=================

.. doxygenfunction:: make_wrapper
  :project: static-views

.. doxygenstruct:: is_view
  :project: static-views

.. doxygenstruct:: is_algorithm
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

.. doxygenstruct:: detail::take_impl
  :project: static-views
  :members:
  :undoc-members:

.. doxygenstruct:: detail::slice_impl
  :project: static-views
  :members:
  :undoc-members:

.. doxygenstruct:: detail::through_impl
  :project: static-views
  :members:
  :undoc-members:

.. doxygenstruct:: detail::hashed_impl
  :project: static-views
  :members:
  :undoc-members:
