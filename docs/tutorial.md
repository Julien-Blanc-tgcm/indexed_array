<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Tutorial

## Using the library

The library is header-only. Adding `indexed_array/include` to your
include path, and 

```cpp
#include <jbc/indexed_array.hpp>
```

should be enough to use the library. To use describe enum reflection integrations, it is
necessary to add the following compile definition: `INDEXED_ARRAY_HAS_DESCRIBE`, or to
include `jbc/indexed_array/describe.hpp` manually.

Compiling must be done using at least `C++17` mode. For clarity, the examples in this
documentation assume a `using namespace jbc::indexed_array` directive, and a
`using namespace std` as well.

## A simple example with describe

Usage with a `describe`-d enum is pretty straightforward. You declare an `indexed_array`
the same way you would declare an `std::array`, but the size parameter is replaced by the
*type* used as the index.

```cpp
BOOST_DEFINE_ENUM_CLASS(Foo, bar1, bar2, bar3, bar4, bar5);

indexed_array<string, Foo> data; // data.size() == 5

assert(data[Foo::bar1].empty()); // default constructed

for (auto& s : data)
{
	s = "Hello";
}

assert(data[Foo::bar3] == "Hello");
```

This declares an array of five string objects, which can be accessed using Foo
values only (trying to use a plain integer, or any other type, will fail with
a compile-time error).

All `std::array` operations are supported.

## Using a numeric range (interval)

Using a numeric range as the indexer allows using non-zero starting index, like
available in several languages (Fortran, Ada, Algol, etc.). The syntax used is
similar as for `describe`-d enums, but this time the type passed must be an
`interval<X, Y>` where `X` is the lower bound (inclusive) and `Y` the higher
bound (inclusive).

For example:

```cpp
indexed_array<char, interval<0, 5>> data; // data.size() == 6;
data[0] = 'H';
data[1] = 'e';
data[2] = 'l';
data[3] = 'l';
data[4] = 'o';
data[5] = '\0';
assert(string(data.data()) == "Hello")
```

We have declared something that contains 6 items, contiguous in memory, and these
items can be accessed using `0` to `5` integer values. That is, something that behaves
a lot like a plain `std::array<char, 6>`. Let's change this a bit:

```cpp
indexed_array<char, interval<1, 6>> data; // data.size() == 6;
data[1] = 'H';
data[2] = 'e';
data[3] = 'l';
data[4] = 'l';
data[5] = 'o';
data[6] = '\0';
assert(string(data.data()) == "Hello")
```

The difference here lies in the way items are retrieved. Valid indexes now range from
`1` to `6` (inclusive). Accessing `data[0]` is now undefined behaviour, and `data.at(0)`
will thow `std::out_of_range`.

The following are valid declarations:

```cpp
indexed_array<std::string, interval<-12, -3>> d1;        // data.size == 10
indexed_array<char, interval<-3, 6>> d2;                 // data.size() == 10
indexed_array<std::unique_ptr<int>, interval<10, 19> d3; // data.size() == 10
// since interval is inclusive, this declare a one-sized array
indexed_array<int, interval<4, 4>> d4;                   // data.size() == 1
indexed_array<char, interval<2ULL, 8ULL>>;               // data.size() == 7
```

The following declarations are invalid:
```cpp
indexed_array<char, interval<4, 2>> d1;    // fails, interval must be [min,max] with min <= max
indexed_array<char, interval<-2, 4U>> d2;  // fails, interval must use both integers of same type
indexed_array<char, interval<-2, 4LL>> d3; // fails, interval must use both integers of same type
```

## Using an enum range (interval)

If there is no static enum reflection data, or if you want to use a subset of the enum values, you
can use an interval as well. Given the following enum declaration:

```cpp
enum class LogComponent {
	All = -1,
	WebServer = 0,
	Templating = 1,
	Worker = 2
};
```

The following declaration is valid:
```cpp
constexpr indexed_array<string_view, interval<LogComponent::WebServer, LogComponent::Worker>> 
logprefixes = {
	"<<webserver>>",
	"<<template >>",
	"<<worker   >>"
};
```

And can be used like this

```cpp
template <LogComponent component, typename... Args>
void log(Args&& args)
{
	log_string(logprefixes[component], std::forward<Args>(args...));
}
```

The size of the array is `3`, and calling `logprefixes[LogComponent::All]` is undefined behaviour.

The same rules apply when using integer intervals or enum intervals.

## Common operations

`indexed_array` supports all `std::array` operations, with the same semantic.

Iterations:
```cpp
for (auto prefix : logprefixes)
{
	std::cout << prefix << std::endl;
}
```

Will output:
```cpp
<<webserver>>
<<template >>
<<worker   >>
```

Reverse iteration is supported as well.

Fill/swap
```cpp
indexed_array<int, interval<1, 10>> d1, d2;
d1.fill(10);
d2.swap(d1);
assert(d2[1] == 10);
```

Bound checked or unchecked access
```cpp
indexed_array<int, interval<1, 10>> d1;
d1.at(11) = 10; // throws std::out_of_range
d1.at(0) = 0; // throws std::out_of_range
d1[0] = 0; // undefined behaviour
d1[123] = 0; // undefined behaviour
```

## Iterating both keys and values

The library offers an helper function that allows to iterate through both the keys and the values.
This is not a real iteration, as the keys are known at compile-time, not run-time. This is more
a convenience helper:

```cpp
indexed_array<int, interval<1, 10>> d1;
for_each(d, [](int key, int& val)
{
	val = key;
}
assert(d[2] == 2);
```

## Multidimensional indexing

The library also supports multidimensional indexing. Declaring a multidimensional indexed array
is very similar to declaring a single dimension:

```
indexed_array<int, interval<1, 4>, Foo> data;
assert(data.size() == 4 * 5); // 5 is the number of enum values in Foo
```

Accessing the elements can de bone by two ways:
```
int v1 = data.at(2, Foo::bar1);
int v2 = data[{2, Foo::bar1}];
```

The data layout is the following:

| 1, Foo::bar1 | 1, Foo::bar2 | ... | 1, Foo::bar4 |
| 2, Foo::bar1 | 2, Foo::bar2 | ... | 2, Foo::bar4 |
| 3, Foo::bar1 | 3, Foo::bar2 | ... | 3, Foo::bar4 |
| 4, Foo::bar1 | 4, Foo::bar2 | ... | 4, Foo::bar4 |

It is also possible to get a view on a single row:

```
auto second_row = data[2];
int v3 = second_row[Foo::bar3];
```

There is no limit on the number of dimensions.

*Note: iteration on both keys and values is currently not implemented for multidimensional arrays*


Back to [index](index.md), or see [advanced usage](advancedusage.md)
