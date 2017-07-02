
.. _tutorial:

*********************************************************************
                              Tutorial
*********************************************************************



.. _overview:

Overview
========


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
I called this filter eager, because there is also a *lazy* filter, 
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
manipulate views. An algorithm is a function ``f``:math:`View_1 \to R`,
i.e. taking a view and producing some result. On top of the usual call
operator we also overload ``operator|`` for algorithms. This allows to use
them as pipes. If we have multiple algorithms with ``R`` being some view,
we can chain them as ``view | f1 | f2 | .... | fn``. This also provides a
way to use the ``optimise`` function automatically: if ``operator|``
detects that the view on the left is an *rvalue*, it can optimise it away.
We then repeat this operation recusively until an *lvalue* is encountered.

.. _examples:

Examples
========


Static Map
----------

Let's start by implementing a toy ``static_map``. We base is on ``hashed``
view of key-value pairs. The trickiest part is ``operator[]`` of
``static_map``::

  constexpr decltype(auto) operator[](key_type const& k)
  {
    // _hv is the hashed view.
    // _eq is the comparison function
    // For simplicity, let's use C++17 constexpr lambdas. If C++14
    // compatibility is required, it's easy to rewrite this using a
    // struct.
    return find_if(_hv[_hv.hash_function()(k)], [&k, this](auto&& x) { 
      return _eq(k, x.first); });
  }

We ask hashed view for a view of all the elements that (possibly) have the
same hash as ``k``. Then we perform a linear search through this view. It
doesn't get any simpler than that. Construction of static_map is also
quite simple::

  template < class Sequence
           , class Pred = std::less<void>
           , class Hasher = std::hash<void> // assume for a minute that
                                            // it's constexpr
           >
  constexpr auto make_static_map(Sequence& xs, Pred&& equal = Pred{},
    Hasher&& hash_function = Hasher{})
  {
    auto hashed_view  = xs | hashed(std::forward<Hasher>(hash_function));
    return static_map<decltype(hashed_view), Pred>{std::move(hashed_view),
      std::forward<Pred>(equal)};
  }

We let ``operator|`` do all the work of adopting the sequence ``xs``.
Notice that template arguments of ``static_map`` are ``HashedView`` and
``Pred`` in place of common ``Key``, ``Tp``, ``Pred``, ``Hasher``.
We can easily extract these types from the ``HashedView``::

    using value_type = std::decay_t< decltype( 
      *std::declval<HashedView>()[std::devlval<std::size_t>()].begin() )>;
    using key_type = typename value_type::first;
    using mapped_type = typename value_type::second;
    using key_equal = Pred;
    using hasher = typename HashedView::hasher;
    using reference = value_type &;
    using const_reference = value_type const&;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using iterator = typename HashedView::iterator;
    using const_iterator = typename HashedView::const_iterator;

There is some boilerplate code to write, but apart from that we're done.


Parsing Strings at Compile Time
-------------------------------

