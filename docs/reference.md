<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Reference

## indexed\_array

```cpp
template<typename Value, typename Indexer>
/* */ indexed_array
```

Container that encapsulate a fixed size array, whose elements are accessible using a custom
indexing mechanism instead of a zero-based integer indexing. It is an aggregate type and
intend to be a drop-in replacement for `std::array`. The storage is guaranted to be contiguous,
and can be accessed as a C-array.

### Member functions

```cpp
indexed_array()
indexed_array(typename... Args)
```

Default constructor. May result in the values in the array not being initialized.

```cpp
indexed_array(typename... /* safe_arg */ Args)
```

Specialization of the constructor that takes a `safe_arg` list of values. This is
equivalent to aggregate initialization, but additional checks are done at compile
time:
* the number of arguments must match with the array size
* the explicit index of each argument must match the order of the indexer

```cpp
at(/* index */)
operator[/* index */] noexcept
```

Access to an element of the array. `at` is range-checked and throws `std::out_of_range` if the
value is incorrect. In case of a multidimensional array, `operator[]` signature is
`(std::tuple<index1, index2, ...>)`, whereas `at` signature is `(index1, index2, ...)`.

```cpp
front()
back()
data()
begin()
cbegin()
end()
cend()
rbegin()
crbegin()
rend()
crend()
empty()
size()
max_size()
fill()
swap()
```

Same semantic and guarantees as `std::array`.

```cpp
slice(/* 1-dimension index */) noexcept
slice_at(/* 1-dimension index */)
```

Returns a slice of the array, at the given index. For multidimensional arrays of extent `n`, it returns an
`indexed_span` (see below) of extent `n-1`. For single dimension array, it returns the element at the given
index. `slice` does not do any bound checking, `slice_at` throws `std::out_of_range` on error.

```cpp
indexed_array<int, Color, Material, interval<1, 10>> foo;
auto reds = foo.slice(Color::Red); // reds is indexed_span<int, Material, interval<1, 10>>
```

## indexed\_span

```cpp
template<typename Value, typename Indexer>
/* */ indexed_span
```

Span-like class which allows accessing elements through a custom indexer. `indexed_span` does
not own the data, and its size is always known at compile time (it always cover the full 
`Indexer` range). It is multidimensional if the Indexer is. It is related to 
`std::span<T, std::static_extent>` and has a similar interface.

### Member functions

```cpp
indexed_span() = delete // empty constructor is deleted
// copy/move operators defaulted
indexed_span(indexed_span const&)
indexed_span(indexed_span &&)
operator=(indexed_span const&)
operator=(indexed_span &&)
// constructor from a Value*
indexed_span(Value* begin);
```

Constructs a new `indexed_span` over the range `[begin, begin + Indexer::size]`, which must be
a valid contiguous memory segment containing objects of type `Value`. Behaviour is undefined
otherwise.

```cpp
at(/* Index */)
operator[/* Index */] noexcept
```

Element access. `at` is range-checked, and throws `std::out_of_range` on error. In case of
multidimensional span, `operator[]` signature is `std::tuple<Index1, Index2, ...>` whereas
`at` signature is `at(Index1, Index2, ...)`.

```cpp
front()
back()
data()
begin()
end()
rbegin()
rend()
empty()
size()
```

Same semantic as the corresponding methods in `std::span`.

```cpp
slice(/* 1-dimension index */) noexcept
slice_at(/* 1-dimension index */)
```

Returns a slice of the span, at the given index. For multidimensional spans of extent `n`, it returns an
`indexed_span` of extent `n-1`. For single dimension span, it returns the element at the given
index. `slice` does not do any bound checking, `slice_at` throws `std::out_of_range` on error.

## Sets of value

### interval

```cpp
template <auto minInclusive, type_identity_t<decltype(minInclusive)> maxInclusive>
struct interval
```

Represent a contiguous inclusive interval of all values between `minInclusive` and 
`maxInclusive` (thus, it contains `maxInclusive - minInclusive + 1` elements). This
type is a marker meant to be used with `default_indexer`.

### union\_of

```cpp
template <typename... Args>
/* */ union_of = typename to_integer_sequence<typename union_of_helper<Args...>::type>::type;
```

Represent the union of several sets of value (either `interval`, `integer_sequence` or `single_value`). The
result is always an `integer_sequence`.

### single\_value

```cpp
template <auto T1>
/* */ single_value = std::integral_constant<decltype(T1), T1>;
```

Represent a single value, meant to be used with `union_of` helper.
