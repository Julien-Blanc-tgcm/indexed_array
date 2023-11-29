.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Indexed_array iterators
=======================

``indexed_array`` provides random access iterators, with O(1) complexity. These iterators are
valid as long as the object exists, no operation on ``indexed_array`` does iterator
invalidation.

begin
-----
.. _indexed_array_begin:

These functions return an iterator to the beginning of the indexed_array. If the array is empty,
the iterator will be equal to ``end()``.

.. cpp:function:: constexpr iterator indexed_array::begin()
.. cpp:function:: constexpr const_iterator indexed_array::begin() const
.. cpp:function:: constexpr const_iterator indexed_array::cbegin() const

.. _indexed_array_end:

end
---

These functions return an iterator which is immediately after the last element. It is undefined
behaviour to try to dereference such an iterator.

.. cpp:function:: constexpr iterator indexed_array::end()
.. cpp:function:: constexpr const_iterator indexed_array::end() const
.. cpp:function:: constexpr const_iterator indexed_array::cend() const

.. _indexed_array_rbegin:

rbegin
------

These functions return a reverse iterator to the first element of the reversed indexed_array (which is
the last element of the array). If the array is empty, then it is equal to ``rend()``.

See https://en.cppreference.com/w/cpp/container/array/rbegin for a detailed explanation on reverse iteration.

.. cpp:function:: constexpr reverse_iterator indexed_array::rbegin()
.. cpp:function:: constexpr const_reverse_iterator indexed_array::rbegin() const
.. cpp:function:: constexpr const_reverse_iterator indexed_array::crbegin() const

.. _indexed_array_rend:

rend
----

These functions return a reverse iterator which is past the last element of the reversed indexed_array. It
is undefined behaviour to try to dereference such an iterator.

.. cpp:function:: constexpr reverse_iterator indexed_array::rend()
.. cpp:function:: constexpr const_reverse_iterator indexed_array::rend() const
.. cpp:function:: constexpr const_reverse_iterator indexed_array::crend() const

Multidimensional arrays
-----------------------

.. warning::

    Special care should be taken when using iterators and multidimensional arrays. Iterators
    always iterate through the whole array, and not through the highest-rank dimension.
