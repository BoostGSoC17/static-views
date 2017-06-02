.. _overview:

***************
Overview
***************


.. default-domain:: cpp
.. highlight:: c++


Static Views is a compile-time analogue of
`Ranges TS <https://ericniebler.github.io/range-v3/>`_. Recall the
classification of computations in ``C++`` Louis Dionne introduced in
`Boost.Hana <http://www.boost.org/doc/libs/1_64_0/libs/hana/doc/html/index.html>`_ 
docs. There are four types of computations in ``C++``:

1) Runtime computations.
2) ``constexpr`` computations.
3) Heterogeneous computations.
4) Type-level computations.

If focus on manipulating containers it seems that we currently have modern
libraries to fill all but one of the quadrants. 
`STL <http://en.cppreference.com/w/cpp/algorithm>`_, 
`Boost.Range <http://www.boost.org/doc/libs/1_64_0/libs/range/doc/html/index.html>`_ 
and `Ranges TS <https://ericniebler.github.io/range-v3/>`_ focus on the
first quadrant. 
`Boost.Fusion <http://www.boost.org/doc/libs/1_64_0/libs/fusion/doc/html/index.html>`_ 
and Boost.Hana occupy the third. Boost.Hana
actually covers the fourth as well. There is a number of template
metaprogramming libraries submitted for review. They all focus on
type-level computations.

But what about the second quadrant? What is currently the best way to,
say, find the smallest element in the second half of a constexpr C-style
array? Probably something along the lines::

  template <class T, std::size_t N>
  constexpr decltype(auto) find_min_second_half(T (&xs)[N]) noexcept
  {
    std::size_t min = N / 2 + 1;
    for (std::size_t i = min + 1; i < N; ++i)
      if (xs[i] < xs[min]) min = i;
    return xs[min];
  }

Is it difficult? No. Is it a good idea to write something like that each
time we want to perform a simple operation on a ``constexpr`` array? I 
don't think so. That's where *StaticViews* enters the game. The main goal
of this library is to provide a set of reusable and composable *views* 
to make most common operations one-liners.::

  constexpr int xs[] = {8, -7, 10, -1, 8, -2, 5};

  // The example from above
  constexpr auto m = xs | drop(std::size(xs) / 2) 
                        | min();
  static_assert(m == -2, "");

I used ``constexpr`` C-array as the most well-known example of constexpr
container, but pretty much any compile-time container of known size can
act as a source of data::

  constexpr std::array<int, 7> xs = {{8, -7, 10, -1, 8, -2, 5}};
  static_assert(
    compose( [](auto&& v) { return v.size(); }
           , filter([](auto&& x) { return x % 2 == 0; })
           , raw_view
           )(xs) == 4, "");

This is, of course, only valid in ``C++17``, but this is essentially what
StaticViews is targeted on. The library itself will be written in 
``C++14``, but constexpr lambdas make the user experience much better.

Initially, the goal of this project was implementation of a ``static_map``
container. Ideally, one would like to choose a memory layout policy,
error handling policy, searching algorithm, comparison function, hash
function and much more. At this point, it seems easier to follow the path
of 
`Boost.MultiIndex <http://www.boost.org/doc/libs/1_64_0/libs/multi_index/doc/index.html>`_
and to provide building blocks which everyone can combine into a container
suitable for his use case.

Although, we currently can't create a ``constexpr`` linked list where 
linking is achieved through pointers, we can create a linked list using
indices. The following picture
illustrates it further

.. code-block:: none

  normal linked list                 list based on indices

  +-------+------+<<< nullptr          +-------+-------+
  | DATA0 | PTRS |                     | DATA0 | -1, 2 |
  +-------+------+>>>-+                +-------+-------+
                      |
  +-------+------+<<<-|--------+       +-------+-------+
  | DATA1 | PTRS |    |        |       | DATA1 |  3, 4 |
  +-------+------+>>>-|---+    |       +-------+-------+
                      |   |    |
  +-------+------+<<<-+   |    |       +-------+-------+
  | DATA2 | PTRS |        |    |       | DATA2 |  0, 3 |
  +-------+------+>>>-+   |    |       +-------+-------+
                      |   |    |
  +-------+------+<<<-+   |    |       +-------+-------+
  | DATA3 | PTRS |        |    |       | DATA3 |  2, 1 |
  +-------+------+>>>-----|--->+       +-------+-------+
                          |
  +-------+------+<<<-----+            +-------+-------+
  | DATA4 | PTRS |                     | DATA4 | 1, -1 |
  +-------+------+>>> nullptr          +-------+-------+

Data structure on the right can easily be implemented in ``constexpr``.
But do we actually need something like that? We can just save the
*permutation* of elements in another array.

.. code-block:: none

  +---+---+---+---+---+
  | 0 | 2 | 3 | 1 | 4 |
  +---+---+---+---+---+

