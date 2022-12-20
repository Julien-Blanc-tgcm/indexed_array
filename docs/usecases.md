<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Use cases

## Objects in a CANOpen dictionary

The structure of an array-object in a CANOpen dictionary is the following (for an
Integer32 array):

```
SubIndex0: Integer8 // contains the number of subindexes
SubIndex1-n: Integer32 // contains the array of data
```

It uses a one-based index for the data. It is possible to use an indexed
array starting at 1 to store the data that will be accessible in the CANOpen
dictionary, which will greatly improve the readability of the code as the same
indexing scheme is used both internally and externally.

### Migrating code from other languages that uses 1-based indexing

Similarly, when migrating old code from a language that uses 1-based indexing (Visual Basic comes to
mind), using `indexed_array` can provide a great help, in that it allows keeping the
algorithms untouched, by using the same indexing scheme as the one they were originally
written with.

## Associating messages with HTTP status codes:

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

One important feature is that if someone decide that we now must handle the `304 NotModified` status code,
and add it to the `http_status` enum, the `http_messages` array will now fail to compile until we add the
relevant error message.

## Handling both standard and manufacturer specific values

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
BOOST_DESCRIBE_ENUM(...);
```

Later in the code:

```
indexed_array<char const*, SomeEnum> vocalMessage {
	safe_arg<SomeEnum::Foo>("foo.mp3"),
	safe_arg<SomeEnum::Bar>("bar.mp3"),
	...
	safe_arg<SomeEnum::Blah>("blah_v2.mp3"),
	safe_arg<MSExtension1>("outputactive.mp3"),
	...
	};
```

The usage of `safe_arg` here ensures that if someone modifies the enum (because a new
revision of the standard added a new value after Blah), the `vocalMessage` array will
fail to compile. This makes it easy to spot all places in the code which will be
inpacted by the code change.

## A faster `enum_to_string`

Describe provides an helper function to convert an enum to a string. This function, however,
iterate through all enums, not taking the advantage of the contiguity of enum values, if
applicable. We can thus make it a bit faster, by using an indexed array to store the strings
associated to each enum value.

First we need to create the list of values:
```
template <class E, template<class... T> class L, class... T>
constexpr jbc::indexed_array::indexed_array<char const*, E> describe_enumerators_as_indexed_array_impl(L<T...>)
{
	return jbc::indexed_array::indexed_array<char const*, E>{T::name...};
}
template <typename T>
struct to_string_helper
{
	static constexpr jbc::indexed_array::indexed_array<char const*, E> describe_enumerators_as_indexed_array
	    {describe_enumerators_as_indexed_array_impl<E>(boost::describe::describe_enumerators<E>())};
};
```
And then use that. But we want to use that only if `indexed_array<char const*, E>` is `O(1)`, otherwise,
there won't be any benefit. If that's not the case, we fallback to normal formatting via describe. We use
the static `is_o1` property to enable the overload only when it will gives a performance boost:
```
template<class T>
char const* to_string(T e, typename std::enable_if<
    boost::describe::has_describe_enumerators<T>::value && 
    jbc::indexed_array::indexed_array<char const*, T>::is_o1, T>::type = {})
{
	if (jbc::indexed_array::indexed_array<char const*, T>::indexer::in_range(e))
		return to_string_helper<T>::describe_enumerators_as_indexed_array[e];
	return "<ERROR>";
};

template<class T>
char const* to_string(T e, typename std::enable_if<
    boost::describe::has_describe_enumerators<T>::value && 
    jbc::indexed_array::indexed_array<char const*, T>::is_o1, T>::type = {})
{
	return enum_to_string(e);
};
```

The overall performance improvement is about 10 to 15% with gcc or clang for a common enum (around 10 values). The code may also smaller, especially
when the enum has a lot of values (around 25% smaller with an enum with thirty values). The bigger the 
number of values in the enum, the bigger the gains.

## Storing base addresses for different devices

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

Now we can store the base address of each i2c device like this:

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

## Strongly typed index for multidimensional arrays

Let's say we have the following class, with multiple 3-dimensional array:

```cpp
// xyz indexing
std::array<std::array<std::array<float, 100>, 200>, 300> initial_data;
// beware: for performance reason, this array is indexed differently (z is first, zyx)
std::array<std::array<std::array<float, 300>, 200>, 100> calculation_result;
```

So, we now have a recipe for disasters. Two arrays with different indexing schemes at
the same place. We assume a good design, so this will not be visible in the public interface. But
what about preventing mistakes when someone touches the implementation? Making it impossible
to write something like:

```cpp
return calculation_result[coord_x][coord_y][coord_z]; // oops
```

Let's make that more explicit:

```cpp
Enum class x_coordinate : std::size_t {}; // strongly typed alias
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
// return calculation_result[{x{coord_x}, y{coord_y}, z{coord_z}}]; // does not compile
return calculation_result[{z{coord_z}, y{coord_y}, x{coord_x}}]; // we fixed the bug
```

This check is done purely at compile time, and will not incurs any runtime cost.

Back to the [Index](index.md), or continue to [Reference](reference.md)
