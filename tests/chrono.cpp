#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>
#include <boost/describe.hpp>

#include <chrono>

#include "jbc/indexed_array.hpp"


using namespace jbc::indexed_array;

auto l = [](std::chrono::weekday d) { return static_cast<std::size_t>(d.iso_encoding() - 1); };

BOOST_AUTO_TEST_CASE(chrono_indexer)
{
	int a = 2;
	indexed_array<int, lambda_indexer<l, 7>> arr{1, 2, 3, 4, 5, 6, 7};
	BOOST_TEST(arr.size() == 7);
	BOOST_TEST(arr[std::chrono::Thursday] == 4);
}

struct heterogenous_index
{
	constexpr std::size_t operator()(std::chrono::seconds s) const
	{
		auto v = s.count();
		if (v < 0 || v >= 60)
			return std::numeric_limits<std::size_t>::max();
		return static_cast<std::size_t>(s.count());
	}
	constexpr std::size_t operator()(std::chrono::minutes m) const
	{
		auto v = m.count();
		if (v < 0 || v >= 60)
			return std::numeric_limits<std::size_t>::max();
		return static_cast<std::size_t>(m.count() + 60);
	}
};

BOOST_AUTO_TEST_CASE(chrono_heterogenous)
{
	using het_index = lambda_indexer<heterogenous_index{}, 120>;
	indexed_array<int, het_index> arr;
	int i = 0;
	for (auto& a : arr)
		a = i++;
	BOOST_TEST(arr[std::chrono::seconds{10}] == 10);
	BOOST_TEST(arr[std::chrono::minutes{10}] == 70);
}
