# Indexed Array

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License](https://img.shields.io/badge/license-Boost%201.0-lightgrey.svg)](https://www.boost.org/LICENSE_1_0.txt)
[![GitHub Actions](https://github.com/Julien-Blanc-tgcm/indexed_array/actions/workflows/cmake.yml/badge.svg)](https://github.com/Julien-Blanc-tgcm/indexed_array/actions)

A C++20 fixed size container, with arbitrary indexes. Provides support
for indexing via enum values, arbitrary intervals (starting at non-zero
values), and non-contiguous sets. Extensible to support virtually any
kind of indexing, including multi-dimensional indexing.

Also offers a feature named “safe-initialization”, which allows providing the
index with the value when initializing the array content. This can be seen
as the pendant of C++20 designated initializer-lists, but for array content.

## ✨ Features

### Arbitrary Indexing
- **Ranges**: `interval<Start, End>` for contiguous integer ranges.
- **Enums**: Works with `enum class` (greatly enhanced by using `C++26`, or an enum reflection library
  such as `boost::describe`, `magic_enum` or `wise_enum`. Works with non-contiguous enums.
- **Sets**: `union_of<...>` for non-contiguous or mixed indexes.
- **Custom Indexers**: Implement your own indexer with `lambda_indexer` or by satisfying the `indexer` concept.

### Safe Initialization
- **Compile-time checks**: Ensures all indexes are initialized.
- **Tagged Initializers**: Prevents errors when inserting new values in the middle of an initialization list.

### Multidimensional

* multidimensional indexing with indexed span (non-owning view over the data) support

### Additional Containers
- **`indexed_bitset`**: A bitset with arbitrary indexes (e.g., for enum-based flags).
- **`indexed_span`**: A non-owning view over an `indexed_array` or raw data.

### Integration with enum reflection libraries

Out of the box, `indexed_array` has support for the following enum reflection libraries:

* [Boost Describe](https://www.boost.org/doc/libs/latest/libs/describe/doc/html/describe.html)
* [Magic Enum](https://github.com/Neargye/magic_enum)
* [Wise Enum](https://github.com/quicknir/wise_enum)
* C++26 reflection

To enable the integration, you must define the relevant `INDEXED_ARRAY_HAS_[DESCRIBE|MAGIC_ENUM|WISE_ENUM|REFLECTION]` macro
when compiling.

## 🎯 Motivation

The rationale for writing this library is the following:

* avoid the `static_cast` repetitions when accessing some array elements via enum values,
and avoid arithmetic fiascos when some indexes are not starting at 0.
* enforce stricter rules than the C++ default ones for aggregate initialization, to detect
more errors at compile time.
* transparently support “gaps” or “holes” in indexing values

Code such as:

```cpp
auto& val = my_data[static_cast<std::size_t>(index) - 1];
```

Should simply be written as:

```cpp
auto& val = my_data[index];
```

This makes it clear both what the index is (being a strongly typed enum), the range of acceptable
values, and avoids any mistakes like forgetting to apply an offset or incorrectly using a wrongly
typed variable as the index.

Non-contiguous indexing is a nice feature if you need to handle special values, or are handling both
normative values and manufacturer specific extensions.

The main motivation for safe initialization comes from a project where several enums are generated
from a description file in an early stage of the compilation, and later used in the code (for example
they give the size of some arrays). Some changes in the source description file may lead to breakage,
getting a compile time error for such breakages was thus a requirement.

Multi-dimensional indexing came as a convenience extension, as it can be seen as just a special
case of indexing.


## 📦 Installation

The library is header only. 

### Install as a Submodule (Recommended)
```bash
git submodule add https://github.com/Julien-Blanc-tgcm/indexed_array.git
git submodule update --init --recursive
```

## 🔧 Compilation

### Requirements

- `C++20` compliant compiler (GCC 9+, Clang 11+, MSVC 2022+)
  - `C++23` and `C++26` compiler brings extra features
- [Boost](https://www.boost.org/) (mp11, describe for enum support).
  - Boost 1.80+ recommended.
- [CMake](https://cmake.org/) >= 3.25

### Tests

To compile tests, just run :
```sh
mkdir build
cd build
cmake ../.
make -j
ctest
```

By default, tests are built and run using `boost::describe` integration. 

To enable `C++26` reflection, use instead.
```sh
cmake -DUSE_CPP26_REFLECTION ../.
```

### Integration

Just add the `indexed_array/include` directory to your include path. For cmake

```cmake
# include indexed_array base directory
include_directories(indexed_array/include)
```

And in your cpp file

```cpp
#include <jbc/indexed_array.hpp>
```

Some macros needs to be defined to enable integrations. Only one can
be picked at a time :

```
INDEXED_ARRAY_HAS_DESCRIBE    # For Boost Describe
INDEXED_ARRAY_HAS_MAGIC_ENUM  # For Magic Enum
INDEXED_ARRAY_HAS_WISE_ENUM   # For Wise Enum
INDEXED_ARRAY_HAS_REFLECTION  # For C++26 reflection
```

For cmake:

```cmake
# Enable enum reflection (choose ONE)
add_definitions(-DINDEXED_ARRAY_HAS_MAGIC_ENUM)  # For Magic Enum
# add_definitions(-DINDEXED_ARRAY_HAS_DESCRIBE)   # For Boost Describe
# add_definitions(-DINDEXED_ARRAY_HAS_WISE_ENUM)   # For Wise Enum
# add_definitions(-DINDEXED_ARRAY_HAS_REFLECTION)  # For C++26 reflection
```

The integrations can also be enabled directly in code by including the relevant header file instead
```
jbc/indexed_array/describe.hpp
jbc/indexed_array/magic_enum.hpp
jbc/indexed_array/wise_enum.hpp
jbc/indexed_array/reflection.hpp
```

## 📚 Documentation

* See [Online doc](https://julien-blanc-tgcm.github.io/indexed_array/)

## 💡 Examples

### Basic indexing with an offset
```cpp
// we want an array of 10 strings, index starting at one
using MyArray = indexed_array<std::string, interval<1, 10>>;

MyArray arr;
arr[1] = "First element of the array";

// this one will be indexed by an enum value
using AnotherArray = indexed_array<std::string, interval<my_enum::first, my_enum::last>>;
```

### Indexing via enums
```cpp
enum class Color { Red, Green, Blue };
using ColorArray = indexed_array<std::string, interval<Color::Red, Color::Blue>>;
ColorArray colors;
colors[Color::Red] = "Red";
```

### Indexing via full enum range 

*requires C++26 or an enum reflection library, see below)*

```cpp
enum class Color { Red, Green, Blue };
using ColorArray = indexed_array<std::string, Color>;
ColorArray colors;
colors[Color::Red] = "Red";
assert(arr.size() == 3); // 3 elements
```

### Non-contiguous indexers
```cpp
using MixedIndexes = union_of<
    interval<1, 5>,
    single_value<10>,
    single_value<20>
>;
using MixedArray = indexed_array<int, MixedIndexes>{};
MixedArray arr;
arr[1] = 100;
arr[10] = 200;
arr[20] = 300;
assert(arr.size() == 7); // 7 elements in array
assert(sizeof(arr) == 7 * sizeof(int)); // no holes, only the required size is used
```

### Indexed bitsets

```cpp
enum class Permission { Read, Write, Execute };
using PermissionBitset = indexed_bitset<Permission>>;
PermissionBitset perms{};
perms.set(Permission::Read);
if (perms.test(Permission::Write)) { /* ... */ }
```

### Indexed span

```cpp
enum class Color { Red, Green, Blue };
std::array<int, 2> arr{1, 2, 3};
indexed_span<int, Color> sp(arr.data()); // fixed extent, no size parameter
assert(sp.size() == 3);
assert(sp[Color::Green] == 2);
```

### Multidimensional

```
enum class Color { Red, Green, Blue};
indexed_array<int, invertal<0, 10>, Color> arr;
arr[5, Green] = 15;
assert(arr.size() == 30);
```

## ⚠️ Limitations

- **Static Indexes Only**: All indexes must be known at compile time.
- **No Dynamic Resizing**: The size of an `indexed_array` is fixed at compile time.
- **Performance**: For non-contiguous indexes, access by index is no longer O(1). This can be checked at compile time to ensure
  that there is no overhead.

## 📜 Licence

Distributed under the
[Boost Software License, Version 1.0](http://boost.org/LICENSE_1_0.txt).

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

- **Code Style**: This project uses `clang-format`. Run `clang-format -i file.hpp` to format your code.
- **Tests**: Add tests for new features in the `tests/` directory.
- **Documentation**: Add doc for new features in the `docs/` directory.
- **Pull Requests**: Open a PR against the `main` branch and ensure all tests pass.
- **Issues**: Report bugs or feature requests via [GitHub Issues](https://github.com/Julien-Blanc-tgcm/indexed_array/issues).

