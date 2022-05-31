<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Custom Indexer

If needed, the library user can provide its own indexing mechanism. An indexer
must provide the following members:
* an `index` typedef, which is the type used in the public interface
* a static `std::size_t size` member, which is the size of the container
* a static `template<bool b> size_t at(index)`, which does the actual
translation between interface index and internal container index. The boolean is
here to distinguish between throwing (`b` is `true`)/ non-throwing (`b` is `false`)
versions. The indexer shall raise an `out_of_range` exception when `b` is `true` and
it is given an invalid index. This method may be `noexcept(!b)`.

This is an example of a custom index:

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
};
```

## Multidimensional indexer

The multidimensional indexer must provide two additional members:

* `root_indexer`, which is the indexer for the top-level dimensions
* `slice_indexer`, which is the indexer for the remaining dimensions

```
// 3-dimensional indexer with top-level reverse fibonnaci indexing
struct fib_multidim_indexer
{
	using index = mp_list<int, int, int>;
	using root_indexer = reverse_fibonnaci_index;
	using slice_indexer = make_default_indexer<interval<3, 12>, interval<-3, 2>>;
	inline static constexpr size_t const size = root_indexer::size * slice_indexer::size;
	template<bool b>
	static constexpr auto at(int v1, int v2, int v3) noexcept(!b)
	{
		return root_indexer::at<b>(v1) * slice_indexer::size + slice_indexer::at<b>(v2, v3);
	};
};
```
