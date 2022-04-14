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

BOOST_AUTO_TEST_CASE(triviality)
{
	int a = 2;
	using T = indexed_array<int, Color>;
	indexed_array<int, Color > arr{
	    safe_arg<Color::Red>(10),  //
	    safe_arg<Color::Green>(-4),  //
	    safe_arg<Color::Blue>(1),  //
	    safe_arg<Color::Black>(a), //
	    safe_arg<Color::White>(3)  //
	};
	BOOST_TEST(std::is_trivial_v<T>);
	BOOST_TEST(std::is_trivially_copyable_v<T>);
	BOOST_TEST(std::is_trivially_constructible_v<T>);
	BOOST_TEST(std::is_trivially_destructible_v<T>);
	BOOST_TEST(std::is_trivially_copy_constructible_v<T>);
	BOOST_TEST(std::is_trivially_move_constructible_v<T>);
	BOOST_TEST(std::is_trivially_copy_assignable_v<T>);
	BOOST_TEST(std::is_trivially_move_assignable_v<T>);
}
