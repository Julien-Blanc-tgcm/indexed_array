# Indexed Array

A C++17 fixed size containers, with arbitrary indexes. Provides support
for indexing via enum values, arbitrary intervals (starting at non-zero
values), and non-contiguous sets. Extensible to support virtually any
kind of indexing.

## Supported Compilers

* GCC 9 and later
* Clang 11 and later

(Not tested with other compilers, but should work with any c++17 compliant
compiler)

## Requirements

* boost::mp11 library
* boost::describe library for described enums support (can be disabled at
  compile time)

## Licence

Distributed under the
[Boost Software License, Version 1.0](http://boost.org/LICENSE_1_0.txt).
