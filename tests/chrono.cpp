#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>
#include <boost/describe.hpp>

#include <chrono>

#include "jbc/indexed_array.hpp"


using namespace jbc::indexed_array;

constexpr auto l = [](std::chrono::weekday d) { return static_cast<std::size_t>(d.iso_encoding() - 1); };

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

struct heterogeneous_index2
{
	constexpr std::size_t operator()(std::chrono::weekday w) const
	{
		return w.iso_encoding() - 1;
	}
	constexpr std::size_t operator()(std::chrono::month m) const
	{
		return static_cast<unsigned>(m) + 6;
	}
};

/*BOOST_AUTO_TEST_CASE(chrono_heterogeneous_safe_arg)
{
	using namespace std::chrono;
	auto idx = [](auto c) { return heterogeneous_index2{}(c); };
	using A = indexed_array<int, lambda_indexer<idx, 19u>>; // 19 is 7 weekdays + 12 months
	A a{
	    safe_arg2(Monday, 0),
	    safe_arg2(Tuesday, 1),
	    safe_arg2(Wednesday, 2),
	    safe_arg2(Thursday, 3),
	    safe_arg2(Friday, 4),
	    safe_arg2(Saturday, 5),
	    safe_arg2(Sunday, 6),
	    safe_arg2(January, 7),
	    safe_arg2(February, 8),
	    safe_arg2(March, 9),
	    safe_arg2(April, 10),
	    safe_arg2(May, 11),
	    safe_arg2(June, 12),
	    safe_arg2(July, 13),
	    safe_arg2(August, 14),
	    safe_arg2(September, 15),
	    safe_arg2(October, 16),
	    safe_arg2(November, 17),
	    safe_arg2(December, 18)
	};
	BOOST_TEST(a[February] == 8);
}*/
