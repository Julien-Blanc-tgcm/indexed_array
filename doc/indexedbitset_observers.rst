.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Indexed_bitset observers
========================

.. _indexed_bitset_size:
.. cpp:function:: constexpr size_t indexed_bitset::size() const

    :returns: The number of individual bits in the bitset

Extensions
----------

.. _indexed_bitset_in_range:

.. cpp:function:: static constexpr bool indexed_bitset::in_range(index)

   static constexpr method that returns ``true`` if the argument is a
   valid index value for this bitset, ie calling ``operator[value]`` is
   well defined behavior. Returns ``false`` otherwise.
