
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
The first one is chosen by default. To use the second instead, define the
``BOOST_STATIC_VIEWS_USE_BOOST`` macro before the inclusion of the
headers. Static Views will then use `Boost.Config`_ macros instead of
reinventing the wheel.


.. _sequences-and-views:

Sequences and Views
==========================================================================

Very rarely do we deal with compile-time data of infinite size. Usually,
when we choose on hard-coding some data into the program, we store it as a
static ``C``-style array or tuple. These data structures not only have
finite size which is part of the type, they also provide random access to
elements. Such data structures are called *Sequences* and are the ones
that Static Views is meant to be used with. ``C``-style arrays and tuples
aren't the only types that work out of the box. :cpp:`std::array` and 
:cpp:`gsl::span`, for example, are supported too. If you're unsure whether
your own sequence type :cpp:`T` will work, just ask:

.. code-block:: cpp

  static_assert(static_views::concepts::Sequence::test<T>(),
      "Nope, doesn't work out of the box.");

As far as the library is concerned, sequences own the data and provide
access to it. We don't work with sequences explicitly though. We use them
through *views*. Views are non-owning containers that provide random
access to elements. There's one special view called :cpp:`raw_view` which
wraps a sequence and can be constructed from a reference to it:

.. code-block:: cpp
  
  static constexpr unsigned maintainers_phone_number[] = {1, 2, 3, 4, 5, 6, 7, 8};
  constexpr auto xs = static_views::raw_view(maintainers_phone_number);

.. note::

  At this point it may be worthwhile to note that although we call
  :cpp:`raw_view` as a function, it's not a function. Rather, it's a
  functor, i.e. a class with an overloaded :cpp:`operator()`. This
  approach is not limited to :cpp:`raw_view`. *All* functions in the
  library are actually functors. The advantage of such an approach is that
  you can pass them to higher order functions without worrying about
  templates.

  ``@advanced users``: No, these "functions" won't cause ODR violations.
  The method described in `n4381
  <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html>`_
  is used to avoid that.


**All other views can only be constructed from views!**.

.. code-block:: cpp

  static constexpr char data[] = {'h', 'e', 'l', 'l', 'o'};
  // drop(n) drops the first n elements
  constexpr auto view_bad = static_views::drop(2)(data); // Error! 
  constexpr auto view_good = static_views::drop(2)(      // OK
      static_views::raw_view(data));                     //

It's not a big deal if you forget this rule though. :cpp:`static_assert`'s
will tell you exactly what you've done wrong. For example, in the above
case Clang produces the following error message:

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

Notice how in the example above an *rvalue* is passed to :cpp:`drop`. This
is not the only supported use case. *Lvalues* can be passed, too:

.. code-block:: cpp

  static constexpr auto data = std::make_tuple(1, 2, 3);
  static constexpr auto xs   = static_views::raw_view(data);
  constexpr auto view = static_views::drop(2)(xs);

There's one important thing to note -- the use of :cpp:`static`. "Normal"
:cpp:`constexpr` variables live only in the mind of the compiler. You
simply **can't take their address inside constant expressions** as
specified in section 5.20 of the Standard:

  | if the value is of pointer type, it contains the
  | address of an object with static storage duration,
  | the address past the end of such an object (5.7),
  | the address of a function, or a null pointer value

There is, unfortunately, no way issue :cpp:`static_assert`'s on such
errors. Compilers are quite helpful though:

.. code-block:: none

  test.cpp:16:20: error: constexpr variable 'view' must be initialized by a constant expression
      constexpr auto view = static_views::drop(2)(xs);
                     ^      ~~~~~~~~~~~~~~~~~~~~~~~~~
  test.cpp:16:20: note: pointer to 'xs' is not a constant expression
  test.cpp:15:20: note: declared here
      constexpr auto xs   = static_views::raw_view(data);

Says Clang, if you forget to make :cpp:`xs` static.

Another interesting thing to note is that the :cpp:`drop` functor is
curried, i.e. its signature is

.. math::

  \mathtt{drop}: \mathbb{N} \to \mathtt{View} \to \mathtt{View}

rather than

.. math::

  \mathtt{drop}: \mathbb{N} \times \mathtt{View} \to \mathtt{View} \,.

All view-creating functions are curried. In other words, all functions
that allow you to create views can be described by the following
signature:

.. math::

  f: \mathtt{arg_1} \times \mathtt{arg_2} \times \dots 
      \times \mathtt{arg_N} \to \mathtt{View} \to \mathtt{View} \,.

This allows people who love Bash-like pipes to use an alternative syntax
to stack views:

.. code-block:: cpp

  static constexpr int data[] = {0, 0, 0, 1, 0, 2, 0, 3};
  constexpr auto xs = static_views::raw_view(data)
                    | static_views::drop(3)
                    | static_views::stride(2); // xs is now {1, 2, 3}

