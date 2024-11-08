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

.. _indexed_bitset_ull:

.. cpp:function:: indexed_bitset::indexed_bitset(unsigned long long val)

    Construct an indexed bitset whose underlying value is ``val``. 
 
    .. warning:: Be careful that custom
        index is **not** used here, so building an ``indexed_bitset<interval<-2, 1>> b`` with the
        value ``5`` is valid, and will result in ``b[-2] == true && b[0] == true``.

.. _indexed_bitset_safe_init:

.. cpp:function:: indexed_bitset::indexed_bitset(safe_arg<Indexer::value_type, bool>&&... values)

    Construct an indexed bitset with the given parameters. Each parameter must contain a single bool,
    and be a valid safe argument (its index will be checked). See :ref:`safe_initialization` for
    details.

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

