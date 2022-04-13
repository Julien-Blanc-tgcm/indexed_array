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

# Feature table comparison

TODO: fill section
