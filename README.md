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

## Sample use cases

### Associating messages with HTTP status codes:

Using `indexed_array`, we can easily create a map between HTTP codes and plain text
messages. This map will have an array-like storage, and can reside in read-only memory.

```cpp
enum class http_status {
	success = 200,
	moved_permanently = 301,
	found = 302,
	bad_request = 400,
	unauthorized = 401,
	forbidden = 403,
	not_found = 404,
	internal_error = 500
};

BOOST_DESCRIBE_ENUM(http_status, ...)

indexed_array<char const*, http_status> http_messages {
	safe_arg<http_status::success>("Ok"),
	safe_arg<http_status::moved_permanently>("Moved permanently"),
	safe_arg<http_status::found>("Found"),
	safe_arg<http_status::bad_request>("Bad request"),
	safe_arg<http_status::unauthorized>("Unauthorized"),
	safe_arg<http_status::forbidden>("Forbidden"),
	safe_arg<http_status::not_found>("Page not found"),
	safe_arg<http_status::internal_error>("Internal error")
};
```

We can, of course, associate nearly anything this way, including function objects, which could be used to
further customize a reply.

One important feature is that if someone decide that we now must handle the `304 NotModified` status code,
and add it to the `http_status` enum, the `http_messages` array will now fail to compile until we add the
relevant error message.

### Storing base addresses for different devices

We're writing a driver for a chip who support four `i2c` devices.

```cpp
enum class i2c_id {
	i2c1,
	i2c2,
	i2c3,
	i2c4
};

enum class i2c_register {
	set_clock,
	read,
	write,
	ack,
	...
};
```

Now we can store the base address of the i2c device like this:

```cpp
indexed_array<std::uint32_t, i2c_id> i2c_base_address {
	safe_arg<i2c_id::i2c1>(0x1060000u),
	safe_arg<i2c_id::i2c2>(0x1070000u),
	safe_arg<i2c_id::i2c3>(0x1080000u),
	safe_arg<i2c_id::i2c4>(0x1090000u),
};

// and the offsets like that
indexed_array<std::uint32_t, i2c_register> i2c_register_offset {
	...
};
```

And access it like that in the code of the driver:

```cpp
// current_device is an i2c_id
// i2c_register_offset
auto address = i2c_base_address[current_device] + i2c_register_offset[i2c_register::set_clock];
// write the relevant value at address to set the clock of the i2c chip
```

If we now decide to support a new chip in our driver, which has slightly different base address and
offsets, we can now write:

```cpp
enum class mcu_chip {
	fictive_chip_revA,
	fictive_chip_revB
};

indexed_array<std::uint32_t, mcu_chip, i2c_id> i2c_base_address {
	safe_arg<mcu_chip::fictive_chip_revA, i2c_ip::i2c1>(0x1060000u),
	safe_arg<mcu_chip::fictive_chip_revA, i2c_ip::i2c2>(0x1070000u),
	safe_arg<mcu_chip::fictive_chip_revA, i2c_ip::i2c3>(0x1080000u),
	safe_arg<mcu_chip::fictive_chip_revA, i2c_ip::i2c4>(0x1090000u),
	// i2c1 address has changed in this revision
	safe_arg<mcu_chip::fictive_chip_revB, i2c_ip::i2c1>(0x1B60000u), 
	// others untouched
	safe_arg<mcu_chip::fictive_chip_revB, i2c_ip::i2c2>(0x1070000u), 
	safe_arg<mcu_chip::fictive_chip_revB, i2c_ip::i2c3>(0x1080000u),
	safe_arg<mcu_chip::fictive_chip_revB, i2c_ip::i2c4>(0x1090000u),
}
```

And use it like that in our driver

```cpp
auto address = i2c_base_address[{current_chip, current_device}] + 
               i2c_register_offset[{current_chip, i2c_register::set_clock}];
```

### Strongly typed index for multidimensional arrays

Let's say we have the following class, with multiple 3-dimensional array:

```cpp
// xyz indexing
std::array<std::array<std::array<float, 100>, 200>, 300> initial_data;
// beware: for performance reason, this array is indexed differently (z is first, zyx)
std::array<std::array<std::array<float, 300>, 200>, 100> calculation_result;
```

So, we now have a recipe for disasters. Two arrays with different indexing schemes at
the same place. We assume a good design, this is not visible in the public interface. But
what about preventing mistakes when someone touches the implementation? Making it impossible
to write something like:

```cpp
float get_calculation_result(int coord_x, int coord_y, int coord_z)
{
	return calculation_result[coord_x][coord_y][coord_z]; // oops
}
```

Let's make that more explicit:

```cpp
enum class x_coordinate : std::size_t {}; // strongly typed alias
enum class y_coordinate : std::size_t {};
enum class z_coordinate : std::size_t {};

indexed_array<float, interval<x_coordinate{0}, x_coordinate{299}>, 
                     interval<y_coordinate{0}, y_coordinate{199}>,
                     interval<z_coordinate{0}, z_coordinate{99}>> initial_data;
indexed_array<float, interval<z_coordinate{0}, z_coordinate{99}>, 
                     interval<y_coordinate{0}, y_coordinate{199}>,
                     interval<x_coordinate{0}, x_coordinate{299}>> calculation_result;

// for convenience, we add this into our cpp file
using x = x_coordinate;
using y = y_coordinate;
using z = z_coordinate;
```

Now, accesses to array content must be done using the following syntax:

```cpp
float get_calculation_result(int coord_x, int coord_y, int coord_z)
{
	// return calculation_result[{x{coord_x}, y{coord_y}, z{coord_z}}]; // does not compile
	return calculation_result[{z{coord_z}, y{coord_y}, x{coord_x}}]; // we fixed the bug
}
```

This check is done purely at compile time, and will not incurs any runtime cost.

## Integration with enum reflection libraries

Out of the box, `indexed_array` has support for the following enum reflection libraries:

* [Boost Describe](https://www.boost.org/doc/libs/1_79_0/libs/describe/doc/html/describe.html)
* [Magic Enum](https://github.com/Neargye/magic_enum)
* [Wise Enum](https://github.com/quicknir/wise_enum)

To enable the integration, you must define the relevant `INDEXED_ARRAY_HAS_[DESCRIBE|MAGIC_ENUM|WISE_ENUM]` macro
when compiling.

## Supported Compilers

* GCC 9 and later
* Clang 11 and later

(Not tested with other compilers, but should work with any `C++17` compliant
compiler)

## Requirements

* `boost::mp11` library
* `boost::describe` library for described enums support (can be disabled at
  compile time)

## Documentation

* See [Online doc](https://julien-blanc-tgcm.github.io/indexed_array/)

## Licence

Distributed under the
[Boost Software License, Version 1.0](http://boost.org/LICENSE_1_0.txt).
