<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Extensions

## Support more types in the library

### Support for enum libraries

Currently, support for the following enum reflection libraries is provided:

* `boost::describe` [web page](https://github.com/boostorg/describe)
* `magic_enum` [web page](https://github.com/Neargye/magic_enum)
* `wise_enum` [web page](https://github.com/quicknir/wise_enum)

Adding a new enum library usually requires only two things:

* a compile-time check `is_xxx_enum`, to conditionaly enable the indexer
* an helper type, which will transform the enum into a compile time list
of all enum values

### Compile-time safe indexing using safe\_numerics

Since anything can be used as an index, it is theoretically possible to use
`safe_numerics` [web page](https://www.boost.org/doc/libs/1_80_0/libs/safe_numerics/doc/html/index.html)
to gain compile time check of array accesses. That would require writing a custom indexer for
`safe_range`.

## Indexed array and perfect hashing

Back to [index](index.md), or see [advanced usage](advancedusage.md)