We get previous/next elements by looking at left/right neighbour.
Naturally, we arrive at the concept of a *view*. A view is something
that provides us with indexed access of our data. To abstract away
from how this data is actually represented, we define sequence
traits::

  template <class Sequence>
  struct sequence_traits {
    static constexpr auto size() noexcept -> std::size_t;
    static constexpr decltype(auto) at(std::size_t) noexcept(??);
  };
  
StaticViews defines a single view called ``raw_view`` that adds
*view interface* to the sequence. This allows to only think in terms
of views and totally forget about sequences (I believe this is done
in Ranges TS using a class that's even named ``view_interface``).
Now that we've moved sequences out of the picture, we turn to views.
What we want from a ``View`` is something along the lines::

  template <class C> // Container type we are viewing
                     // which is usually again a View.
  struct View {
    constexpr auto begin(); // Give me an iterator to the beginning
    constexpr auto end(); // Returns a sentinel.

    constexpr auto size() -> std::size_t; // (OPTIONAL)
                                          // How many elements can
                                          // I see through this view.

    constexpr decltype(auto) operator[](T i); // Give me the element at
                                              // index `i`.
    // ?? constexpr auto map_to_parent(T i); 
    // ?? constexpr auto parent();

    static constexpr auto capacity() -> std::size_t; // What's an upper
                                                     // bound on the
                                                     // number of elements
                                                     // in C?
  };
  
Now why do we need each of these functions?

``begin`` and ``end``
  are the bare bones of ranges. Although we live in constexpr land and
  can do everything with indices, iterators provide a nice way to
  express forward ranges, because, ``size()`` for a forward range is 
  :math:`\mathcal{O}(N)`.

``size``
  returns the number of elements in the view.

``capacity``
  is required to return an upper bound on the number of elements in this
  view. Thus we always have ``size() <= capacity``. We need this function
  to be able to determine the size of a buffer. For example, if we want
  to have random access into a filtered range, we need the filter view
  to store the indices of "good" elements in an internal buffer. But we
  need the size of this buffer before filtering, i.e. before we know
  what ``size()`` is. ``capacity()`` function solves this problem. Yes,
  we use somewhat more storage than necessary, but we gain random access.

``operator[]``
  provides indexed access to the data. Notice that I haven't
  specified the index type to be used. It can differ per View. Although
  for random access iterators it is usual to have 
  ``begin()[i] == operator[](i)``, it is not required.
  
Views can be divided into two categories: stateful and stateless.
Examples of stateless views are ``drop``, ``drop_while``, ``stride``.
For these views ``operator[]`` can be written as a composition of
some function ``map:`` :math:`T \to T` that performs some operations
on the index ``i``, and a call to ``parent().operator[]``. For example, ``drop(n)``'s ``operator[]`` can be implemented as::
  
  constexpr auto operator[](std::size_t const i)
  {
    return parent()[i + n];
  }
    

Such views are very much like the views in Ranges TS, because they 
provide constant time copy constructors. 

An example of a stateful view is *eager* ``filter``. Indeed, to provide
random access, we keep an array of "good" indices. That's our state. 
Although ``capacity`` is fixed upon compilation and thus copying such
an array is ``O(1)``, I still like to think about it as being ``O(N)``.
I called this filter ``eager``, because there is also a *lazy* filter, 
one that does the filtering on an as-needed basis. The problem with 
eager filtering is that chaining many such filters results in a
substantial extra memory being used. Lazy filtering is not perfect
either, it doesn't provide random access to elements and may perform
many calls to the predicate function. So a compromise could be reached
by an ``optimise()`` function. The idea is that it takes a view of 
a view of a view of a view ... of a sequence and claps this whole chain
into a single view of a sequence.

There is one view that deserves some extra attention -- ``hashed`` view.
First of all, it is terribly inefficient to use buckets to solve
collisions. Open adressing is the way to go. This means that we have
to keep an array which size is larger than the amount of elements. But
then we can't say in ``O(1)`` time which is the ``i``'th occupied element.
What would be nice is to receive a *hash* as an index and return a view
of elements that (possibly) have the same hash. This allows to decouple
hashing and comparing of elements: ``hashed`` view only knows how to hash
an element, and search algorithm receives a predicate to compare the
elements. Say, we have a hashed view of a dictionary (list of words) and
use word length as a hash. We can then easily obtain all the four-letter
words that have at least two vowels by asking the hashed view for all
words with four letters and the filtering out the ones with less than two
vowels.

Just like in Boost.Range and Ranges TS, we can also have *algorithms* that
manipulate views. An algorithm is a function ``f``:math:`View1 \to R`,
i.e. taking a view and producing some result. On top of the usual call
operator we also overload ``operator|`` for algorithms. This allows to use
them as pipes. If we have multiple algorithms with ``R`` being some view,
we can chain them as ``view | f1 | f2 | .... | fn``. This also provides a
way to use the ``optimise`` function automatically: if ``operator|``
detects that the view on the left is an *rvalue*, it can optimise it away.
We then repeat this operation recusively until an *lvalue* is encountered.


