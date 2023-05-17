.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

indexed_span
============

.. code:: cpp

   template<typename Value, typename Indexer>
   /* */ indexed_span

Span-like class which allows accessing elements through a custom
indexer. ``indexed_span`` does not own the data, and its size is always
known at compile time (it always cover the full ``Indexer`` range). It
is multidimensional if the Indexer is. It is related to
``std::span<T, std::static_extent>`` and has a similar interface.

The primary use case for ``indexed_span`` is that it is the type returned
by a multidimensional ``indexed_array`` when accessing a lower-rank
dimension via the ``slice()`` method.

Constructors
------------

.. code:: cpp

   indexed_span() = delete // empty constructor is deleted
   // copy/move operators defaulted
   constexpr indexed_span(indexed_span const&)
   constexpr indexed_span(indexed_span &&)
   constexpr operator=(indexed_span const&)
   constexpr operator=(indexed_span &&)
   // constructor from a Value*
   constexpr indexed_span(Value* begin);

..
   Constructs a new ``indexed_span`` over the range
   ``[begin, begin + Indexer::size]``, which must be a valid contiguous
   memory segment containing objects of type ``Value``. Behaviour is
   undefined otherwise.

   Since the extent of the span depends on the indexer, the constructor
   from a raw data pointer does not takes a size parameter.

Member functions
----------------

Data access
^^^^^^^^^^^

.. cpp:function:: reference at(Index i) const
.. cpp:function:: reference operator[](Index i) const noexcept

   :returns: A reference to the item at index i

   Element access. ``at`` is range-checked, and throws
   ``std::out_of_range`` on error. In case of multidimensional span,
   ``operator[]`` signature is ``std::tuple<Index1, Index2, ...>``
   whereas ``at`` signature is ``at(Index1, Index2, ...)``. If using
   ``C++23``, ``operator[]`` is also available as ``[Index1, Index2, ...]```

.. cpp:function:: Value* data() const

    :returns: A pointer to the start of the sequence of elements

    Gives access to the underlying storage.

.. cpp:function:: constexpr Value& front() const

    Returns the first element of the span. This is the same as ``*begin()``.

.. cpp:function:: constexpr Value& back() const

    Returns the last element of the span. This is the same as ``*rbegin()``.

Observers
^^^^^^^^^

.. cpp:function:: constexpr bool empty() const

    Returns true if the sequence of elements is empty.

.. cpp:function:: constexpr size_t size() const

    Returns the number of elements in the span

Iteration
^^^^^^^^^

.. cpp:function:: constexpr iterator begin() const
.. cpp:function:: constexpr iterator end() const
.. cpp:function:: constexpr reverse_iterator rbegin() const
.. cpp:function:: constexpr reverse_iterator rend() const

   Standard iteration. Same semantic as the corresponding methods in ``std::span``.

Subrank accessing
^^^^^^^^^^^^^^^^^

.. cpp:function:: slice(oneDimensionIndex index) noexcept

.. cpp:function:: slice_at(oneDimensionIndex)

    :arg: index The index of the slice. It must be an index of the type of the higher-level
        rank of the indexer.
    :returns: An ``indexed_span`` of lower rank, a view of the data at index ``index``

    Both functions returns a slice of the span, at the given index. For multidimensional
    spans of rank ``n``, it returns an ``indexed_span`` of rank
    ``n-1``. For single dimension span, it returns the element at the
    given index. ``slice`` does not do any bound checking, ``slice_at``
    throws ``std::out_of_range`` on error.

Note that there is no ``first``/``last``/``subspan`` methods. They do not fit
well with the concept of custom indexing, because the return type shall
be known at compile time, as it would depend on the parameters.
