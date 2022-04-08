<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Safe Initialization

Safe Initialization is a feature of the library that allows compile time
checking of the aggregate initialization. An example being better than
a long explanation:

## Example

```cpp
BOOST_DEFINE_ENUM_CLASS(Color, red, blue, green, black);
indexed_array<int, interval<Color>> values { 
    0xFF0000,
    0x00FF00, // whoops
    0x0000FF, // whoops
    0x0 };
```

Now with safe initialization:

```cpp
indexed_array<int, Color> values {
    safe_arg<Color::red>(0xFF0000),
    safe_arg<Color::green>(0x00FF00),
    safe_arg<Color::blue>(0x0000FF),
    safe_arg<Color::black>(0x0)}; // fails to compile
```

This actually fails to compile because the arguments are in wrong order.
The number of arguments is also checked:

```cpp
BOOST_DEFINE_ENUM_CLASS(Color, red, blue, green, black, white); // someone added a value here
indexed_array<int, Color> values {
    safe_arg<Color::red>(0xFF0000),
    safe_arg<Color::blue>(0x0000FF),
    safe_arg<Color::green>(0x00FF00),
    safe_arg<Color::black>(0x0)}; // fails to compile, missing initializer for "white"
```

Fully fixed version:
```cpp
BOOST_DEFINE_ENUM_CLASS(Color, red, blue, green, black, white); // someone added a value here
indexed_array<int, Color> values {
    safe_arg<Color::red>(0xFF0000),
    safe_arg<Color::blue>(0x0000FF),
    safe_arg<Color::green>(0x00FF00),
    safe_arg<Color::black>(0x0),
    safe_arg<Color::white>}; // compiles without error
```

## Safe Arg

`safe_arg` is a tag type, that allows static checking of order of arguments. The constructor 
of `indexed_array` is specialized for `safe_arg`, and will assert that:

* the number of arguments passed is equal to `indexer::size`
* for each argument `safe_arg<i>`, the result of `indexer::at(i)` is equal to its position
in the constructor argument list, starting at 0.

## Usage with complex indexers

`safe_arg` is compatible with any indexer, and fully supports holes and duplicate values, 
which, since they are ignored, should not be provided more than once in the initializer
list (failure to do so will result in a compile time error).
