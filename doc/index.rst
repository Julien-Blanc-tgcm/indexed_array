.. Copyright 2023 Julien Blanc
   Distributed under the Boost Software License, Version 1.0.
   https://www.boost.org/LICENSE_1_0.txt

IndexedArray's documentation
============================

Overview
--------

This library provides a generic fixed-size container, that is a drop-in
replacement for ``std::array``, that uses custom indexes. It allows writing
code like the following:

.. code-block:: cpp
    :caption: Sample usage

    enum class Foo { Bar1 = 4, Bar2, Bar3 };
    // ...
    indexed_array<string, Foo> foo; // foo.size() == 3
    foo[Foo::Bar2] = "an example string"; // modify the second element of foo


Multi-dimensional indexers are supported as well, you can write something like

.. code-block:: cpp
    :caption: Multidimensional sample

    indexed_array<string, Foo, Foo> foo; // foo.size() == 9
    foo.at(Foo::Bar2, Foo::Bar2) = "an example string"; // modify the “central” element
    // If compiling in c++23 mode:
    foo[Foo::Bar2, Foo::Bar2] = "an example string";


Table of content
----------------

.. toctree::
    :maxdepth: 2

    Introduction and rationale for the library <introduction>
    Tutorial for simple usage <tutorial>
    Avanced usage <advancedusage>
    Multidimensional arrays <multidimensional>
    Bitsets <bitset>
    Safe initialization <safeinitialization>
    Performance <performance>
    Integrations and extensions <extensions>
    Example use cases <usecases>
    Reference <reference>
    Design principles <design>
    Comparison with other libraries <comparison>

Requirements
------------

The library has the following requirements:

* A ``C++17`` capable compiler (``C++20`` is required for some features)
* ``boost::mp11``

Optional, but strongly recommended, is one of the following:

* ``boost::describe``
* ``magic_enum``
* ``wise_enum``

Indices and tables
------------------

* :ref:`genindex`
* :ref:`search`
