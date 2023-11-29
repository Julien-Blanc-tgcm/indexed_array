.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Indexed_array data access
=========================

Members from std::array
-----------------------

These members have been designed to provide perfect interoperability with ``std::array``, they have the same semantics
and guarantees. The only difference is in the type of the index parameter.

.. _indexed_array_at:
.. _indexed_array_bracket:

.. cpp:function:: constexpr reference indexed_array::at(index)
.. cpp:function:: constexpr const_reference indexed_array::at(index) const
.. cpp:function:: constexpr reference indexed_array::operator[](index)
.. cpp:function:: constexpr const_reference indexed_array::operator[](index) const

    Access to an element of the array. ``at`` is range-checked and throws
    ``std::out_of_range`` if the value is incorrect. In case of a
    multidimensional array, ``operator[]`` signature is
    ``(std::tuple<index1, index2, ...>)``, whereas ``at`` signature is
    ``(index1, index2, ...)``. 

    ``index`` may be anything that is implicitly convertible to the actual index
    used by the indexer.

.. note::

    When compiling in C++23 mode, ``operator[]`` can also be used with the following
    signature: ``operator[index1, index2, ...]``.

.. _indexed_array_front:

.. cpp:function:: constexpr reference indexed_array::front()
.. cpp:function:: constexpr const_reference indexed_array::front() const

    Access to the first element of the array, via a reference (or a const reference).

.. _indexed_array_back:
   
.. cpp:function:: constexpr reference indexed_array::back()
.. cpp:function:: constexpr const_reference indexed_array::back() const

    Access to the last element of the array.

.. note::

    Calling ``front`` or ``back`` on an empty indexed_array is undefined behavior.

.. _indexed_array_data:

.. cpp:function:: constexpr value_type* indexed_array::data() noexcept
.. cpp:function:: constexpr value_type const* indexed_array::data() const noexcept

    Access to the underlying data, via raw pointer access. Like ``std::array``, the pointer is not derefenceable
    if ``empty()``, but the range [``data()``, ``data() + size()``] is always valid.

Additional members
------------------

.. _indexed_array_slice:
.. cpp:function:: indexed_span indexed_array::slice(single_dimension_index) noexcept
.. _indexed_array_slice_at:
.. cpp:function:: indexed_span indexed_array::slice_at(single_dimension_index) noexcept

    Returns a slice of the array, at the given index. For
    multidimensional arrays of extent ``n``, it returns an
    ``indexed_span`` (see below) of extent ``n-1``. For single dimension
    array, it returns the element at the given index. ``slice`` does not
    do any bound checking, ``slice_at`` throws ``std::out_of_range`` on
    error.

.. code-block:: cpp
    :caption: slice sample

    indexed_array<int, Color, Material, index_range<1, 10>> foo;
    auto reds = foo.slice(Color::Red); // reds is indexed_span<int, Material, index_range<1, 10>>
