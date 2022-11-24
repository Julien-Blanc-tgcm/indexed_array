#include "jbc/indexed_array.hpp"

using namespace jbc::indexed_array;

extern int f(int);

enum class color
{
	red,
	green,
	blue,
	black
};

const std::array<int, 4> vals_int{0xFF0000, 0x00FF00, 0x0000FF, 0};

const indexed_array<int, interval<color::red, color::black>> vals_idx{0xFF0000, 0x00FF00, 0x0000FF, 0};

int test_idx(color idx)
{
	return f(vals_idx[idx]);
}

int test_arr(int idx)
{
	return f(vals_int[idx]);
}
