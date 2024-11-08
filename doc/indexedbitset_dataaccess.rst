.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

indexed_bitset data access
===========================

Data access
-----------

.. _indexed_bitset_test:
.. _indexed_bitset_bracket:
.. cpp:function:: constexpr reference indexed_bitset::test(Index i) const
.. cpp:function:: constexpr reference indexed_bitset::operator[](Index i) const
    
    Element access. ``test`` is range-checked, and throws
    ``std::out_of_range`` on error. In case of multidimensional span,
    ``operator[]`` signature is ``std::tuple<Index1, Index2, ...>``
    whereas ``test`` signature is ``test(Index1, Index2, ...)``. If using
    ``C++23``, ``operator[]`` is also available as ``[Index1, Index2, ...]```

   :param i: The index. Anything that is implicitly convertible to the Index can
       be used.
   :returns: the value at index i (either ``true`` or ``false``)

.. _indexed_bitset_all:
.. cpp:function:: constexpr reference indexed_bitset::all() const

    Returns true if all the bits in the bitset have the value ``true``.
    
.. _indexed_bitset_any:
.. cpp:function:: constexpr reference indexed_bitset::any() const

    Returns true if at least one of the bits in the bitset has the value ``true``. This is
    equivalent to ``!bitset.none()``.
    
.. _indexed_bitset_none:
.. cpp:function:: constexpr reference indexed_bitset::none() const

    Returns true if no bit in the bitset has the value ``true``. If applicable, this
    is equivalent to ``to_ulonglong() == 0``.

.. _indexed_bitset_count:
.. cpp:function:: constexpr std::size_t indexed_bitset::count() const

    Returns the number of bits set to ``true`` in the bitset.

