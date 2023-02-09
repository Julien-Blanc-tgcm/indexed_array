#define BOOST_TEST_MODULE Indexed Array

#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"

using namespace jbc::indexed_array;

enum class Color
{
	Red = -4,
	Green = -1,
	Blue = 0,
	Black = 6,
	White = 8
};

BOOST_DESCRIBE_ENUM(Color, Red, Green, Blue, Black, White);

BOOST_AUTO_TEST_CASE(enum_arrayinit)
{
	int a = 2;
	std::array<int, 5> ref{10, -4, 1, a, 3};
	indexed_array<int, Color> arr{ref};
	BOOST_TEST(arr.size() == 5);
	BOOST_TEST(arr[Color::Red] == 10);
	BOOST_TEST(arr[Color::Green] == -4);
	BOOST_TEST(arr[Color::Blue] == 1);
	BOOST_TEST(arr.at(Color::Black) == 2); // also verify bound check access
	BOOST_TEST(arr[Color::White] == 3);
	int i = 0;
	for (auto& a : arr)
	{
		BOOST_TEST(a == ref[i]);
		i += 1;
	}
}

BOOST_AUTO_TEST_CASE(integerlist_indexing_arrayinit)
{
	std::array<std::string, 5> ref{
	    "-4", //
	    "7",
	    "-8",
	    "6",
	    "123" //
	};
	indexed_array<std::string, std::integer_sequence<int, -4, 7, -8, 6, 123> > arr{ref};
	BOOST_TEST(arr.size() == 5);
	BOOST_TEST(arr[-4] == "-4");
	BOOST_TEST(arr[7] == "7");
	BOOST_TEST(arr.at(-8) == "-8");
	BOOST_TEST(arr[6] == "6");
	BOOST_TEST(arr[123] == "123");
	int i = 0;
	for (auto const& a : arr)
	{
		BOOST_TEST(a == ref[i]);
		i += 1;
	}
}

BOOST_AUTO_TEST_CASE(integerlist_contiguous_indexing)
{
	std::array<std::string, 5> ref{"0", "10", "20", "30", "40"};
	indexed_array<std::string, std::integer_sequence<int, 0, 1, 2, 3, 4> > arr{ref};
	BOOST_TEST(arr.size() == 5);
	BOOST_TEST(arr[0] == "0");
	BOOST_TEST(arr.at(1) == "10");
	BOOST_TEST(arr[2] == "20");
	BOOST_TEST(arr[3] == "30");
	BOOST_TEST(arr[4] == "40");
	int i = 0;
	for (auto const& a : arr)
	{
		BOOST_TEST(a == ref[i]);
		i += 1;
	}
}

#ifndef _MSC_VER // msvc complains about std::array of more than 2^31 in size, disable the test
BOOST_AUTO_TEST_CASE(integer_overflow)
{
	// must use long long here. Using 32bits integer will result in a compile error
	using T = indexed_array<int, index_range<-2'000'000'000ll, 1'000'000'000ll> >;
	auto v = std::tuple_size<T>::value;
	BOOST_TEST(v == 3'000'000'001u);
}
#endif
