<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Advanced usage

## Non-contiguous indexes

`indexed_array` supports a special form of indexes, which are non-contiguous. An example of
a non-contiguous index is the sequence `{1, 2, 4, 8}`. This sequence can be mapped to
`{0, 1, 2, 3}` which can then be used as the internal index for the internal array.

Using non-contiguous indexes with integer values is highly non-recommended. But using them
with enums addresses a few uncommon use cases in a simple and elegant way.

An important note to consider is that when using non-contiguous indexes, access to the
elements in the `indexed_array` is no longer `O(1)` but becomes `O(N)`. This is because the
indexer must iterate through all values, incrementing the inner index, to retrieve the
corresponding value.

`indexed_array` provides the static member `is_o1` which is true if the `indexed_array` has `O(1)`
access, and false otherwise. This allows putting a `static_assert` in case `O(1)` access is a
requirement, or to enable some optimizations based on this property.

### Integer sequences

The library provides the template class `value_sequence`, which is very similar to
`std::integer_sequence`, but does not require the type parameter to be an integer-like
type. The library uses this class for indexing using a sequence of values, and provides
automatic conversion from `std::integer_sequence`, so that we can write:

```
using T = indexed_array<int, std::integer_sequence<int, -1, 0, 1, 2, 3>>;
assert(T::size() == 5);
```

This works as well if the values are not contiguous:

```
using T = indexed_array<int, std::integer_sequence<int, 1, 2, 4, 8>>;
assert(T::size() == 4);
```

### Non contiguous enum values

Sometimes enum values are not contiguous, because some value is reserved or no longer in use.

```cpp
enum class MotorType
{
	LocalManufacturer = 1,
	// we no longer work with BadManufacturer, we don't handle the value in this product
	// but it is reserved to avoid confusion, other products still uses it
	// BadManufacturer = 2,
	RemoteManufacturer = 3,
	GoodManufacturer = 4
};
```

With compile-time reflection available on the enum, one can simply write:

```
indexed_array<Configuration, MotorType> defaultConfiguration = /* initialize */;
```

This declare an array whose size is **3**:

```
assert(&defaultConfiguration[MotorType::LocalManufacturer] == begin(defaultConfiguration))
assert(&defaultConfiguration[MotorType::RemoteManufacturer] == begin(defaultConfiguration) + 1);
assert(&defaultConfiguration[MotorType::GoodManufacturer] == begin(defaultConfiguration) + 2);
```

If the enum has no compile-time reflection available, we can still use it:

```
using MotorTypeList = value_sequence<MotorType,
    MotorType::LocalManufacturer,
    MotorType::RemoteManufacturer,
    MotorType::GoodManufacturer>;
indexed_array<Configuration, MotorTypeList> defaultConfiguration = /* ... */;
```

The two arrays are identical.

### Union of values

Sometimes we're interested in a subset of the values, which may also be non-contiguous. Let's
take back our example from the lift industry:

```
enum class Floor : uint8_t {
	Car = 0,
	F1 = 1,
	F2 = 2,
	F3 = 3,
	... // the norm defines up to 254 Floors
	F254 = 254,
	All = 255
};
```

In real life, no lift ever has so many floors. We only use a subset of the floors, for example 
`[F1..F32]` or `[F1..F64]`. We already covered that case earlier, using an interval. But lifts
also have some special *floors*, such as the machinery or the pit (beneath the lowest user level).
So, we modify our enum accordingly:
```
...
	Machinery = 240,
	Pit = 241,
	/* ... other values are reserved for extensions */
	All = 255
};
```

We have some fire detectors, whose state (`Off`, `On`, `Unknown`) we must store in our program.
There is a detector at each floor, in machinery and in pit. But there is none inside the car. To
store this data, we can declare the following:

```
indexed_array<FiredetectorState, 
              union_of<
                  interval<Floor::F1, Floor::F32>,
                  single_value<Floor::Machinery>,
                  single_value<Floor::Pit>>> firedetectors;
```

This declares an `indexed_array` which contains **34** items (32 common floors + 2 technical ones). All
items are contiguous in memory and there is no room wasted:
`sizeof(firedetectors) == 34 * sizeof(FiredetectorState)`.

The layout is the following:

```
+-----+-----+-----+-...-+------+------+------+------+
| VF1 + VF2 + VF3 + ... + VF30 + VF31 + VMac + VPit +
+-----+-----+-----+-... +------+------+------+------+
```

### Name aliases

Sometimes enums are defined this way, to provide poor-man's reflection:

```cpp
enum class MyEnum
{
	first,
	AValue = first,
	Another,
	Foo,
	last = Foo// if adding new values, update this
};
```

This allows us to declare
```
indexed_array<int, interval<MyEnum::first, MyEnum::last>> arr;
assert(arr.size() == 3);
```

## Custom indexes

If needed, the library user can provide its own indexing mechanism. An indexer
must provide the following members:
* an `index` typedef, which is the type used in the public interface
* a static `std::size_t size` member, which is the size of the container
* a static `template<bool b> size_t at(index)`, which does the actual
translation between interface index and internal container index. The boolean is
here to distinguish between throwing (`b` is `true`)/ non-throwing (`b` is `false`)
versions. The indexer shall raise an `out_of_range` exception when `b` is `true` and
it is given an invalid index. This method may be `noexcept(!b)`.
* a static `is_o1` boolean value, which tells if the indexer is `O(1)`. This is optional,
but is provided by standard indexers, and may be used to optimize some algorithms.
* a static `in_range(index)` boolean method, which returns true if the given index
is in the range of possible values (ie, `at(index)` won't throw).

This is an unrealistic example of a custom index:

```cpp
struct reverse_fibonnaci_index
{
	using index = int;
	inline static constexpr size_t const size = 5;
	template <bool b>
	static constexpr auto at(index v) noexcept(!b)
	{
		if (v == 1)
			return 0;
		if (v == 2)
			return 1;
		if (v == 3)
			return 2;
		if (v == 5)
			return 3;
		if (v == 8)
			return 4;
		if constexpr(b)
			throw std::out_of_range("invalid index");
		return -1; // this value may be catched by other tools when accessing underlying array
	}
	static constexpr bool is_o1 = false;
	static constexpr bool in_range(index v) noexcept
	{
		return at<false>(v) != -1;
	}
};
```

Which can then be used as:
```
indexed_array<std::string, reverse_fibonnaci_index> data; // data.size() == 5
```

In real life, a reason to write a custom index is performance. Let's take back our lift floor example.
Accessing an element inside the container is now O(n), due to the way the indexer works. But it could be
made faster, something like:
```
template <bool b>
static constexpr auto at(index floor) noexcept(!b)
{
	if (v == Floor::Machinery)
		return size() - 2;
	if (v == Floor::Pit)
		return size() - 1;
	if constexpr (b)
	{
		if(floor >= Floor::F1 && floor <= Floor::F32)
			return static_cast<size_t>(floor) - 1;
		throw std::out_of_range("invalid index");
	}
	return static_cast<size_t>(floor) - 1;
```

Back to the [index](index.md) or continue to [multidimensional indexing](multidimensional.md)
