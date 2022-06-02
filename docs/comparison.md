<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Comparison with other approaches

`indexed_array` solves a common problem, for which solutions already exists. They,
however, all have their different trade-offs, which lead me to the writing of this
library.

## Using std c++20 range library (views)

C++20 `views` are an elegant solution to the problem of indexing using arbitrary indexes.
However, they are not containers per-se: they don't own the data. As such, they do
not guarantee anything about the data, such as contiguity or elements storage order,
which can make a difference in some scenarios.

The good news is that `indexed_array`, like any container, can take full benefit of `views`,
which can be used on top of it.

## Using `std::unordered_map`

`std::unordered_map` provides the same convenience of access as `indexed_array`. However, it
is a dynamically sized container. It does not provide the triviality guarantees, nor does it
provides safe initialization.

## Boost.MultiIndex

`boost::multi_index` is intended to provide different ways of indexing a data set. This is
different from `indexed_array` which provides a single indexing mechanism. It does not
provide triviality guarantees, and is dynamically resizable.

## Frozen

[Frozen](https://github.com/serge-sans-paille/frozen)

Frozen's `unordered_map` offers another way to access elements via custom values. It provides
a fixed size, container-like structure. It allows providing a custom hash. It guarantees O(1)
access to its elements.

There are a few differences, though:

* there are no guarantees on the way the element are stored inside the container. In particular,
you don't have any control on the order of the elements.
* there is no triviality propagation
* there are no guarantees on the size of the data structure. An `unordered_map` of 4 pairs of int values
takes already more than 150 bytes.
* there is no unchecked access, only the throwing at() function is provided

# Feature table comparison

| Feature \ Library | `indexed_array` | `std::array` | `std::unordered_map` | Boost.MultiIndex | Frozen |
|:------------------|:---------------:|:------------:|:--------------------:|:----------------:|:------:|
| Index via arbitrary values |    -  |  ✓            |    ✓                 |    ✓             |   ✓    |
| Contiguity of elements     |    ✓  |  ✓            |    -                 |    -             |   -    |
| Triviality propagation     |    ✓  |  ✓            |    -                 |    -             |   -    |
| No heap allocation         |    ✓  |  ✓            |    -                 |    -             |   ✓    |
| Can reside in ro sections / flash memory |  ✓  | ✓ |    -                 |    -             |   ?    |
| No size overhead  |      ✓         |   ✓           |    -                 |    -             |   -    |
| Safe initialization |    ✓         |     -         |      partial         |     -            |   ✓    |
| Multidimensional support | ✓       |     -         |        -             |     ✓            |   -    |
| Dynamic size     |    -            |     -         |        ✓             |     ✓            |   -    |
| Guaranted O(1) access | partial¹   |     ✓         |        ✓             |     ✓            |   ✓    |
| Minimum C++ version |  `C++17`      |  `C++11`      |    `C++03`           |  `C++03`         | `C++14` |


¹: Access guarantees depends on indexer template parameter. O(1) access is guaranted by the default indexer
if the index is a contiguous range of elements (no gaps between the successive values). If that's not the
case, the current implementation use O(n) access. This could be improved to O(log(n)) in a further release,
for great values of n.

