
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


.. _sequences:

Sequences
==========================================================================

Very rarely do we deal with compile-time data of infinite size. Usually,
when we decide on hard-coding some data into the program, we store it as a
:cpp:`static` ``C``-style array or a tuple. These data structures not only
have finite size which is part of the type, they also provide random
access to elements. We call such data structures *StaticSequences*. If,
however, number of elements is not part of the type, we drop the word
"Static" and just call the data structure a *Sequence*.
:cpp:`std::initializer_list` and :cpp:`std::vector` are examples of
(non-Static) Sequences.

Sequences only specify the amount of data and how to access it. They are
never copied, moved or otherwise touched. It is common practice to declare
a Sequence as a global :cpp:`static` variable.

``C``-style arrays and :cpp:`std::tuple`'s aren't the only types that "are
Sequences" out of the box. :cpp:`std::array` and :cpp:`gsl::span`, for
example, are supported too. By supported I means that StaticViews agrees
that a particular type is indeed a sequence. This information is part of
the public interface, so you can ask the library whether some type
:cpp:`T` is considered a Sequence:

.. code-block:: cpp

  // Ask whether T is a Sequence
  constexpr bool T_is_a_Sequence = static_views::concepts::StaticSequence::test<T>();

  // Assert that T is a Sequence. Will trigger a static_assert with
  // and explanation if not.
  static_views::concepts::Sequence::check<T>();



.. _views:

Views
=========================================================================

The core building blocks of the library are *Views*. Just like the name
suggests, views let you have a look at your data, modify it if you want,
but never ever do they own the data. Views can change the way you see
your data without changing the data itself. That's the main idea of having
views in the first place.

Suppose you want to do something to the second half of your data. In the
world of iterators, you obtain an iterator to the middle, an iterator to
the end, and pass them to your algorithm of choice. Something along the
lines

.. code-block:: cpp

  find(begin(xs) + xs.size() / 2, end(xs), element);

In the world of views, however, we just create a view containing the
second half of the data. This can be accomplished by, for example,
"throwing away" the first half of the data:

.. code-block:: cpp

  find(drop(xs.size() / 2)(xs), element);

The two approaches are very similar so far. The disadvantage of using
iterators lies in inability to easily compose algorithms. For example,
suppose we add a requirement that only every third element is to be
examined. Using views, this problem is easily solved:

.. code-block:: cpp

  find(xs | drop(xs.size() / 2) | stride(3), element);

This syntax should be familiar to anyone who's written a shell script or
two in their lives:

+--------------------------+--------------------------------+
|         ``C++``          |             ``Bash``           |
+==========================+================================+
| :cpp:`xs | drop(123)`    | ``cat "xs" | tail -n +123``    |
+--------------------------+--------------------------------+

And with iterators:

.. code-block:: cpp

  // Assume stride returns a stride iterator.
  find(stride(begin(xs) + xs.size() / 2, 3), stride(end(xs), 3), element);

Notice how we essentially do the work twice. "Neither readable nor
maintainable this code is."

   find(begin(xs) + xs.size() / 2, end(xs), element);

The main problem with this approach is that you can't compose the
algorithms easily. For example, suppose we add a requirement that only
every third element it to be examined. Using views, this problem is easily
solved:

  If the pipe syntax seems too radical, it's OK. You're not required to
  use it.  *Piping* is equivalent to a function call, so the previous
  example could very well have been written as:

  .. code-block:: cpp

    find(stride(3)(drop(xs.size() / 2)(xs)), element);


.. _creating-views:

Creating Views
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

So you can see that views are easily composable, i.e. you create views of
views of views etc. We need a base case to terminate the recursion,
though. There's a special views for that -- :ref:`raw_view <raw-view>`. It
is the only view that can be constructed from a :ref:`sequence
<sequences>`. **All other views can only be constructed from views!**

.. code-block:: cpp

  using namespace static_views = boost::static_views;

  // A Sequence
  static constexpr unsigned maintainers_phone_number[] = {1, 2, 3, 4, 5, 6, 7, 8};
  // xs is a View
  constexpr auto xs = static_views::raw_view(maintainers_phone_number);

So you pass a reference to a Sequence to :ref:`raw_view <raw-view>` and
get a View back. Mathematically speaking

.. math::

   \mathtt{raw\_view}: \mathtt{Sequence} \to \mathtt{View} \,.

.. note::

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

After you've created a raw view of your data, you can move on to create
other views:

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

The library then automatically decides what to store. It's similar to
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


.. _using-views:

