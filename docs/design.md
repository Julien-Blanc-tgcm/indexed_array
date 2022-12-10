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
template <typename Value, typename Indexer>
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

It resides in the namespace `indexed_array::detail`, and a smart using directive is used in the namespace
`indexed_array` to allow using `indexed_array::detail::indexed_array` with the correct template arguments.

## The default\_indexer type

Now that we have a basic generic array type, most of the work resides in providing indexers. The library
provides an implementation, called `default_indexer`, which should be suitable for most purposes, and a
few helper types used to customize the `default_indexer`.

```cpp
template <typename Index, typename T = void>
struct default_indexer
{ // default implementation is empty
};
```

In all indexers, to avoid code duplication, the `at` method is templated on a boolean parameter, which will tell if
it needs to do bound check or not. It shall throw an `std::out_of_range` exception if requested to
do bound checking, and the index given is invalid (`indexed_array` won't do bound check when accessing
the inner array).

### Handling integers and enum the same way

In several situations (index computations), we want to handle integral types and enum types the same way. So,
one of the first thing we do is define the following helper:

```cpp
template <typename T, T value, typename U = void>
struct integral_value
{
};

template <typename T, T v>
struct integral_value<T, v, std::enable_if_t<std::is_integral<T>::value, void> >
{
	static inline constexpr T value = v;
};

template <typename T, T v>
struct integral_value<T, v, std::enable_if_t<std::is_enum<T>::value, void> >
{
	static inline constexpr typename std::underlying_type<T>::type value =
	    static_cast<typename std::underlying_type<T>::type>(v);
};

template<auto C>
static inline constexpr auto const integral_value_v = integral_value<decltype(C), C>::value;
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

The `describe` library is a compile time reflection library designed by Peter Dimov, and is part of `boost`.
The `indexed_library` provides first class integration for enums who have compile time introspection using
this library.

To implement that, the required part is to convert a `describe_enumerators<Enum>` to an 
`integer_sequence<Enum, Values...>`. After that is done, we can simply use the `default_indexer` for
integer sequences. We benefit automatically from the optimization for contiguous sequences (supporting
this use case was the main motivation for this optimization in the first place).

Doing this conversion is pretty easy, and integrating enums is as simple as:

```cpp
template <typename... Args>
struct describe_to_integer_sequence
{
};

template <typename Enum, template <class...> typename L, typename... Args>
struct describe_to_integer_sequence<Enum, L<Args...> >
{
	using type = std::integer_sequence<Enum, Args::value...>;
};

template <typename Enum>
struct default_indexer<Enum, typename std::enable_if_t<boost::describe::has_describe_enumerators<Enum>::value, void> >
{
	using helper_list_type = typename describe_to_integer_sequence<Enum, describe::describe_enumerators<Enum> >::type;
	using index = Enum;
	static inline constexpr auto const size = default_indexer<helper_list_type>::size;
	template <bool throws_on_error = false>
	static constexpr auto at(Enum v) noexcept(!throws_on_error)
	{
		return default_indexer<helper_list_type>::template at<throws_on_error>(v);
	}
};
```

Adding support of other compile-time enum introspection mechanisms should be relatively simple as well.

## Going multidimensional

### Adapting indexed\_array

Now that we have a container that can be arbitrarily indexed, it makes a lot of sense to have some indexes, not
in the form of just one value, but in the form of a set of values. As long as these values can be safely
converted into an integer in the range `[0 - (size -1)]`, everything is fine. This can be done easily by
defining the following indexer:

```cpp
struct custom_multidimensional_index
{
	using index = mp11::mp_list<int, Foo>;
	inline static constexpr size_t const size = 8;
	template <bool c = true>
	static constexpr auto at(int v1, Foo v2)
	{
		auto res = v1 * 4 + static_cast<int>(v2) + 1;
		if constexpr(c)
		{
			if(res < 0 || static_cast<std::size_t>(res) >= size)
				throw std::out_of_range("Invalid index");
		}
		return res;
	}
};
```

The problem resides with the definition of the `at()` and `operator[]` methods. They can't take an `Indexer::index`
anymore. We need to split that into separate arguments, so that we can write:

```cpp
auto value = my_multidim_array.at(2, Foo::bar);
auto value2 = my_multidim_array[{1, Foo::bar}]; // notice the {}, won't be needed in C++23
```

To allow this, the trick here is to inherit from a base class, that we can specialize, and which will define these operators.

```cpp
template <typename Value, typename Indexer, typename Owner, typename Index>
class indexed_array_helper
{
	// ...
	constexpr reference at(Index index);
};
```

And we specialize it when `index` is a typelist:

```cpp
template <typename Value, typename Indexer, typename Owner, template <class...> class Index, typename... Args>
class indexed_array_helper<Value, Indexer, Owner, Index<Args...> >
{
	constexpr reference at(Args... index);
}
```

This allows to inject the correct interface into `indexed_array` (see the code for more implementation details).

### Adapting default\_indexer

The default indexer now needs to be adapted, so that we can write:

```cpp
indexed_array<string, range<4, 8>, my_custom_index, Foo> my_data;
```

What we need is just a bit of machinery to transform that into:
```cpp
indexed_array<string, 
             default_indexer<mp_list<
                 default_indexer<range<4,8>>,
                 my_custom_index,
                 default_indexer<Foo>
             >>>
