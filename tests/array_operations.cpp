#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"

#include <type_traits>

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

BOOST_AUTO_TEST_CASE(equality)
{
	using Base = indexed_array<std::string, Color, index_range<3, 8>, index_range<-2, 4> >;

	Base b1;
	Base b2;
	int i = 0;
	for (auto& b : b1)
	{
		b = std::to_string(i++);
	}
	i = 0;
	for (auto& b : b2)
	{
		b = std::to_string(i++);
	}
	BOOST_TEST(b1 == b2);
	BOOST_TEST(!(b1 != b2));
	BOOST_TEST(b1 >= b2);
	BOOST_TEST(b2 >= b1);
	BOOST_TEST(b1 <= b2);
	BOOST_TEST(b2 <= b1);
	b1[{Color::Red, 4, 0}] = "12";
	BOOST_TEST(!(b1 == b2));
	BOOST_TEST(b1 != b2);
}

BOOST_AUTO_TEST_CASE(inferiority)
{
	using Base = indexed_array<std::string, Color, index_range<3, 8>, index_range<-2, 4> >;

	Base b1;
	Base b2;
	int i = 0;
	for (auto& b : b1)
	{
		b = std::to_string(i++);
	}
	i = 1;
	for (auto& b : b2)
	{
		b = std::to_string(i++);
	}
	BOOST_TEST(b1 < b2);
	BOOST_TEST(!(b1 > b2));
	BOOST_TEST(!(b2 < b1));
	BOOST_TEST(b2 > b1);
	BOOST_TEST(b1 <= b2);
	BOOST_TEST(!(b1 >= b2));
	BOOST_TEST(!(b2 <= b1));
	BOOST_TEST(b2 >= b1);
}

BOOST_AUTO_TEST_CASE(size)
{
	using Base = indexed_array<std::string, Color, index_range<3, 8>, index_range<-2, 4> >;
	Base b1;
	BOOST_TEST(b1.size() == 5 * 6 * 7);
	BOOST_TEST(!b1.empty());
	BOOST_TEST(b1.max_size() == b1.size());
	using Base2 = indexed_array<int, std::integer_sequence<int> >;
	Base2 b2;
	BOOST_TEST(b2.size() == 0);
	BOOST_TEST(b2.empty());
	BOOST_TEST(b2.max_size() == b2.size());
}

BOOST_AUTO_TEST_CASE(swap)
{
	using Base = indexed_array<std::string, Color, index_range<3, 8>, index_range<-2, 4> >;
	Base b1;
	int i = 0;
	for (auto& b : b1)
	{
		b = std::to_string(i++);
	}
	Base b2;
	for (auto& b : b2)
	{
		b = std::to_string(i++);
	}
	b2.swap(b1);
	i = 0;
	for (auto& b : b2)
	{
		BOOST_TEST(b == std::to_string(i++));
	}
	for (auto& b : b1)
	{
		BOOST_TEST(b == std::to_string(i++));
	}
}

BOOST_AUTO_TEST_CASE(slice)
{
	using T = indexed_array<int, Color>;
	T t;
	t[Color::Black] = 12;
	t[Color::Red] = 15;
	BOOST_TEST(t.slice(Color::Black) == 12);
	BOOST_TEST(t.slice_at(Color::Red) == 15);

	BOOST_CHECK_THROW(t.slice_at(static_cast<Color>(-10)), std::out_of_range);

	T const t2{0, 1, 2, 3, 4};
	BOOST_TEST(t2.slice(Color::Black) == 3);
	BOOST_TEST(t2.slice_at(Color::Red) == 0);
	BOOST_CHECK_THROW(t2.slice_at(static_cast<Color>(12)), std::out_of_range);
}
