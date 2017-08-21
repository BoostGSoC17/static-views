
.. _tutorial:

**************************************************************************
                                 Tutorial
**************************************************************************

.. default-domain:: cpp
.. highlight:: none

.. role:: cpp(code)
  :language: c++

.. contents::
  :local:

.. _installing:

Installing
==========================================================================

The simplest way to get started with the library is to use its single
header edition. Just one call to ``wget``:

.. code-block:: bash

  wget https://github.com/BoostGSoC17/static-views/single-header/static_views.hpp

and you're done! Alternatively, clone the `repo
<https://github.com/BoostGSoC17/static-views>`_ and point your compiler to
the ``include/`` directory.

The library has two modes of operation: "standalone" and "part of Boost".
The first one is chosen by default. Static Views then only relies on
``STL``. To use the second instead, define the
``BOOST_STATIC_VIEWS_USE_BOOST`` macro before the inclusion of the
headers. Static Views will then use `Boost.Config`_ macros instead of
reinventing the wheel.


.. _sequences-and-views:

Sequences and Views
==========================================================================

Very rarely do we deal with compile-time data of infinite size. Usually,
when we decide on hard-coding some data into the program, we store it as a
:cpp:`static` ``C``-style array or a tuple. These data structures not only
have finite size which is part of the type, they also provide random
access to elements. We call such data structures *StaticSequences*. If,
however, number of elements is not part of the type, we drop the word
"Static" and just call the data structure a *Sequence*. Sequences only
specify the amount of data and how to access it. Sequences are never
copied, moved or otherwise touched. It is common practice to declare
Sequence as global :cpp:`static` variable.

``C``-style arrays and :cpp:`std::tuple`'s aren't the only types that "are
Sequences" out of the box. :cpp:`std::array` and :cpp:`gsl::span`, for
example, are supported too. If you're unsure whether your own type
:cpp:`T` is considered a Sequence, you can ask the library:

.. code-block:: cpp

  static_assert(static_views::concepts::StaticSequence::test<T>(),
      "Nope, T is not a StaticSequence.");
  static_assert(static_views::concepts::Sequence::test<T>(),
      "Nope, T is not a Sequence.");

The core building blocks of the library is are views. Just like the name
suggests, views let you have a look at your data, modify it if you want,
but never ever do they own the data. Views can change the way you see
your data without changing the data itself. That's the main idea of having
views in the first place. So if you have a view :cpp:`xs` and want to
search for an element in the second half of it, how do you accomplish it?
Normally you would do something like this:

.. code-block:: cpp

   find(begin(xs) + xs.size() / 2, end(xs), element);

The main problem with this approach is that you can't compose the
algorithms easily. For example, suppose we add a requirement that only
every third element it to be examined. Using views, this problem is easily
solved:

.. code-block:: cpp

   find(xs | drop(xs.size() / 2) | stride(3), element);

This syntax should be familiar to anyone who's written a shell script or
two in their lives:

+--------------------------+--------------------------------+
|         ``C++``          |             ``Bash``           |
+==========================+================================+
| :cpp:`xs | drop(123)`    | ``cat "xs" | tail -n +123``    |
+--------------------------+--------------------------------+

If this syntax seems too radical, it's OK. You're not required to use it.
*Piping* is equivalent to a function call, so the previous example could
very well have been written as:

.. code-block:: cpp

   find(stride(3)(drop(xs.size() / 2)(xs)), element);

So you can see that views can be easily composed, i.e. you create views of
views of views etc. We need a base case to terminate the recursion,
though. There's a special views for that -- :ref:`raw_view <raw-view>`. It
is the only view that can be constructed from a sequence. **All other
views can only be constructed from views!**. 

.. code-block:: cpp

  using namespace static_views = boost::static_views;
  
  static constexpr unsigned maintainers_phone_number[] = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr auto xs = static_views::raw_view(maintainers_phone_number);

So you pass a reference to a Sequence to :ref:`raw_view <raw-view>` and
get a View back. Mathematically speaking

.. math::

   \mathtt{raw\_view}: \mathtt{Sequence} \to \mathtt{View} \,.

Such notation will be used throughout the documentation, because it would
be a lie to say that :cpp:`raw_view` is a function. It is a functor the
type of which is an implementation detail. So as a quick recap, this
:math:`f: \mathtt{View} \times \mathbb{N} \to \mathtt{char\ const*}` means
that :math:`f` is a function taking a View and a natural number (i.e.
unsigned integral type) and returning a pointer to :cpp:`char const`; this
:math:`g: (\mathbb{N} \to \mathbb{N}) \to \mathtt{View} \to \mathtt{View}`
means that :math:`g` is a function that takes a function mapping natural
numbers to themselves, and returns a function taking a View and returning
another View.


