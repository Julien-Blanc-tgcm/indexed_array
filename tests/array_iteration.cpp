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

BOOST_AUTO_TEST_CASE(iteration)
{
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;

	Base b1;
	int i = 0;
	for (auto& b : b1)
	{
		b = std::to_string(i++);
	}
	Base const b2{b1};
	i = 0;
	for (auto& b : b2)
	{
		BOOST_TEST(b == std::to_string(i++));
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

BOOST_AUTO_TEST_CASE(reverse_iteration)
{
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;

	Base b1;
	int i = 0;
	for (auto& b : b1)
	{
		b = std::to_string(i++);
	}
	for (auto e = b1.rbegin(); e != b1.rend(); ++e)
	{
		BOOST_TEST(*e == std::to_string(--i));
	}
	i = b1.size();
	for (auto e = b1.crbegin(); e != b1.crend(); ++e)
	{
		BOOST_TEST(*e == std::to_string(--i));
	}
}

BOOST_AUTO_TEST_CASE(front_back)
{
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;

	Base b1;
	int i = 0;
	for (auto& b : b1)
	{
		b = std::to_string(i++);
	}
	BOOST_TEST(b1.front() == "0");
	BOOST_TEST(b1.back() == std::to_string(i - 1));
	Base const b2(b1);
	BOOST_TEST(b2.front() == "0");
	BOOST_TEST(b2.back() == std::to_string(i - 1));
}
