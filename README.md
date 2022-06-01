# Indexed Array

A C++17 fixed size containers, with arbitrary indexes. Provides support
for indexing via enum values, arbitrary intervals (starting at non-zero
values), and non-contiguous sets. Extensible to support virtually any
kind of indexing, including multi-dimensional indexing.

Also offers a feature named “safe-initialization”, which allows providing the
index with the value when initializing the array content. This can be seen
as the pendant of C++20 designated initializer-lists, but for array content.

## Sample code

```cpp
// we want an array of 10 strings, index starting at one
using MyArray = indexed_array<std::string, interval<1, 10>>;

MyArray arr;
arr[1] = "First element of the array";

// this one will be indexed by an enum value
using AnotherArray = indexed_array<std::string, interval<my_enum::first, my_enum::last>>;
```

## Motivation

The rationale for writing this library is twofold:

* avoid the `static_cast` repetitions when accessing some array elements via enum values,
and avoid arithmetic fiascos when some indexes are not starting at 0.
* enforce stricter rules than the C++ default ones for aggregate initialization, to detect
more errors at compile time.

Code such as:

```cpp
auto& val = my_data[static_cast<std::size_t>(index) - 1];
```

Should simply be written as:

```cpp
auto& val = my_data[index];
```

This makes it clear both what the index is (being a strongly typed enum), the range of acceptable
values, and avoids any mistakes like forgetting to apply an offset.

Non-contiguous indexing is a nice feature if you need to handle special values, or are handling both
normative values and manufacturer specific extensions.

```cpp
enum class SomeEnum : std::uint8_t
{
	// These values are normative values, do not change them
	Foo = 0,
	Bar = 1,
	Baz = 2,
	...
	Blah = 12,
	// These values are manufacturer extensions, in the reserved range
	MSExtension1 = 0x80,
	MSLight = 0x81,
	...
};
```

The main motivation for safe initialization comes from a project where several enums are generated
from a description file in an early stage of the compilation, and later used in the code (for example
they give the size of some arrays). Some changes in the source description file may lead to breakage,
getting a compile time error for such breakages is thus a requirement.

Multi-dimensional indexing came as a natural extension of this, as it can be seen as just a special
case of indexing.

## Supported Compilers

* GCC 9 and later
* Clang 11 and later

(Not tested with other compilers, but should work with any c++17 compliant
compiler)

## Requirements

* boost::mp11 library
* boost::describe library for described enums support (can be disabled at
  compile time)

## Documentation

* See [Online doc](https://julien-blanc-tgcm.github.io/indexed_array/)

## Licence

Distributed under the
[Boost Software License, Version 1.0](http://boost.org/LICENSE_1_0.txt).
