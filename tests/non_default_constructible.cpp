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

class Test
{
	int value_;

  public:
	explicit Test(int v) : value_{v}
	{
	}
	Test() = delete;
	operator int()
	{
		return value_;
	}
};

BOOST_AUTO_TEST_CASE(non_default_constructible)
{
	indexed_array<Test, Color> arr{Test{2}, Test{3}, Test{4}, Test{5}, Test{-1}};
	BOOST_TEST(static_cast<int>(arr[Color::Black]) == 5);
}

BOOST_AUTO_TEST_CASE(non_default_constructible_safe_arg)
{
	indexed_array<Test, Color> arr{safe_arg<Color::Red>(Test{2}),
	                               safe_arg<Color::Green>(Test{3}),
	                               safe_arg<Color::Blue>(Test{4}),
	                               safe_arg<Color::Black>(Test{5}),
	                               safe_arg<Color::White>(Test{-1})};
	BOOST_TEST(static_cast<int>(arr[Color::White]) == -1);
}
