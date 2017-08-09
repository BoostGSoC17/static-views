##########################################################################
                         Boost.StaticViews
##########################################################################

.. highlight:: none

.. role:: cpp(code)
  :language: c++

.. toctree::
   :maxdepth: 2

   tutorial
   reference
   examples
   tests

.. _overview:

Overview
==========================================================================

Static Views is a *compile-time* analogue of `Ranges TS`_. By compile-time
I mean that it's best used on compile-time data. But no means is the usage
limited to compile-time. Recall the classification of computations in
``C++`` Louis Dionne introduced in `Boost.Hana`_ docs. There are four
types of computations in ``C++``:

1) Runtime computations.
2) ``constexpr`` computations.
3) Heterogeneous computations.
4) Type-level computations.

If we focus on manipulating containers, it seems that we currently have
modern libraries to fill all but one of the quadrants.  `STL
<http://en.cppreference.com/w/cpp/algorithm>`_, `Boost.Range`_ and `Ranges
TS`_ focus on the first quadrant.  `Boost.Fusion`_ and `Boost.Hana`_
occupy the third. `Boost.Hana`_ actually covers the fourth as well. We also
have `Boost.Mp11`_ for type-level computations. But what about the second
quadrant? There is a number of proposals to make some standard algorithms
and iterators :cpp:`constexpr`. They won't be available till ``C++20`` at
least. 

Static views is aimed to provide the tools for manipulation of
:cpp:`constexpr` data, and provide it *now*, in ``C++14``. Although the
library might make use of ``C++17`` features when available, a ``C++14``
conforming compiler is all you need to get started. The library provides a
small subset of views available in `Ranges TS`_, and as en example of what
one can achieve using these tools -- :cpp:`static_map`, a compile-time
hash table.



Indices and tables
==================

* :ref:`genindex`
* :ref:`search`


.. _Boost.Hana: http://www.boost.org/doc/libs/develop/libs/hana/doc/html/index.html
.. _Boost.Fusion: http://www.boost.org/doc/libs/develop/libs/fusion/doc/html/index.html
.. _Boost.Range: http://www.boost.org/doc/libs/develop/libs/range/doc/html/index.html
.. _Boost.Mp11: http://www.boost.org/doc/libs/develop/libs/mp11/doc/html/index.html
.. _Ranges TS: https://ericniebler.github.io/range-v3
