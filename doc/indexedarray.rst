.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Indexed_array
=============

.. code:: cpp

   template <typename Value, typename Indexer>
   /* */ indexed_array

Container that encapsulate a fixed size array, whose elements are
accessible using a custom indexing mechanism instead of a zero-based
integer indexing. It is an aggregate type and intend to be a drop-in
replacement for ``std::array``. The storage is guaranted to be
contiguous, and can be accessed as a C-array.

Constructor
-----------

The following constructors are explicitly defaulted:

Default constructor. May result in the values in the array not being initialized:

.. doxygenfunction:: jbc::indexed_array::detail::indexed_array::indexed_array()

Copy / move constructors:

.. doxygenfunction:: jbc::indexed_array::detail::indexed_array::indexed_array(indexed_array<Value, Indexer> const& other)

.. doxygenfunction:: jbc::indexed_array::detail::indexed_array::indexed_array(indexed_array<Value, Indexer> &&)

Construction from a list of values is possible as well:

.. cpp:function:: template<typename... Args> jbc::indexed_array::detail::indexed_array::indexed_array(Args&&... args)

    Construction with a list of values. Roughly equivalent to aggregate initialization of ``std::array``
    
.. cpp:function:: template<typename... Args> jbc::indexed_array::detail::indexed_array::indexed_array(SafeArg<Args, Value>&&... list)

    Construction from a list of safe args. This is equivalent to aggregate initialization, but additional checks are done at compile time:
    
    * the number of arguments must match with the array size
    * the explicit index of each argument must match the order of the indexer


Member functions
----------------

.. doxygenclass:: jbc::indexed_array::detail::indexed_array
    :members-only:
    :members: at, operator[], begin, end, rbegin, rend, cbegin, cend, crbegin, crend, size, fill, swap
    :undoc-members:

.. code:: cpp

   at(/* index */)
   operator[/* index */] noexcept

..

   Access to an element of the array. ``at`` is range-checked and throws
   ``std::out_of_range`` if the value is incorrect. In case of a
   multidimensional array, ``operator[]`` signature is
   ``(std::tuple<index1, index2, ...>)``, whereas ``at`` signature is
   ``(index1, index2, ...)``.

.. code:: cpp

   front()
   back()
   data()
   begin()
   cbegin()
   end()
   cend()
   rbegin()
   crbegin()
   rend()
   crend()
   empty()
   size()
   max_size()
   fill()
   swap()

..

   Same semantic and guarantees as ``std::array``.

.. code:: cpp

   is_o1

..

   static constexpr property that is ``true`` if the array element
   access is ``O(1)`` and false otherwise

.. code:: cpp

   in_range(/* index */)

..

   static constexpr method that returns ``true`` if the argument is a
   valid index value for this array, ie calling ``operator[value]`` is
   well defined behavior. Returns ``false`` otherwise.

.. code:: cpp

   slice(/* 1-dimension index */) noexcept
   slice_at(/* 1-dimension index */)

..

   Returns a slice of the array, at the given index. For
   multidimensional arrays of extent ``n``, it returns an
   ``indexed_span`` (see below) of extent ``n-1``. For single dimension
   array, it returns the element at the given index. ``slice`` does not
   do any bound checking, ``slice_at`` throws ``std::out_of_range`` on
   error.

.. code:: cpp

   indexed_array<int, Color, Material, index_range<1, 10>> foo;
   auto reds = foo.slice(Color::Red); // reds is indexed_span<int, Material, index_range<1, 10>>


