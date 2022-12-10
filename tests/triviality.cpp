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

BOOST_AUTO_TEST_CASE(triviality)
{
	int a = 2;
	using T = indexed_array<int, Color>;
	BOOST_TEST(std::is_trivial_v<T>);
	BOOST_TEST(std::is_trivially_default_constructible_v<T>);
	BOOST_TEST(std::is_trivially_copyable_v<T>);
	BOOST_TEST(std::is_trivially_constructible_v<T>);
	BOOST_TEST(std::is_trivially_destructible_v<T>);
	BOOST_TEST(std::is_trivially_copy_constructible_v<T>);
	BOOST_TEST(std::is_trivially_move_constructible_v<T>);
	BOOST_TEST(std::is_trivially_copy_assignable_v<T>);
	BOOST_TEST(std::is_trivially_move_assignable_v<T>);
}

BOOST_AUTO_TEST_CASE(triviality2)
{
	int a = 2;
	using T = indexed_array<int, Color, Color>;
	BOOST_TEST(std::is_trivial_v<T>);
	BOOST_TEST(std::is_trivially_default_constructible_v<T>);
	BOOST_TEST(std::is_trivially_copyable_v<T>);
	BOOST_TEST(std::is_trivially_constructible_v<T>);
	BOOST_TEST(std::is_trivially_destructible_v<T>);
	BOOST_TEST(std::is_trivially_copy_constructible_v<T>);
	BOOST_TEST(std::is_trivially_move_constructible_v<T>);
	BOOST_TEST(std::is_trivially_copy_assignable_v<T>);
	BOOST_TEST(std::is_trivially_move_assignable_v<T>);
}

BOOST_AUTO_TEST_CASE(nothrow)
{
	using T = indexed_array<int, Color>;
	BOOST_TEST((std::is_nothrow_constructible_v<T, int, int, int, int, int>));
	using T2 = indexed_array<std::string, Color>;
	BOOST_TEST(!(std::is_nothrow_constructible_v<T2, std::string, std::string, std::string, std::string, std::string>));
	static_assert(std::is_nothrow_destructible_v<T>);
	static_assert(std::is_nothrow_constructible_v<T2>);
}

BOOST_AUTO_TEST_CASE(nothrow2)
{
	using T = indexed_array<int, Color>;
	BOOST_TEST((std::is_nothrow_constructible_v<T,
	                                            decltype(safe_arg<Color::Red>(int{})),
	                                            decltype(safe_arg<Color::Green>(int{})),
	                                            decltype(safe_arg<Color::Blue>(int{})),
	                                            decltype(safe_arg<Color::Black>(int{})),
	                                            decltype(safe_arg<Color::White>(int{}))>));
	using T2 = indexed_array<std::string, Color>;
	BOOST_TEST(!(std::is_nothrow_constructible_v<T2,
	                                             decltype(safe_arg<Color::Red>(std::string{})),
	                                             decltype(safe_arg<Color::Green>(std::string{})),
	                                             decltype(safe_arg<Color::Blue>(std::string{})),
	                                             decltype(safe_arg<Color::Black>(std::string{})),
	                                             decltype(safe_arg<Color::White>(std::string{}))>));
	static_assert(!std::is_nothrow_copy_constructible_v<std::string>);
}