With curried functions there's no need to come up with two names for the
same view (think :cpp:`copy` and :cpp:`copied`, :cpp:`filter` and
:cpp:`filtered`, etc. in Boost.Range_).



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

:cpp:`static_map` is a hash map, so lookups are :math:`\mathcal{O}(1)`,
both at compile-time and at run-time. At most :cpp:`fruits.bucket_size()`
comparisons of keys are performed, where the bucket size is ``2`` by
default, but can be set explicitly if you feel your hash function isn't
that good. Custom comparison and hash functions are supported, too. The
feature that distinguishes :cpp:`static_map` from a common hash map is the
fact that you choose which field it used as a key and which as a value. It
is thus not required for :cpp:`data_type` to only have two fields. You can
use arbitrary types, as long as they have :cpp:`constexpr` constructors.


Bitmaps (YET TO BE UPDATED)
==========================================================================

Suppose we have a bitmap (24-bit RGB) icon saved as a ``constexpr`` array of
chars::
  
  static constexpr unsigned char smiley[] = {
    0x22, 0x22, 0x00, 0x25, 0x25, ...
    ...
  };

Now if a need comes up to convert it to 32-bit RGBA, StaticViews comes in
handy. The only thing we have to do by ourselves is to write a function to
convert ``unsigned char[3]`` (RGB) into ``uint32_t`` (RGBA). Let's do it
for little endian::

  [](auto&& xs) noexcept -> std::uint32_t
  {
      return (static_cast<std::uint32_t>( 0xFF) << 24)
           | (static_cast<std::uint32_t>(xs[2]) << 16)
           | (static_cast<std::uint32_t>(xs[1]) <<  8)
           | (static_cast<std::uint32_t>(xs[0]) <<  0);
  }

The big endian case is left as an exercise for the reader. If we're now
given a view of ``char`` representing 24-bit colours, we can easily create
a view of ``uint32_t``::

  template <class V>
  constexpr auto rgb_to_rgba(V&& rgb)
  {
    if (rgb.size() % 3 != 0)
      throw invalid_format_error{};

    return rgb | chunk(3) 
               | map([](auto&& xs) noexcept -> std::uint32_t {
                   return (static_cast<std::uint32_t>( 0xFF) << 24)
                        | (static_cast<std::uint32_t>(xs[2]) << 16)
                        | (static_cast<std::uint32_t>(xs[1]) <<  8)
                        | (static_cast<std::uint32_t>(xs[0]) <<  0);
                 });
  }

What ``chunk(N)`` does is divide the source view into contiguous chunks of
N elements (the last chunk may be shorter). If we want to do the inverse::

  template <class V>
  constexpr auto rgba_to_rgb(V&& rgba) noexcept
  {
    return rgba | map([](auto&& x) noexcept {
                    return raw_view(std::array<unsigned char, 3>{
                      (x & 0x000000FF), (x & 0x0000FF00), (x & 0x00FF0000)
                    });
                  });
                | concat();
  }

We convert each integer into a view of chars and the flatten the view of
views (think monadic bind for lists).

Conversions to a 1-bit bitmaps is a tiny bit trickier. Apart from
conversion from 24-bit RGB to a ``bool``, we also need to convert 8
``bool``'s into a char::

  [](auto&& bs)
  {
    unsigned char c = 0x00;
    for (std::size_t i = 0; i < bs.size(); ++i) {
      c |= static_cast<unsigned char>(bs[i]) 
           << (bs.size() - 1 - i);
    }
    return c;
  }

Assume that the ``pixel_rgb_to_bw`` takes a view
of 3 ``char``'s and returns a ``bool`` (true -- white, false -- black).
Then the complete conversion function becomes::

  template <class V>
  constexpr auto rgb_to_bw(V&& rgb)
  {
    if (rgb.size() % 3 != 0)
      throw invalid_format_error{};

    return rgb | chunk(3)
               | map(pixel_rgb_to_bw)
               | chunk(8)
               | map([](auto&& bs) noexcept {
                    unsigned char c = 0x00;
                    for (std::size_t i = 0; i < bs.size(); ++i) {
                      c |= static_cast<unsigned char>(bs[i]) 
                           << (bs.size() - 1 - i);
                    }
                    return c;
                 });
  }

.. _Boost.Config: http://www.boost.org/doc/libs/develop/libs/config/doc/html/index.html
.. _Boost.Hana: http://www.boost.org/doc/libs/develop/libs/hana/doc/html/index.html
.. _Boost.Fusion: http://www.boost.org/doc/libs/develop/libs/fusion/doc/html/index.html
.. _Boost.Range: http://www.boost.org/doc/libs/develop/libs/range/doc/html/index.html
.. _Boost.Mp11: http://www.boost.org/doc/libs/develop/libs/mp11/doc/html/index.html
.. _Ranges TS: https://ericniebler.github.io/range-v3