So :ref:`drop <drop-view>` that we've already encountered has the
following signature:

.. math::

   \mathtt{drop}: \mathbb{N} \to \mathtt{View} \to \mathtt{View} \,.

.. code-block:: cpp

  static constexpr char data[] = {'h', 'e', 'l', 'l', 'o'};
  // drop(n) drops the first n elements
  constexpr auto view_bad  = static_views::drop(2)(data); // Error! A Sequence rather
                                                          // than a View is passed
  constexpr auto view_good = static_views::drop(2)(       // OK
      static_views::raw_view(data));


Notice how in the example above an *rvalue* is passed to :ref:`drop
<drop-view>`. This is not the only supported use case. *Lvalues* can be
passed, too:

.. code-block:: cpp

  static constexpr auto data = std::make_tuple(1, 2, 3);
  static constexpr auto xs   = static_views::raw_view(data);
  constexpr auto view = static_views::drop(2)(xs);

The library then automatically decides what to store.  It's similar to
lambdas where you choose whether to capture a variable by value or by
reference. There's one important thing to note -- the use of
:cpp:`static`. "Normal" :cpp:`constexpr` variables live only in the mind
of the compiler. You simply **can't take their address inside constant
expressions** as specified in section 5.20 of the Standard:

  | if the value is of pointer type, it contains the
  | address of an object with static storage duration,
  | the address past the end of such an object (5.7),
  | the address of a function, or a null pointer value

There is, unfortunately, no way issue :cpp:`static_assert`'s on such
errors. So that's probably the only "rule" you really need to remember
when using the library. Compilers try to be helpful though:

.. code-block:: none

  test.cpp:16:20: error: constexpr variable 'view' must be initialized by
  a constant expression
      constexpr auto view = static_views::drop(2)(xs);
                     ^      ~~~~~~~~~~~~~~~~~~~~~~~~~
  test.cpp:16:20: note: pointer to 'xs' is not a constant expression
  test.cpp:15:20: note: declared here
      constexpr auto xs   = static_views::raw_view(data);

Says Clang, if you forget to make :cpp:`xs` static.


By now you should have an idea of what Views are and how to create them.
The only remaining question is what to do with them later. Here's a
synopsis of a general view:

.. code-block:: cpp

  struct View {
      View(View&&); 

      // Returns the number of elements viewed.
      constexpr auto size() const noexcept -> std::size_t;

      // Return the element at the specified index. These versions
      // are optimised for performance and perform no error checking.
      // Use them when implementing optimised algorithms etc.
      constexpr auto unsafe_at(std::size_t) noexcept;
      constexpr auto unsafe_at(std::size_t) const noexcept;

      // Return the element at the specified index. An exception is
      // thrown if the index is out of bounds.
      constexpr auto operator[](std::size_t);
      constexpr auto operator[](std::size_t) const;

      // Returns either dynamic_extent to indicate that the size
      // is unknown until run-time or a non-negative number that is
      // at least as big as size().
      static constexpr std::ptrdiff_t extent() noexcept;

      // Iterators
      constexpr auto begin() const noexcept;
      constexpr auto end() const noexcept;
  };



.. _static-map-tutorial:

Static Map
==========================================================================

As a real life example of what one can implement using the library, an
implementation of a :cpp:`static_map` is provided. This is also probably
the coolest feature of the library.

Consider the following scenario. You have some data that's very unlikely
to change, so you've hard-coded it:

.. code-block:: cpp

  static constexpr std::pair<int, char const*> data[] = {
      { 123, "apple" },
      { 456, "pear" },
      { 789, "banana" }
  };

Now some time later you need to get the fruit by code. You write the
following function

.. code-block:: cpp

  constexpr auto get_fruit(int const code) noexcept -> char const*
  {
      constexpr auto n = sizeof(data) / sizeof(data[0]);
      for (std::size_t i = 0; i < n; ++i) {
          if (data[i].first == code)
              return data[i].second;
      }
      throw some_error{"Key not found"};
  }

and use it at compile-time happily ever after:

.. code-block:: cpp

  constexpr auto fruit = get_fruit(123);

Then one day you need to get the fruit without knowing the :cpp:`code` at
compile-time. Not a big deal for small datasets, right? But what to do
when data has a couple thousands of elements? You can, of course, feed
:cpp:`data` to :cpp:`std::unordered_map` (assuming codes are hashable).
But you lose the :cpp:`constexpr`'ness of :cpp:`get_fruit`, and the
dreadful dynamic memory allocations...

