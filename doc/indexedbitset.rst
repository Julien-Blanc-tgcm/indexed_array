.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Indexed bitset
==============

.. toctree::
    :hidden:

    indexedbitset_constructors
    indexedbitset_dataaccess
    indexedbitset_observers

.. cpp:class:: template <typename Indexer> indexed_bitset

    Container that encapsulate a bitset, whose individual bits are
    accessible using a custom indexing mechanism instead of a
    zero-based integer indexing. It is an aggregate type and is a
    replacement for ```std::bitset``, although the set of operation
    provided is slightly different. The guarantees are the same as
    ``std::bitset`` for operations that exists in both.

Construction and destruction
----------------------------

+--------------------------------------------------------------------------+---------------------------------+
| *Constructors*                                                                                             |
+==========================================================================+=================================+
| :ref:`indexed_bitset() = default <indexed_bitset_default_ctr>`           | Default construction            |
+--------------------------------------------------------------------------+---------------------------------+
| | :ref:`indexed_bitset(indexed_bitset const&) <indexed_bitset_copy_ctr>` | Copy and move constructors      |
| | :ref:`indexed_bitset(indexed_bitset&&) <indexed_bitset_move_ctr>`      |                                 |
+--------------------------------------------------------------------------+---------------------------------+
| :ref:`indexed_bitset(Args&&...) <indexed_bitset_aggregate>`              | Aggregate-like initialization   |
+--------------------------------------------------------------------------+---------------------------------+
| :ref:`indexed_bitset(checked_arg&&...) <indexed_bitset_safe_init>`       | Safe initialization             |
+--------------------------------------------------------------------------+---------------------------------+
| :ref:`indexed_bitset(std::bitset const&) <indexed_bitset_bitset_ctr>`    | Construction by copy of a bitset|
+--------------------------------------------------------------------------+---------------------------------+
| *Operators*                                                                                                |
+--------------------------------------------------------------------------+---------------------------------+
| | :ref:`operator=(indexed_bitset const&) <indexed_bitset_copy>`          | Copy / move assignment          |
| | :ref:`operator=(indexed_bitset &&) <indexed_bitset_copy>`              |                                 |
+--------------------------------------------------------------------------+---------------------------------+

Destructor is explicitly defaulted.

Member functions
----------------

Data Access
^^^^^^^^^^^

Observers
^^^^^^^^^


