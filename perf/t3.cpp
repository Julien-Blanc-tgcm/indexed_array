#include "indexed_array.hpp"

using namespace jbc::indexed_array;

extern int f(int);

enum class color
{
	red = 1,
	green = 2,
	blue = 4,
	black = 8
};

struct custom_indexer
{
	using index = color;
	static inline constexpr size_t size = 4;
	template<bool c = true>
	static constexpr std::size_t at(index v) noexcept(!c)
	{
		int r = static_cast<int>(v);
		if constexpr(c)
		{
			if (r != 1 && r != 2 && r != 4 && r != 8)
				throw std::out_of_range("Invalid index");
		}
		return (r & 0x1) + (r & 0x2) + ((r & 0x4) >> 2) + ((r & 0x4) >> 1) + ((r & 0x8) >> 1) - 1;
	}
};

using stdarray = std::array<int, 4>;
constexpr const stdarray vals_int{0xFF0000, 0x00FF00, 0x0000FF, 0};

using idxarray = indexed_array<int, custom_indexer>;
constexpr const idxarray vals_idx{0xFF0000, 0x00FF00, 0x0000FF, 0};

static_assert(vals_idx[color::red] == 0xFF0000);
static_assert(vals_idx[color::green] == 0x00FF00);
static_assert(vals_idx[color::blue] == 0x0000FF);
static_assert(vals_idx[color::black] == 0x0);

int test_idx(idxarray const& arr)
{
	return f(arr[color::blue]) + f(vals_idx[color::red]);
}

int test_arr(stdarray const& arr)
{
	return f(arr[2]) + f(vals_int[0]);
}

int test_idx2(color c)
{
	return vals_idx[c];
}

int test_arr2(int c)
{
	return vals_int[c];
}

int test_idx3(color c)
{
	switch(c)
	{
		case color::red:
			return vals_int[0];
		case color::green:
			return vals_int[1];
		case color::blue:
			return vals_int[2];
		case color::black:
			return vals_int[3];
	}
	return vals_int[static_cast<int>(c)];
}


using T1 =    indexed_array<int, interval<3, 7>>;
using T2 =         indexed_array<int, std::integer_sequence<int, 3, 4, 5, 6, 7>>;

int g(T1& arr, int idx)
{
	return f(arr[idx]);
}

int g(T2& arr, int idx)
{
	return f(arr[idx]);
}
