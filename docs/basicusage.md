<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Basic Usage

The library defines a single type, `indexed_array`, which takes two template 
parameters:
* the type of the object stored in the container (like for a standard array)
* an indexer, which is in charge of providing the size of the container, as well
as doing index translation between the container interface and the internal
array.

The library provides a `default_indexer` which should be suitable for most use
cases. Users can defines custom indexers, see [Custom Indexer](customindexer.md).

## Usage with arbitrary intervals

The library has built-in support for intervals, using the `interval` helper.

```cpp
indexed_array<std::string, interval<4, 7>> data; // data.size() == 4
data[4] = "4";
data[5] = "5";
data[6] = "6";
data[7] = "7";
// data[8] = "8"; undefined behaviour
try {
	data.at(8) = "8";
} catch(std::out_of_range&) {
	// should be catched
}
```

Negative intervals are supported as well
```cpp
indexed_array<std::string, interval<-2, 0>> data; // data.size() == 3
```

## Usage with enums

Enums are supported as well as intervals. So, given the following enum:

```cpp
enum class Foo {
	bar1 = 3,
	bar2,
	bar3,
	bar4,
	bar5
};
```

One can write:

```cpp
indexed_array<std::string, interval<Foo::bar1, Foo::bar5>> data; // data.size() == 5
indexed_array<std::string, interval<Foo::bar2, Foo::bar4>> data; // data.size() == 3
```

## Usage with described enums

If the enum has compile-time introspection using `boost::describe`` library, one can
simply write:

```cpp
BOOST_DEFINE_ENUM_CLASS(Foo, bar1, bar2, bar3, bar4, bar5);
indexed_array<string, Foo> data; // data.size() == 5
```

This also works if the enum has “holes”, like in the following example:

```cpp
enum E
{
	v1 = 1,
	v2 = 2,
	v3 = 4,
};
BOOST_DESCRIBE_ENUM(E, v1, v2, v3)
indexed_array<string, E> data; // data.size() == 3
```

## Usage with integer lists

The container can be used with any arbitrary list as an index. However, if an
index is used more than once in the list, it is silently dropped (this makes
it possible to support aliases in enum in a pretty straightforward way).

It is highly recommended to use [Safe Initialization](safeinitialization.md) when using non contiguous
index schemes.

```cpp
using l = std::index_sequence<5, 3, 1, 2, 4, 0, 0, 8>;
indexed_array<char, l> t{'0', '1', '2', '3', '4', '5', '6'}; // t.size() == 7
for(auto i : t) cout << i; // will output "0123456"
for(int i = 0; i < 6, i++) // will output 
	cout << t[i]; // will output "523140"
cout << t[8]; // will output "6"
cout << t.at(7); // will throw out_of_range
```

