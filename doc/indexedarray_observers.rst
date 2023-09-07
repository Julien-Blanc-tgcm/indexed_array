.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Indexed_array observers
=======================

std::array interface
--------------------

.. _indexed_array_empty:

.. cpp:function:: constexpr bool indexed_array::empty() const

    Returns true if the container is empty, ie the array size is 0. Otherwise,
    the array is not empty.


.. _indexed_array_size:

.. cpp:function:: constexpr std::size_t indexed_array::size() const

    Returns the size of the array, which is a compile-time constant.

.. _indexed_array_max_size:

.. cpp:function:: constexpr std::size_t indexed_array::max_size() const

    Returns the maximum size of the array, which is always the same as its size.

.. _indexed_array_fill:

.. cpp:function:: void fill(value_type const& value)

    Fills the array with value, ie, replace every element in the array with
    value.

.. _indexed_array_swap:

.. cpp:function:: void swap(indexed_array& other)

    Exchanges the array contents, as per std::array::swap.

Extensions
----------

.. _indexed_array_in_range:

.. cpp:function:: static constexpr bool indexed_array::in_range(index)

   static constexpr method that returns ``true`` if the argument is a
   valid index value for this array, ie calling ``operator[value]`` is
   well defined behavior. Returns ``false`` otherwise.

.. _indexed_array_is_o1:

.. cpp:member:: static constexpr bool indexed_array::is_o1

    static member that describe if access to this indexed array is guaranted to be O(1). This
    information can be used for static asserts when O(1) access is a requirement, or for 
    performance improvements by selecting different algorithm in function of this property.

