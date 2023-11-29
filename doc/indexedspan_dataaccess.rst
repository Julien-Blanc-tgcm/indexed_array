.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

indexed_span data access
========================

Data access
-----------

.. _indexed_span_at:
.. _indexed_span_bracket:
.. cpp:function:: constexpr reference indexed_span::at(Index i) const
.. cpp:function:: constexpr reference indexed_span::operator[](Index i) const
    
    Element access. ``at`` is range-checked, and throws
    ``std::out_of_range`` on error. In case of multidimensional span,
    ``operator[]`` signature is ``std::tuple<Index1, Index2, ...>``
    whereas ``at`` signature is ``at(Index1, Index2, ...)``. If using
    ``C++23``, ``operator[]`` is also available as ``[Index1, Index2, ...]```

   :param i: The index. Anything that is implicitly convertible to the Index can
       be used.
   :returns: A reference to the item at index i

.. _indexed_span_data:
.. cpp:function:: constexpr pointer indexed_span::data() const

    Gives access to the underlying storage.

    :returns: A pointer to the start of the sequence of elements

.. _indexed_span_front:
.. cpp:function:: constexpr reference indexed_span::front() const
    
    :returns: The first element of the span. This is the same as ``*begin()``.

.. _indexed_span_back:
.. cpp:function:: constexpr reference indexed_span::back() const

    :returns: The last element of the span. This is the same as ``*rbegin()``.

Subrank access
--------------

.. _indexed_span_slice:
.. cpp:function:: constexpr auto indexed_span::slice(oneDimensionIndex index) const noexcept
.. cpp:function:: constexpr auto indexed_span::slice_at(oneDimensionIndex index) const noexcept

    Both functions returns a slice of the span, at the given index. For multidimensional
	spans of rank ``n``, it returns an ``indexed_span`` of rank
	``n-1``. For single dimension span, it returns the element at the
	given index. ``slice`` does not do any bound checking, ``slice_at``
	throws ``std::out_of_range`` on error.

    :param index: The index of the slice. It must be an index of the type of the higher-level
        rank of the indexer.
    :returns: An ``indexed_span`` of lower rank, a view of the data at index ``index``

.. note::
    There is no ``first``/``last``/``subspan`` methods. They do not fit
    well with the concept of custom indexing, because the return type shall
    be known at compile time, as it would depend on the parameters.

