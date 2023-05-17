.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

Extensions
==========

Support more types in the library
---------------------------------

Support for enum libraries
~~~~~~~~~~~~~~~~~~~~~~~~~~

Currently, support for the following enum reflection libraries is
provided:

-  ``boost::describe`` `web
   page <https://github.com/boostorg/describe>`__
-  ``magic_enum`` `web page <https://github.com/Neargye/magic_enum>`__
-  ``wise_enum`` `web page <https://github.com/quicknir/wise_enum>`__

Adding a new enum library usually requires only two things:

-  a compile-time check ``is_xxx_enum``, to conditionaly enable the
   indexer
-  an helper type, which will transform the enum into a compile time
   list of all enum values

Compile-time safe indexing using safe_numerics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Since anything can be used as an index, it is possible to
use ``safe_numerics`` `web
page <https://www.boost.org/doc/libs/1_80_0/libs/safe_numerics/doc/html/index.html>`__
to gain compile time check of array accesses. This requires writing
a custom indexer for ``safe_range``.

An example of such an indexer is provided in the ``indexed_array/safe_numerics.hpp``
header file:

.. doxygenstruct:: jbc::indexed_array::safe_signed_range_indexer

Indexed array and perfect hashing
---------------------------------

Despite being inherently an array, ``indexed_array`` has some properties
that resembles a map. The main difference with a map is that only values
are stored, but because it does actually a mapping between a key and an
index, it can virtually be used with anything as a key.

If you have a perfect hashing function, which maps a set of n values in
the range ``[0..n-1]``, then it can be used as the indexer to create an
``indexed_array``. It can be used, for example, to index an
``indexed_array`` with a set of known strings (as ``string_view``).

Such uses show the versatility the design of ``indexed_array`` offers,
but providing such an indexer is clearly outside the scope of the
library.
