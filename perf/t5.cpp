#include "indexed_array.hpp"

#include <cstdint>

using namespace jbc::indexed_array;

extern int f(int);

enum class color : std::uint32_t
{
	red,
	green,
	blue,
	black
};

using stdarray = std::array<int, 4>;
constexpr const stdarray vals_int{0xFF0000, 0x00FF00, 0x0000FF, 0};

using idxarray = indexed_array<int, interval<color::red, color::black>>;
constexpr const idxarray vals_idx{
	safe_arg<color::red>(0xFF0000),
	safe_arg<color::green>(0x00FF00), 
	safe_arg<color::blue>(0x0000FF),
	safe_arg<color::black>(0)};

static_assert(vals_idx[color::red] == 0xFF0000);
static_assert(vals_idx[color::green] == 0x00FF00);
static_assert(vals_idx[color::blue] == 0x0000FF);
static_assert(vals_idx[color::black] == 0x0);

int test_idx(idxarray const& arr)
{
	return f(arr.at(color::blue)) + f(vals_idx.at(color::red));
}

int test_arr(stdarray const& arr)
{
	return f(arr.at(2)) + f(vals_int.at(0));
}

int test_fidx(color idx)
{
	return f(vals_idx[idx]);
}

int test_farr(int idx)
{
	return f(vals_int[idx]);
}

