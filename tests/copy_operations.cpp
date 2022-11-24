#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>
#include <boost/describe.hpp>

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

BOOST_AUTO_TEST_CASE(fill)
{
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;
	Base b1;
	b1.fill("b");
	for (auto& b : b1)
	{
		BOOST_TEST(b == "b");
	}
}

BOOST_AUTO_TEST_CASE(copy)
{
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;
	Base b1;
	b1.fill("b");
	Base b2(b1);
	for (auto& b : b2)
	{
		BOOST_TEST(b == "b");
	}
	BOOST_TEST(b1 == b2);
}

BOOST_AUTO_TEST_CASE(move)
{
	using Base = indexed_array<std::unique_ptr<int>, Color, interval<3, 8>, interval<-2, 4> >;
	Base b1;
	int i = 0;
	for (auto& b : b1)
	{
		b = std::make_unique<int>(i++);
	}
	Base b2(std::move(b1));
	for (auto& b : b1)
	{
		bool res = b == nullptr;
		BOOST_TEST(res);
	}
	i = 0;
	for (auto& b : b2)
	{
		bool res = b != nullptr;
		BOOST_TEST(res);
		std::cout << *b << std::endl;
		BOOST_TEST(*b == i);
		i++;
	}
}
