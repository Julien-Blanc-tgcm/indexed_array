.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Indexed_bitset constructors
===========================

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

.. _indexed_bitset_bitset_ctr:

.. cpp:function:: indexed_bitset::indexed_bitset(std::bitset<Indexer::size> const& other)

    Copy constructor from an std::bitset of the same size. The newly created indexed_bitset
    has the same underlying value as the given ``std::bitset``, but access to individual
    elements now follows the ``Indexer``'s scheme.

.. _indexed_bitset_copy: 

Copy operators
==============

Copy operators are defaulted, and have the same semantics as ``std::bitset`` ones.

.. cpp:function:: constexpr indexed_bitset::operator=(indexed_bitset const&) = default

.. cpp:function:: constexpr indexed_bitset::operator=(indexed_bitset &&) = default

