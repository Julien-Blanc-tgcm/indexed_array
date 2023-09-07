.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Indexed_array constructors
==========================

Default constructors
--------------------

The following constructors are explicitly defaulted:

.. _indexed_array_default_ctr:

.. cpp:function:: indexed_array::indexed_array()

    Default constructor. May result in the values in the array not being initialized.

.. _indexed_array_copy_ctr:

.. cpp:function:: indexed_array::indexed_array(indexed_array const& other)

.. _indexed_array_move_ctr:

.. cpp:function:: indexed_array::indexed_array(indexed_array&& other)

    Move constructor is defaulted as well.

.. _indexed_array_aggregate:

Aggregate-like initialization
-----------------------------

Construction from a list of values is using two defined constructors:

.. cpp:function:: template<typename... Args> indexed_array::indexed_array(Args&&... args)

    Construction with a list of values. Roughly equivalent to aggregate initialization of ``std::array``. All arguments
    must be convertible to ``value_type``
    
.. _indexed_array_safe_init:

Safe initialization
-------------------

.. cpp:function:: template<typename... Args> indexed_array::indexed_array(safe_arg<Arg, Value>&&... list)

    Construction from a list of safe args. This is equivalent to aggregate initialization, but additional checks are done at compile time:
    
    * the number of arguments must match with the array size
    * the explicit index of each argument must match the order of the indexer

    :param list: The list of values. They need to be all an implementation of the ``checked_arg`` concept, which
        ``safe_arg`` provides.

.. _indexed_array_array_ctr:

Construction via copy of an array
---------------------------------

.. cpp:function:: constexpr indexed_array::indexed_array(std::array<value_type, Indexer::size> const& content)

    Construction from an std::array which has the same size and the same value_type than the indexed_array. The
    content is copied into the storage of the indexed_array.

    :param content: The content to copy into the indexed array

.. _indexed_array_copy:

Copy operators
--------------

Copy operators are defaulted, and thus have the same semantic as the ones defined for std::array.

.. cpp:function:: constexpr indexed_array::operator=(indexed_array const&) = default

.. cpp:function:: constexpr indexed_array::operator=(indexed_array &&) = default
