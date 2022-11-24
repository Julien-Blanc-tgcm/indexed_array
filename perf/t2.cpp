#include "jbc/indexed_array.hpp"

using namespace jbc::indexed_array;

extern int f(int);

enum class color
{
	red = 1,
	green = 2,
	blue = 4,
	black = 8
};

using stdarray = std::array<int, 4>;
const stdarray vals_int{0xFF0000, 0x00FF00, 0x0000FF, 0};

using idxarray = indexed_array<int, std::integer_sequence<color, color::red, color::green, color::blue, color::black>>;
const idxarray vals_idx{0xFF0000, 0x00FF00, 0x0000FF, 0};

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
