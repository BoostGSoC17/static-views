.. _view-concept:

View
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Views are the core building blocks of the library. The are like non-owning
containers. Here's how one can define the View concept using `Concepts TS
<http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4674.pdf>`_
syntax:

.. code-block:: cpp

  template <class T>
  concept bool MoveConstructible = std::is_move_constructible<T>::value;

  template <class T>
  concept bool View()
  {
      return requires (T xs, std::size_t i)  {
          requires MoveConstructible<T>;
          { T::extent(); } noexcept -> std::ptrdiff_t;
          { xs.size(); } noexcept -> std::size_t;
          { xs.unsafe_at(i); };
          { xs[i]; };
      };
  }

Seeing as the library only requires ``C++14``, a slightly different
realisation had to be used.

.. doxygenstruct:: boost::static_views::concepts::View
  :project: static-views