Using Views
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
      static constexpr auto extent() noexcept -> std::ptrdiff_t;

      // Iterators
      constexpr auto begin() const noexcept;
      constexpr auto end() const noexcept;
  };

The interface is very similar to an array. You obtain the number of
elements using the :cpp:`size()` member function. You then access elements
using :cpp:`operator[]`. For the case that you want to use Views with
algorithms that work with iterators, :cpp:`begin()` and :cpp:`end()`
member functions returning iterators are provided.

For starters, let's implement a straightforward linear search using views:

      // Return the element at the specified index. An exception is
      // thrown if the index is out of bounds.
      constexpr auto operator[](std::size_t);
      constexpr auto operator[](std::size_t) const;

  template <class View, class Predicate>
  constexpr auto find_first_i(View const& xs, Predicate&& p) -> std::size_t;

We accept a view and a predicate and return the smallest index :cpp:`i` for
which :cpp:`p(xs[i])` returns :cpp:`true`. To indicate that there's no
such index, we can just return :cpp:`xs.size()`.

.. code-block:: cpp

  template <class View, class Predicate>
  constexpr auto find_first_i(View const& xs, Predicate&& p) -> std::size_t
  {
      using boost::static_views::invoke;

      std::size_t const n = xs.size();
      std::size_t       i;
      for (i = 0; (i < n) && !invoke(p, xs[i]); ++i)
          ;
      return i;
  }

Notice the use of :cpp:`invoke`. It allows us to pass *any* callable
objects. So, for example, pointers to member data will just work.

.. note::

  Why not :cpp:`std::invoke`? First of all, :cpp:`std::invoke` is not part
  of ``C++14`` while the library is ``C++14``-compatible. Then,
  :cpp:`std::invoke` is not :cpp:`constexpr`! It's crazy and I really hope
  it will be fixed in ``C++20``, but for now we're stuck with hand-rolled
  implementations.

Although, the function above will work as expected, it is not really
consistent with how the rest of StaticViews is implemented. All free
"functions" are actually not functions at all. They are references to
functor objects. The advantage of such approach is that it significantly
simplifies passing functions to higher order functions (and :cpp:`sizeof`
empty :cpp:`struct` is 1 byte while :cpp:`sizeof` a function pointer is 4
or 8 bytes **;-)**). Just remember the last time you had a vector of
tuples and you wanted to create a vector of third elements of the tuples.
What we'd like to write is

.. code-block:: cpp

  std::transform(std::begin(source), std::end(source),
      std::back_inserter(destination), std::get<2>);

while what we have to write is

.. code-block:: cpp

  std::transform(std::begin(source), std::end(source),
      std::back_inserter(destination),
      [](auto&& x) -> decltype(auto) {
          return std::get<2>(std::forward<decltype(x)>(x));
      });

And we just hope that we don't forget that extra :cpp:`decltype(auto)` to
make sure we return a reference rather than a copy... I hope, it's clear
from this example that if we want to promote clear functional style of
programming in C++, functors are the way to go. So the StaticViews way of
implementing :cpp:`find_first_i` is

.. code-block:: cpp

  namespace detail {
    struct find_first_i_impl {
        template <class View, class Predicate>
        constexpr auto operator()(View const& xs, Predicate&& p) const
            -> std::size_t
        {
            using boost::static_views::invoke;

            std::size_t const n = xs.size();
            std::size_t       i = 0;
            for (; (i < n) && !invoke(p, xs[i]); ++i)
                ;
            return i;
        }
    };
  } // namespace detail

  BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::find_first_i_impl, find_first_i);

The :cpp:`BOOST_STATIC_VIEWS_INLINE_VARIABLE(type, name)` expands to
(almost) :cpp:`constexpr auto name = type{};`. So we still use the normal
function call syntax.


.. _advanced-usage:

Advanced Usage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are still a couple of issues with our implementation of the search.
The core ``C++`` philosophy is to provide abstractions with no performance
overhead. Although we have an :math:`\mathcal{O}(N)` solution, just like
we should, it is terribly inefficient. We call :cpp:`operator[]`, which is
"safe", i.e. it checks that :cpp:`i < xs.size()`. But we know that this
condition is satisfied, so why check twice? Specifically for this reason
Views have :cpp:`unsafe_at` member function which behaves just like
:cpp:`operator[]` except that it performs no bounds checking. So
:cpp:`xs[i]` :math:`\mapsto` :cpp:`xs.unsafe_at(i)`.

Then there are constrains. Not every type maybe passed as the first
argument. We want :cpp:`xs` to model the View concept. Just like you can
check whether some type is a Sequence, the library provides a way to check
whether a particular type :cpp:`T` models the View concept:

.. code-block:: cpp

  // Check whether T is a View
  constexpr bool T_is_a_View = boost::static_views::concepts::View::test<T>();
  // Assert the T is a View
  boost::static_views::concepts::View::check<T>();

So if we want to be SFINAE friendly, we can do something like:

.. code-block:: cpp

  template <class View, class Predicate, class Dummy = std::enable_if_t<
      boost::static_views::concepts::View::test<View>()>>
  constexpr auto operator()(View const& xs, Predicate&& p) const;

If we don't care about SFINAE, just insert the call to :cpp:`check` into
the function body. And finally, we might want to check for noexcept-ness.
So an acceptable version looks like this:

.. code-block:: cpp

  namespace detail {
    struct find_first_i_impl {
        template <class View, class Predicate>
        constexpr auto operator()(View const& xs, Predicate&& p) const
            noexcept(noexcept(boost::static_views::invoke(p, xs.unsafe_at(i))))
            -> std::size_t
        {
            using boost::static_views::invoke;
            boost::static_views::concepts::View::check<View>();

            std::size_t const n = xs.size();
            std::size_t       i = 0;
            for (; (i < n) && !invoke(p, xs.unsafe_at(i)); ++i)
                ;
            return i;
        }
    };
  } // namespace detail

  BOOST_STATIC_VIEWS_INLINE_VARIABLE(detail::find_first_i_impl, find_first_i);


.. _towards-static-map:

Towards static_map
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There is a slice view in the library which allows you to view an interval
of your data. So if :cpp:`xs` is some view, :cpp:`slice(b, e)(xs)` will
contain elements of :cpp:`xs` at indices :math:`i \in \{\mathtt{b},
\mathtt{b} + 1, \dots, \mathtt{e} - 1\}`:

.. code-block:: cpp

  static constexpr int data[] = { 1, 1, 8, 1, 1,
                                  2, 8, 2, 2, 2,
                                  3, 3, 3, 3, 8,
                                  4, 4, 4, 4, 4 };
  static constexpr auto xs = boost::static_views::raw_view(data);
  constexpr std::size_t row_size = 5;

  #if defined(__cpp_constexpr) && __cpp_constexpr >= 201603
  constexpr auto is_8 = [](int const x) { return x == 8 };
  #else
  struct is_8_impl {
      constexpr auto operator()(int const x) const noexcept
      {
          return x == 8;
      }
  };
  constexpr is_8_impl is_8{};
  #endif

  // We can now search for 8 is a certain row
  constexpr row_3_has_8 = find_first_i(
      boost::static_views::slice(2 * row_size, 3 * row_size)(xs), is_8)
      != row_size;

This is very similar to how a bucket-based hash table performs a lookup,
right? It computes the hash, jumps to a specific bucket and does a linear
search there. How about we implement it?

Notice how we didn't need the :cpp:`is_8` function until the very lookup.
This suggests that we can separate the tasks:

* A special view, called :cpp:`hashed` view takes care of the hashing. Its
  :cpp:`operator[]` returns a :cpp:`slice` view of the data containing all
  elements with the specified hash.

* Hash table, called :cpp:`static_map` in the library, takes care of
  comparing the elements so that we can search for a particular key.

Data is not always ordered nicely as in the previous example. Usually,
elements with the same hash are all over the place. We need a way to
change the order without touching the data. There's a view for that. It's
called the :cpp:`through` view, and it just uses an array of indices to
define the order:

.. code-block:: cpp

  using namespace boost::static_views;

  // Suppose we have some data that's nor ordered the way we want it to be.
  static constexpr int data[] = {5, 2, 1, 3, 4};
  // We create an array of indices that defines the new ordering.
  static constexpr std::size_t indices = {2, 1, 3, 4, 0};

  constexpr auto correctly_ordered = through(raw_view(indices))(raw_view(data));

  std::copy(correctly_ordered.begin(), correctly_ordered.end(),
      std::ostream_iterator<int>{std::cout, ", "});
  // Prints '1, 2, 3, 4, 5, '

So now even if the data is not ordered correctly, :cpp:`hashed` view can
reorder elements into groups with the same hash. :cpp:`operator[]` can
then give us all the elements that correspond to a particular hash.

:cpp:`static_map` then only needs to call our :cpp:`find_first_i` function
to search for an element. Except for some minor details this is the way
things are implemented in the library.


.. _static-map-tutorial:

Static Map
==========================================================================

So we've seen how one could implement a hash table using StaticViews. But
why would we need one?

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
