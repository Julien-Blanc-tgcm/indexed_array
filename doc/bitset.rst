.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

``indexed_bitset``
==================

``indexed_bitset`` is to ``std::bitset`` what ``indexed_array`` is to
``std::array``: it offers similar features, but uses a custom indexing
scheme to access individual bits instead of a standard 0-based index.

It only offers a subset of ``std::bitset`` interface, because some
features may be counter-intuitive and as such dangerous. If you need
these features, you can explicitly cast an ``indexed_bitset`` to its
underlying ``std::bitset``.

Usage
-----

Declaring an ``indexed_bitset`` follows the same pattern as
``indexed_array``, but there is no value type: it is always a bool:

    .. code-block:: cpp
        :caption: Sample indexed bitset
        
        indexed_bitset<index_range<1, 10>> b(0b1001001001u);
        assert(b.size() == 10);

Operations available
--------------------

The common operations on bitset are available:

    .. code-block:: cpp
        :caption: Sample operations
        
        // construct from unsigned value
        test
        set
        reset
        flip
        operator[]
        size
        to_ulong
        to_ullong

and additionally ``in_range`` to check if a value is in the range of acceptable
values for the indexer. Notably, assignment operators are disabled,
because they are too error prone. If assignment is needed, first cast to
the underlying ``std::bitset``:

    .. code-block:: cpp
        :caption: Assignment from value
        
        static_cast<std::bitset&>(b) = 0x08u;
        assert(b[4].test()); // 1-based indexing !