So for users who are too lazy to write their own stack allocators every
time the need to search some static data, there is :cpp:`static_map`. You
create a view of the data, choose which fields should be used as keys and
mapped values, and feed all that to :cpp:`make_static_map`:

.. code-block:: cpp

  namespace static_map = boost::static_views::static_map;
  static constexpr auto fruits = static_map::make_static_map(
      boost::static_views::raw_view(data),
      [](auto&& x) { return x.first; },
      [](auto&& x) { return x.second; });

  // ... later ...

  constexpr auto fruit = fruits[123];

OK, this will only work in ``C++17``. We can, however, come with an
equally elegant solution in ``C++14``:

.. code-block:: cpp

  struct data_type {
      int         code;
      char const* fruit;
  };

  static constexpr data_type data[] = {
      { 123, "apple" },
      { 456, "pear" },
      { 789, "banana" }
  };

  static constexpr auto fruits = static_map::make_static_map(
      boost::static_views::raw_view(data),
      &field::code,
      &field::fruit);

  static_assert(equal_c(fruits[456], "pear"), ""); // Assume that equal_c
                                                   // compares strings at
                                                   // compile-time.

:cpp:`static_map` is a hash map, so lookups are :math:`\mathcal{O}(1)`,
both at compile-time and at run-time. At most :cpp:`fruits.bucket_size()`
comparisons of keys are performed, where the bucket size is ``2`` by
default, but can be set explicitly if you feel your hash function isn't
that good. Custom comparison and hash functions are supported, too. The
feature that distinguishes :cpp:`static_map` from a common hash map is the
fact that you choose which field it used as a key and which as a value. It
is thus not required for :cpp:`data_type` to only have two fields. You can
use arbitrary types, as long as they have :cpp:`constexpr` constructors.


.. _a-note-on-performance:

A Note on Performance
=========================================================================







.. _a-note-on-error-messages:

A Note on Error Messages
=========================================================================

Consider a simple example from above:

.. code-block:: cpp
   
  static constexpr char data[] = {'h', 'e', 'l', 'l', 'o'};
  constexpr auto view_bad  = static_views::drop(2)(data); // Error!

Here, we forgot to create a :ref:`raw_view <raw-view>` of :cpp:`data`
before passing it to :ref:`drop <drop-view>`. And here's the error message
Clang produces:

.. code-block:: none

  In file included from test.cpp:8:
  In file included from ./include/boost/static_views/drop.hpp:13:
  ./include/boost/static_views/algorithm_base.hpp:174:9: error: static_assert failed "I'm sorry, but `View` must model the View concept!"
          static_assert(
          ^
  test.cpp:15:52: note: in instantiation of function template specialization
        'boost::static_views::detail::algorithm_impl<boost::static_views::detail::make_drop_impl, unsigned long>::operator()<char const (&)[5], void, void>'
        requested here
      constexpr auto view_bad = static_views::drop(2)(data); // Error! 
                                                     ^
  ... [snip] ...

  In file included from test.cpp:7:
  In file included from ./include/boost/static_views/raw_view.hpp:17:
  ./include/boost/static_views/view_base.hpp:82:5: error: static_assert failed "`T` doesn't derive from `boost::static_views::view_base`, which is,
        unfortunately, required by the View concept. Hence, the error."
      BOOST_STATIC_VIEWS_DEFINE_CHECK(Derives_from_view_base, T,
      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  ... [snip] ...

  In file included from test.cpp:7:
  In file included from ./include/boost/static_views/raw_view.hpp:17:
  ./include/boost/static_views/view_base.hpp:88:5: error: static_assert failed "`T` doesn't model the MoveConstructible concept, which it should, to have a
        chance at modeling the View concept. Hence, the error."
      BOOST_STATIC_VIEWS_DEFINE_CHECK(Is_move_constructible, T,
      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  ... [snip] ...

So Clang first tells you that data isn't a view. In such simple cases as
above it's obvious that :cpp:`char const[5]` is a sequence rather than a
view. If, however, you've made a mistake while implementing your own
complex view, it may not immediately be obvious what went wrong. For that
reason the library also tells you exactly which requirements aren't
satisfied.


.. _Boost.Config: http://www.boost.org/doc/libs/develop/libs/config/doc/html/index.html
.. _Boost.Hana: http://www.boost.org/doc/libs/develop/libs/hana/doc/html/index.html
.. _Boost.Fusion: http://www.boost.org/doc/libs/develop/libs/fusion/doc/html/index.html
.. _Boost.Range: http://www.boost.org/doc/libs/develop/libs/range/doc/html/index.html
.. _Boost.Mp11: http://www.boost.org/doc/libs/develop/libs/mp11/doc/html/index.html
.. _Ranges TS: https://ericniebler.github.io/range-v3