Strings are sequences of characters, so they may very well be used with
StaticViews. For example, let's verify that all brackets match in a string
literal::

  template <std::size_t N, std::size_t... Is>
  constexpr auto are_brackets_good_impl(char const(&xs)[N],
    std::index_sequence<Is...>) noexcept
  {
    struct Counter {
      int  n    = 0;
      bool good = true;

      constexpr auto operator()(char c) noexcept
      {
        if (c == '(') {
          ++n;
          return;
        }
        if (c == ')') {
          --n;
          if (n < 0) good = false;
        }
      }
    };

    Counter counter;
    for_each(raw_view(xs), counter);
    return counter.good && counter.n == 0;
  }

  template <std::size_t N>
  constexpr auto are_brackets_good(char const (&xs)[N]) noexcept
  {
    return are_brackets_good_impl(xs, std::make_index_sequence<N>{}); 
  }

  // later on
  static_assert(are_brackets_good("(1 + (2))"), "");
  static_assert(!are_brackets_good("(1 + ))(2", "");

Although this particular example doesn't use much of StaticViews
functionality, it illustrates the point that with ``constexpr`` we don't
have to use ``template <char...>`` whenever we want results at compile
time. Not only is code using ranges easier to write, it also runs faster,
because no recusive instantiations are used.

As a more difficult example, let's write a ``printf`` *functor*. For
simplicity, we will only support format specifiers of the form ``%type``::

  struct format_spec {
    char code;
  };

To check whether the user passed correct types we create a mapping from
type to a char code::

  template <class> struct type_to_code;

  template <> struct type_to_code<int>   { static constexpr char value = 'd'; };
  template <> struct type_to_code<float> { static constexpr char value = 'f'; };

This way we also get support for user-defined types -- one only
needs to register a type by specializing ``type_to_code``. 

Without loss of generality we can assume the format string to look like
``STR (FMT STR)*``, where ``STR`` are just some chars we need to print and
``FMT`` is a format specifier. ``STR`` can also have zero length.

We don't know the number of ``STR`` and ``FMT`` pieces in out format
string. We do know, however, that length of ``FMT`` is at least 2 (``%``
and type code). Thus number of ``FMT`` pieces is bounded from above by
half of the length of the format string. A handy data structure for this
case is a static_vector::

  // A vector with fixed-size underlying storage.
  template <class T, std::size_t N>
  struct static_vector {
      std::array<T, N> data;
      std::size_t      size;
  
      constexpr static_vector() noexcept
          : static_vector{std::make_index_sequence<N>{}}
      {
      }
  
      constexpr auto push_back(T x) noexcept
      {
          data[size] = x;
          ++size;
      }
  
  private:
      template <std::size_t... Is>
      constexpr static_vector(std::index_sequence<Is...>) noexcept
          : data{ ((void)Is, T{})... }
          , size{ 0 }
      {
      }
  };

We need a function to read a piece of format string which up to the next
format specifier::

  // Parses the STR part, returns the length
  template <class S>
  constexpr auto next_length(S&& xs) noexcept
  {
      return (xs | take_while([](auto&& c) { return c != '%'; })).size();
  }

And for the ``FMT`` part::

  // Parses the FMT part, returns format_spec
  template <class S>
  constexpr auto parse_format(S&& xs) noexcept -> format_spec
  {
      if (xs.size() < 2 || xs[0] != '%') throw std::runtime_error{""};
          return { xs[1] };
  }

And finally we can write the ``parse`` function::

  // The parsing function
  template <std::size_t N>
  constexpr auto parse(char const (&xs)[N])
  {
      static_vector<std::size_t, N / 2 + 1> begins;
      static_vector<std::size_t, N / 2 + 1> sizes;
      static_vector<format_spec, N / 2>     fmts;
  
      std::size_t b = 0; 
      std::size_t n = next_length(xs | drop(b));
      begins.push_back(b);
      sizes.push_back(n);
  
      b = b + n;
      while (b < N) {
          // read the FMT part
          fmts.push_back(parse_format(xs | drop(b)));
          b = b + 2;
          // read the STR part
          n = next_length(xs | drop(b));
          begins.push_back(b);
          sizes.push_back(n);
          b = b + n;
      }
  
      // for simplicity return just as a tuple
      return std::make_tuple(begins, sizes, fmts);
  }

This is it, there's only a little bit of template metaprogramming code
left to write (see `full source
<https://github.com/BoostGSoC17/static-map/blob/development/include/boost/static_map/parse.hpp>`_).
We can now write::

  constexpr auto x = FORMAT("i = %d, %f%% done");
  x(1, 43.2f);   // OK
  x(1);          // Error!
  x(1, "hello"); // Error!

Although we still have to use a macro, we completely avoided the "string
as a tuple of chars" representation, did almost no metaprogramming and
should thus have much better compile times.

