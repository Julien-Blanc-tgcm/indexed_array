.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

.. _indexed_array:

Indexed_array
=============

.. toctree::
    :hidden:

    indexedarray_constructors
    indexedarray_dataaccess
    indexedarray_iterators
    indexedarray_observers

.. code:: cpp

   template <typename Value, typename Indexer>
   /* */ indexed_array

Container that encapsulate a fixed size array, whose elements are
accessible using a custom indexing mechanism instead of a zero-based
integer indexing. It is an aggregate type and intend to be a drop-in
replacement for ``std::array``. The storage is guaranted to be
contiguous, and can be accessed as a C-array.

Construction and destruction
----------------------------

+-----------------------------------------------------------------------+---------------------------------+
| *Constructors*                                                                                          |
+=======================================================================+=================================+
| :ref:`indexed_array() = default <indexed_array_default_ctr>`          | Default construction            |
+-----------------------------------------------------------------------+---------------------------------+
| | :ref:`indexed_array(indexed_array const&) <indexed_array_copy_ctr>` | Copy and move constructors      |
| | :ref:`indexed_array(indexed_array&&) <indexed_array_move_ctr>`      |                                 |
+-----------------------------------------------------------------------+---------------------------------+
| :ref:`indexed_array(Args&&...) <indexed_array_aggregate>`             | Aggregate-like initialization   |
+-----------------------------------------------------------------------+---------------------------------+
| :ref:`indexed_array(checked_arg&&...) <indexed_array_safe_init>`      | Safe initialization             |
+-----------------------------------------------------------------------+---------------------------------+
| :ref:`indexed_array(std::array const&) <indexed_array_array_ctr>`     | Construction by copy of an array|
+-----------------------------------------------------------------------+---------------------------------+

Destructor is explicitly defaulted.

Member functions
----------------

Data Access
^^^^^^^^^^^

+-----------------------------------------------------------------------+---------------------------------+
| *Standard std::array members*                                                                           |
+=======================================================================+=================================+
| | :ref:`at <indexed_array_at>`                                        | Index-based element access      |
| | :ref:`operator[] <indexed_array_at>`                                |                                 |
+-----------------------------------------------------------------------+---------------------------------+
| :ref:`front <indexed_array_front>`                                    | Access to first element         |
+-----------------------------------------------------------------------+---------------------------------+
| :ref:`back <indexed_array_back>`                                      | Access to last element          |
+-----------------------------------------------------------------------+---------------------------------+
| :ref:`data <indexed_array_data>`                                      | Access to raw content           |
+-----------------------------------------------------------------------+---------------------------------+
| *Additional members*                                                                                    |
+-----------------------------------------------------------------------+---------------------------------+
+ | :ref:`slice <indexed_array_slice>`                                  | Returns a view on the lower-rank+
+ | :ref:`slice_at <indexed_array_slice_at>`                            | content.                        +
+-----------------------------------------------------------------------+---------------------------------+

Access to the underlying data.

+-------------------------------------------+------------------------------------+
| *Iteration*                                                                    +
+===========================================+====================================+
| | :ref:`begin <indexed_array_begin>`      | Iterator to the start of the array |
| | :ref:`cbegin <indexed_array_begin>`     |                                    |
+-------------------------------------------+------------------------------------+
| | :ref:`indexed_array_end`                | Iterator to the end of the array   |
| | :ref:`cend <indexed_array_end>`         | (past last element)                |
+-------------------------------------------+------------------------------------+
| | :ref:`indexed_array_rbegin`             | Reverse iterator to the            |
| | :ref:`crbegin <indexed_array_rbegin>`   | “beginning” of the array           |
+-------------------------------------------+------------------------------------+
| | :ref:`indexed_array_rend`               | Reverse iterator to the “end”      |
| | :ref:`crend <indexed_array_rend>`       | of the array                       |
+-------------------------------------------+------------------------------------+


Observers
^^^^^^^^^

+-------------------------------------------------+------------------------------------+
| *std::array members*                                                                 +
+=================================================+====================================+
| :ref:`empty <indexed_array_empty>`              | Checks if container is empty       |
+-------------------------------------------------+------------------------------------+
+ | :ref:`size <indexed_array_size>`              | Returns the size of the array      |
+ | :ref:`max_size <indexed_array_max_size>`      | ( ``max_size() == size()`` )       |
+-------------------------------------------------+------------------------------------+
+ :ref:`fill <indexed_array_fill>`                | Fills the array with value         |
+-------------------------------------------------+------------------------------------+
+ :ref:`swap <indexed_array_swap>`                | Swap two indexed_array contents    |
+-------------------------------------------------+------------------------------------+
+ *extensions*                                                                         +
+-------------------------------------------------+------------------------------------+
+ :ref:`in_range <indexed_array_in_range>`        | Checks if an index is in the       |
+                                                 | acceptable range                   |
+-------------------------------------------------+------------------------------------+
+ :ref:`is_o1 <indexed_array_is_o1>`              | Tells if element access is O(1)    +
+-------------------------------------------------+------------------------------------+


