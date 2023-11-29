.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

indexed_span
============

.. toctree::
    :hidden:

    indexedspan_constructors
    indexedspan_dataaccess
    indexedspan_iterators
    indexedspan_observers

.. cpp:class:: template<typename Value, typename Indexer> indexed_span

    Span-like class which allows accessing elements through a custom
    indexer. ``indexed_span`` does not own the data, and its size is always
    known at compile time (it always cover the full ``Indexer`` range). It
    is multidimensional if the Indexer is. It is related to
    ``std::span<T, std::static_extent>`` and has a similar interface.

The primary use case for ``indexed_span`` is that it is the type returned
by a multidimensional ``indexed_array`` when accessing a lower-rank
dimension via the ``slice()`` method.

Construction and destruction
----------------------------

+---------------------------------------------------------------------+--------------------------------+
| *Constructors*                                                      |                                |
+=====================================================================+================================+
| ``indexed_span() = delete``                                         | default constructor is deleted |
+---------------------------------------------------------------------+--------------------------------+
| | :ref:`indexed_span(indexed_span const&) <indexed_span_ctr_def>`   | copy/move operators defaulted  |
| | :ref:`operator=(indexed_span const&) <indexed_span_ctr_def>`      |                                |
+---------------------------------------------------------------------+--------------------------------+
| :ref:`indexed_span(Value*) <indexed_span_ctr_val>`                  | constructor from a ``Value*``  |
+---------------------------------------------------------------------+--------------------------------+

Member functions
----------------

Data access
^^^^^^^^^^^

+---------------------------------------------------------------------+--------------------------------+
| *Functions always present*                                          |                                |
+=====================================================================+================================+
| :ref:`indexed_span::at(index) const <indexed_span_at>`              | Element access (range-checked) |
+---------------------------------------------------------------------+--------------------------------+
| :ref:`indexed_span::operator[](index) const <indexed_span_bracket>` | Element access (unchecked)     |
+---------------------------------------------------------------------+--------------------------------+
| :ref:`indexed_span::data() const <indexed_span_data>`               | Access to raw storage          |
+---------------------------------------------------------------------+--------------------------------+
| :ref:`indexed_span::front() const <indexed_span_front>`             | Access to first element        |
+---------------------------------------------------------------------+--------------------------------+
| :ref:`indexed_span::back() const <indexed_span_back>`               | Access to last element         |
+---------------------------------------------------------------------+--------------------------------+
| *Multidimensional specific*                                         |                                |
+---------------------------------------------------------------------+--------------------------------+
| | :ref:`indexed_span::slice(1d-index) const <indexed_span_slice>`   | Access to subrank              |
| | :ref:`indexed_span::slice_at(1d-index) const <indexed_span_slice>`|                                |
+---------------------------------------------------------------------+--------------------------------+

Observers
^^^^^^^^^

+------------------------------------------------------------------+-----------------------------------+
| :ref:`indexed_span::empty() const <indexed_span_empty>`          | Checks sequence for emptiness     |
+------------------------------------------------------------------+-----------------------------------+
| :ref:`indexed_span::size() const <indexed_span_size>`            | Returns the size of the sequence  |
+------------------------------------------------------------------+-----------------------------------+

Iteration
^^^^^^^^^

+---------------------------------------------------------------------+-------------------------------+
| | :ref:`indexed_span::begin() <indexed_span_begin>`                 | Iterator to start of sequence |
| | :ref:`indexed_span::cbegin() <indexed_span_begin>`                |                               |
+---------------------------------------------------------------------+-------------------------------+
| | :ref:`indexed_span::end() <indexed_span_end>`                     | Iterator past the end of      |
| | :ref:`indexed_span::cend() <indexed_span_end>`                    | the sequence                  |
+---------------------------------------------------------------------+-------------------------------+
| | :ref:`indexed_span::rbegin() <indexed_span_rbegin>`               | Iterator to start of reverse  |
| | :ref:`indexed_span::crbegin() <indexed_span_rbegin>`              | sequence                      |
+---------------------------------------------------------------------+-------------------------------+
| | :ref:`indexed_span::rend() <indexed_span_rend>`                   | Iterator past the end of      |
| | :ref:`indexed_span::crend() <indexed_span_rend>`                  | the reverse sequence          |
+---------------------------------------------------------------------+-------------------------------+


