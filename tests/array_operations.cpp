#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>
#include <boost/describe.hpp>

#include "indexed_array.hpp"

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
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;

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
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;

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
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;
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
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;
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
