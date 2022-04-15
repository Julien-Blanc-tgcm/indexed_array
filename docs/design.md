<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Design principles

## The indexed\_array type

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

To avoid code duplication, the `at` method is templated on a boolean parameter, which will tell if
it needs to do bound check or not. It shall throw an `std::out_of_range` exception if requested to
do bound checking, and the index given is invalid (`indexed_array` won't do bound check when accessing
the inner array).

The only challenging issue will be discussed in *going multidimensional*.

## The default\_indexer type

Now that we have a basic generic array type, most of the work resides in providing indexers. The library
provides an implementation, called `default_indexer`, which should be suitable for most purposes, and a
few helper types used to customize the `default_indexer`.

```cpp
template<typename Index, typename T = void>
struct default_indexer
{ // default implementation is empty
};
```

### Handling integers and enum the same way

In several situations (index computations), we want to handle integral types and enum types the same way. So,
one of the first thing we do is define the following helper:

```cpp
template·<typename·T,·T·value,·typename·U·=·void>
struct·integral_value
{
};

template·<typename·T,·T·v>
struct·integral_value<T,·v,·std::enable_if_t<std::is_integral<T>::value,·void>·>
{
→   static·inline·constexpr·T·value·=·v;
};

template·<typename·T,·T·v>
struct·integral_value<T,·v,·std::enable_if_t<std::is_enum<T>::value,·void>·>
{
→   static·inline·constexpr·typename·std::underlying_type<T>::type·value·=
→   ····static_cast<typename·std::underlying_type<T>::type>(v);
};

template<auto·C>
static·inline·constexpr·auto·const·integral_value_v·=·integral_value<decltype(C),·C>::value;
```

This will allow us to convert enum values to their underlying integral type, both at compile and
run time, while keeping integral value unchanged. This will help reduce boilerplate code.

### Indexing using an integral (or enum) interval

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

Then we just have to write the following specialization (see source code for actual implementation):
```cpp
template <typename T, T min, T max>
struct default_indexer<
    interval<min, max>, 
    typename std::enable_if_t<
        std::is_integral<T>::value || std::is_enum<T>::value,
        void>>
{
	// ...
};
```

So this works for any integral or enum type.

### Indexing using an integer sequence

Such indexing provides a mapping between the value and the index in the sequence.

The standard library already provides a type to represent such a sequence, it's called `std::integer_sequence`,
so we just need to write a specialization for it. This is pretty straightforward, but there's a pretty cheap
optimization we can do: detect if the sequence is contiguous, and in that case use an interval-like scheme.

We thus provide two specializations:

```cpp
template <typename T, T... vals>
struct default_indexer<
    std::integer_sequence<T, vals...>,
    typename std::enable_if_t<detail::is_contiguous_sequence<mp11::mp_list_c<T, vals...> >::value, void> >
{
	// ...
};

template <typename T, T... vals>
struct default_indexer<
    std::integer_sequence<T, vals...>,
    typename std::enable_if_t<!detail::is_contiguous_sequence<mp11::mp_list_c<T, vals...> >::value, void> >
{
	// ...
};
```

This require the definition of the `is_contiguous_sequence` helper. A design choice has been done here, a
sequence is considered a valid contiguous sequence even if it contains duplicate values, as long as they
are grouped together. This choice has been made to support aliases in enum values (see below, indexing
with a `describe`-d enum). Which means that we have:

```cpp
static_assert(is_contiguous_sequence<std::integer_sequence<int, 1, 2, 3, 4>>::value);
static_assert(is_contiguous_sequence<std::integer_sequence<int, 0, 0, 1, 2>>::value);
static_assert(! is_contiguous_sequence<std::integer_sequence<int, 0, 1, 0, 2>>::value);
static_assert(! is_contiguous_sequence<std::integer_sequence<int, 0, 1, 2, 4>>::value);
```

### Indexing using a described enum


