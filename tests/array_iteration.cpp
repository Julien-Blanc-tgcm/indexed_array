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
	i = 0;
	for (auto it = b1.cbegin(); it != b1.cend(); ++it)
	{
		BOOST_TEST(*it == std::to_string(i++));
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

BOOST_AUTO_TEST_CASE(reverse_const_iteration)
{
	using Base = indexed_array<std::string, Color, interval<3, 8>, interval<-2, 4> >;
	Base b1;
	int i = 0;
	for (auto& b : b1)
	{
		b = std::to_string(i++);
	}
	auto const& b2 = b1;
	for (auto it = b2.rbegin(); it != b2.rend(); ++it)
	{
		BOOST_TEST(*it == std::to_string(--i));
	}
}

using Base = indexed_array<int, Color>;

constexpr decltype(auto) third_item(Base const& b)
{
	return b.begin() + 2;
}


BOOST_AUTO_TEST_CASE(constexpr_iteration)
{
	constexpr Base b{0, 1, 2, 3, 4};
	static_assert(*third_item(b) == 2);
	BOOST_TEST(*third_item(b) == 2);
	static_assert(std::distance(b.begin(), b.end()) == 5);
	BOOST_TEST(std::distance(b.begin(), b.end()) == 5);
}

constexpr int f(Base b)
{
	b[Color::Blue] = 12;
	return b[Color::Blue];
}

BOOST_AUTO_TEST_CASE(constexpr_alteration)
{
	static_assert(f(Base{0, 1, 2, 3, 4}) == 12);
	BOOST_TEST(f(Base{0, 1, 2, 3, 4}) == 12);
}
