#include "jbc/indexed_array.hpp"

using namespace jbc::indexed_array;

extern int f(int);

static indexed_array<int, interval<3, 7>> vals_idx{0, 1, 2, 3, 4};

int test_idx(int idx)
{
	return f(vals_idx[idx]);
}

static std::array<int, 5> vals_arr{0, 1, 2, 3, 4};

int test_arr(int idx)
{
	return f(vals_arr[idx - 3]);
}
