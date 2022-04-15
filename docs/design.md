<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Design principles

## `indexed_array` type

The library is designed around a single template type, which gave its name to the library,
`indexed_array`. It is defined as follows:

```cpp
template<typename Value, typename Indexer>
class indexed_array
```

Its semantic is very similar to the one of the standard `std::array`. The `Value` parameters determines
the type of the data stored in the container, while the `Indexer` type is in charge of providing the
following information:

* the size of the container
* the “type” to use for the index
* a mapping between an external index and an unsigned integer number in the range `[0 - (size-1)]`.

The implementation is thus pretty straightforward:
* an `array<Value, Indexer::size>` private member
* a `at(typename Indexer::index)` (and respective `operator[]`) public member
* all remaining interface from `std::array` wrapped

The only challenging issue will be discussed in *going multidimensional*.

## `default_indexer` type

Now that we have a basic generic array type, most of the work resides in providing indexers. The library
provides an implementation, called `default_indexer`, which should be suitable for most purposes, and a
few helper types used to customize the `default_indexer`.

```cpp
template<typename Index, typename T = void>
struct default_indexer
{ // default implementation is empty
};
```

### Indexing using an integral interval

The first thing that is needed is a type to represent such an interval (the term interval is used preferrably
over range, to avoid any confusion with the C++20 range library).

The library provides one, whose definition is as simple as :
```cpp
template <auto minInclusive, type_identity_t<decltype(minInclusive)> maxInclusive>
struct interval
{
	using type = decltype(minInclusive);
	static inline constexpr type const min = minInclusive;
	static inline constexpr type const max = maxInclusive;
};
```

Then we just have to write the following specialization:
```cpp
template <typename T, T min, T max>
struct default_indexer<interval<min, max>, typename std::enable_if_t<std::is_integral<T>::value, void> >
{
	// ...
};
```
