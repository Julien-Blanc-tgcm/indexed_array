.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

indexed_span constructors
=========================

.. _indexed_span_ctr_def:

Defaulted members
-----------------

.. cpp:function:: indexed_span::indexed_span(indexed_span const&) noexcept = default
.. cpp:function:: indexed_span::operator=(indexed_span const&) noexcept = default
.. cpp:function:: indexed_span::~indexed_span() noexcept = default

.. note::

    There are no move constructor nor move assignment operator defined, following the design
    choice of ``std::span``. As indexed_span is a non-owning view on the data, there is no 
    need to move it.

Deleted members
---------------

.. cpp:function:: indexed_span::indexed_span() = delete

.. note::

    This constructor could be defined if ``Indexer::size() == 0``, but currently is not.

Other constructors
------------------
.. _indexed_span_ctr_val:

.. cpp:function:: indexed_span::indexed_span(Value* begin)

    Constructs a new ``indexed_span`` over the range
    ``[begin, begin + Indexer::size]``, which must be a valid contiguous
    memory segment containing objects of type ``Value``. Behaviour is
    undefined otherwise.

    Since the extent of the span depends on the indexer, the constructor
    from a raw data pointer does not takes a size parameter.


