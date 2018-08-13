.. _wrapper-hpp:

<boost/static_views/wrapper.hpp>
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: cpp

   // boost/static_views/wrapper.hpp

   template <class T>
   class wrapper {
     public:
       using value_type      = std::remove_reference_t<T>;
       using reference       = value_type&;
       using const_reference = value_type const&;

       template <class U>
       constexpr wrapper(U&&) noexcept(whenever possible)
           requires T is not a reference
                 && Constructible<value_type, U&&>;

       constexpr wrapper(reference) noexcept
           requires T is a reference;

       constexpr wrapper(wrapper const&) noexcept(whenever possible)
           requires T is a reference
                 || CopyConstructible<value_type>;

       constexpr wrapper(wrapper &&) noexcept(whenever possible)
           requires T is a reference
                 || MoveConstructible<value_type>;

       constexpr wrapper& operator=(wrapper const&) noexcept(whenever possible)
           requires T is a reference
                 || MoveAssignable<value_type>;

       constexpr wrapper& operator=(wrapper &&) noexcept(whenever possible)
           requires T is a reference
                 || MoveAssignable<value_type>;

       constexpr auto get() const& noexcept -> const_reference;
       constexpr auto get() &      noexcept -> reference;
       constexpr auto get() &&     noexcept(whenever possible) -> value_type
           requires T is a reference
                 || MoveConstructible<value_type>;

       constexpr explicit operator reference() & noexcept;
       constexpr explicit operator const_reference() const& noexcept;
       constexpr explicit operator value_type() && noexcept(whenever possible)
           requires T is a reference
                 || MoveConstructible<value_type>;
   };

   // exposition only
   constexpr auto make_wrapper = [] (auto&& x) {
       return wrapper<decltype(x)>{ std::forward<decltype(x)>(x)}
   };


.. cpp:class:: template <class T> \
               boost::static_views::wrapper

   .. cpp:type:: value_type = std::remove_reference_t<T>

   .. cpp:type:: reference = std::add_reference_t<value_type>

   .. cpp:type:: const_reference = std::add_reference_t<std::add_const_t<value_type>>

   .. cpp:function:: template <class U> \
                     constexpr wrapper(U&& x)

      .. note::

         This function participates in overload resolution only if
         :cpp:expr:`T` is **not a reference type** and
         :cpp:expr:`value_type` is constructible from :cpp:expr:`U`.

      Creates a new wrapper. ``x`` is perfect forwarded to the constructor
      of :cpp:expr:`value_type`. This function is
      :cpp:expr:`noexcept(true)` if and only if the corresponding
      constructor of :cpp:expr:`value_type` is :cpp:expr:`noexcept(true)`.

   .. cpp:function:: constexpr wrapper(reference) noexcept

      .. note::

         This function participates in overload resolution only if
         ``T`` **is a reference type**.

      Creates a new wrapper.

   .. cpp:function:: wrapper(wrapper const&) = default

   .. cpp:function:: wrapper(wrapper &&) = default

   .. cpp:function:: wrapper& operator=(wrapper const&) = default

   .. cpp:function:: wrapper& operator=(wrapper &&) = default

   .. cpp:function:: constexpr reference get() & noexcept

      Returns a reference to the wrapped object.

   .. cpp:function:: constexpr const_reference get() const& noexcept

      Returns a constant reference to the wrapped object.

   .. cpp:function:: constexpr value_type get() &&

      Returns the stored object. This function is
      :cpp:expr:`noexcept(true)` if and only if the move constructor of
      :cpp:expr:`value_type` is :cpp:expr:`noexcept(true)`.

      .. note::

         This function participates in overload resolution only if
         ``T`` is **not a reference type** and :cpp:expr:`value_type`
         models the `MoveConstructible`_ concept.

   .. cpp:function:: constexpr explicit operator reference() & noexcept

      Conversion to a reference to the wrapped object.

   .. cpp:function:: constexpr explicit operator const_reference() const& noexcept

      Conversion to a constant reference to the wrapped object.

   .. cpp:function:: constexpr explicit operator value_type() &&

      Conversion to the wrapped object. This function is
      :cpp:expr:`noexcept(true)` if and only if the move constructor of
      :cpp:expr:`value_type` is :cpp:expr:`noexcept(true)`.

      .. note::

         This function participates in overload resolution only if
         ``T`` is **not a reference type** and :cpp:expr:`value_type`
         models the `MoveConstructible`_ concept.


.. cpp:var:: constexpr auto boost::static_views::make_wrapper

   .. math::

      \mathtt{make\_wrapper}: \mathtt{U} \to \mathtt{Wrapper}

   Creates a :cpp:class:`wrapper` of an lvalue reference when ``U = T&``
   for some type ``T`` or an rvalue reference when ``U == T&&``. If ``U =
   T&&``, ``T`` is required to model the `MoveConstructible`_ concept.



.. |reference_wrapper| replace:: ``std::reference_wrapper``
.. _reference_wrapper: http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper

.. _MoveConstructible: http://en.cppreference.com/w/cpp/concept/MoveConstructible
