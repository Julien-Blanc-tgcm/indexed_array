#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>
#include <boost/describe.hpp>

#include <chrono>

#include "jbc/indexed_array.hpp"

struct hour
{
	int value;
	constexpr hour(int v) : value{v}
	{
	}
	constexpr operator int() const
	{
		return value;
	}
	constexpr hour() = default;
};

template <typename duration>
struct duration_wrapper
{
	typename duration::rep val;
	constexpr duration_wrapper() = default;
	explicit constexpr duration_wrapper(typename duration::rep v) : val(v) {}
	constexpr duration_wrapper(duration d) : val(d.count()) {}
	constexpr operator typename duration::rep() const { return val; }
};

static_assert(std::is_convertible_v<hour, long long>, "Must be convertible");

using namespace jbc::indexed_array;

static_assert(detail::integral_value<hour, hour(1)>::value == 1, "Must be equal");
static_assert(detail::integral_value<duration_wrapper<std::chrono::minutes>,
                                     duration_wrapper<std::chrono::minutes>(std::chrono::minutes(3))>::value == 3,
              "Must be equal");

BOOST_AUTO_TEST_CASE(chrono_indexer)
{
	int a = 2;
	indexed_array<int, index_range<hour{1}, hour{7}>> arr{1, 2, 3, 4, 5, 6, 7};
	BOOST_TEST(arr.size() == 7);
	BOOST_TEST(arr[hour(4)] == 4);
	indexed_array<int,
	              index_range<duration_wrapper<std::chrono::minutes>(-1), duration_wrapper<std::chrono::minutes>(7)> >
	    arr2{1, 2, 3, 4, 5, 6, 7, 8, 9};
	BOOST_TEST(arr2[std::chrono::minutes(4)] == 6);
}



