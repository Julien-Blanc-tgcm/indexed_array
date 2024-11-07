.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Indexed_bitset observers
========================

Default constructors
--------------------

The following constructors are explicitly defaulted:

.. _indexed_bitset_default_ctr:

.. cpp:function:: indexed_bitset::indexed_bitset()

    Default constructor. All bits are set to zero.

.. _indexed_bitset_copy_ctr:

.. cpp:function:: indexed_bitset::indexed_bitset(indexed_bitset<Indexer> const& other)

    Copy constructor.

.. _indexed_bitset_move_ctr:

.. cpp:function:: indexed_bitset::indexed_bitset(indexed_bitset<Indexer>&& other)

    Move constructor. It is guaranteed to leave other in a valid but unspecified state.