```

And then specialize default\_indexer when taking an mp\_list of indexers:

```cpp
template <typename... Args>
struct default_indexer<boost::mp11::mp_list<Args...>,
                       typename std::enable_if_t<boost::mp11::mp_all<has_member_size<Args>...>::value, void> >
{
	using index = boost::mp11::mp_list<typename Args::index...>;
	static inline constexpr auto const size = product_v<Args::size...>;

	template <bool throws_on_error = false>
	static constexpr auto at(typename Args::index... args) noexcept
	{
		return at_computation_helper<Args...>::template at<throws_on_error>(args...);
	}
};
```

`product_v` is a compile time helper that will gives the product of all given values. `at` function is
implemented in a quite classical way:

```cpp
template <typename Arg, typename... Args>
struct at_computation_helper
{
	template <bool c>
	static constexpr auto at(typename Arg::index idx, typename Args::index... rem)
	{
		return Arg::template at<c>(idx) * product_v<Args::size...> +
		       at_computation_helper<Args...>::template at<c>(rem...);
	}
};
template <typename Arg>
struct at_computation_helper<Arg>
{
	template <bool c>
	static constexpr auto at(typename Arg::index idx)
	{
		return Arg::template at<c>(idx);
	}
};
```

## Safe initialization

The `safe_arg` template class is used, during initialization, to assert that:

* the correct number of arguments is given for the array (not less, not more)
* the order of the arguments matches the one of the indexer

So, we got two assertions to check:

* `at(idx) == <index in initializer list>` (for each item in the list)
* `Indexer::size == <size of initializer list>`

The first assertion is checked recursively for each argument:

```cpp
template <typename Indexer, std::size_t X, typename u, typename... v>
constexpr bool correct_index_()
{
	static_assert(X == at_helper<Indexer, u>::at(), "Invalid value for initializer");
	return X == at_helper<Indexer, u>::at() && correct_index_<Indexer, X + 1, v...>();
}
```

A `typename u` is used here, instead of a direct value. This allow us to support multidimensional
indexers. `at_helper` is here for that: call the indexer with the correct arguments.

And the terminal case is pretty straightforward. This one also checks the number of arguments:

```cpp
template <typename Indexer, std::size_t X>
constexpr bool correct_index_()
{
	static_assert(!(X < Indexer::size), "Not enough initializers provided");
	static_assert(!(X > Indexer::size), "Too many initializers provided");
	return X == Indexer::size;
}
```

Now we just have to add the constructor in `indexed_array`:

```cpp
	// safe_arg constructor
	template <
	    typename... Args,
	    std::enable_if_t<has_member_index<boost::mp11::mp_first<boost::mp11::mp_list<Args...> > >::value, bool> = true>
	constexpr indexed_array(Args&&... args) : data_{static_cast<Value>(args)...}
	{
		static_assert(detail::correct_index<Indexer, typename Args::checked_arg_index...>(), "Argument mismatch");
	}
```

Back to the [Index](index.md), or continue to [Comparison with other libraries](comparison.md)

