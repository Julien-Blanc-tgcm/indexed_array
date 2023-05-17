.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Default Indexer
===============

Sets of value
-------------

index_range
^^^^^^^^^^^

.. code:: cpp

   template <auto minInclusive, type_identity_t<decltype(minInclusive)> maxInclusive>
   struct index_range

..

   Represent a contiguous inclusive range of all values between
   ``minInclusive`` and ``maxInclusive`` (thus, it contains
   ``maxInclusive - minInclusive + 1`` elements). This type is a marker
   meant to be used with ``default_indexer``.

*This was called ``interval`` in previous versions of the library, the
alias is still provided*

union_of
^^^^^^^^

.. code:: cpp

   template <typename... Args>
   /* */ union_of = typename to_value_sequence<typename union_of_helper<Args...>::type>::type;

..

   Represent the union of several sets of value (either ``index_range``,
   ``value_sequence`` or ``single_value``). The result is always a
   ``value_sequence``.

single_value
^^^^^^^^^^^^

.. code:: cpp

   template <auto T1>
   /* */ single_value = std::integral_constant<decltype(T1), T1>;

..

   Represent a single value, meant to be used with ``union_of`` helper.

value_sequence
^^^^^^^^^^^^^^

.. code:: cpp

   template <typename T, T...>
   struct value_sequence
   {
       static_assert(std::is_enum<T>::value || std::is_integral<T>::value);
   };

..

   Represent a sequence of values. Use like this:

.. code:: cpp

   value_sequence<my_enum, my_enum::v1, my_enum::v2, ... >

*This is equivalent to ``std::integer_sequence``, but accepts enum as
constant values, which is not the case for standard integer sequence.*

make_default_indexer
--------------------

.. code:: cpp

   template <typename... Args>
   /* */ make_default_indexer

The ``make_default_indexer`` template is an helper class to define
indexers. Its usage is not necessary when using ``indexed_array`` or
``indexed_span``, but is when defining a custom indexer (see `Advanced
usage <advancedusage.md>`__.

``indexed_array<int, X>`` is an alias for
``indexed_array<int, make_default_indexer<X>>`` if ``X`` is not already
itself an indexer. The following must hold:

.. code:: cpp

   static_assert(std::is_same<
       indexed_array<char, index_range<-2, 3>>,
       indexed_array<char, make_default_indexer<index_range<-2, 3>>>
       >::value);

``default_indexer`` is defined for ``describe``-d enums,
``index_range``\ s and ``value_sequence``\ s.

``default_indexer`` is also defined for multidimensional indexers. It is
the most convenient way to define a multidimensional indexer (and is
what is used by variadic ``indexed_array``):

.. code:: cpp

   using multidim_indexer = make_default_indexer<index_range<-2, 3>, index_range<-5, 6>, my_described_enum>;
   static_assert(std::is_same<
       indexed_array<char, multidim_indexer>,
       indexed_array<char, index_range<-2, 3>, index_range<-5, 6>, my_described_enum>
       >::value);

