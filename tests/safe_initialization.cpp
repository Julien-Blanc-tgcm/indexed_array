#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"

using namespace jbc::indexed_array;

enum class Color
{
	Red = -2,
	Green = -1,
	Blue = 0,
	Black = 1,
	White
};

BOOST_AUTO_TEST_CASE(safe_arg1)
{
	int a = 2;
	indexed_array<int, index_range<Color::Blue, Color::White> > arr{
	    safe_arg<Color::Blue>(1),  //
	    safe_arg<Color::Black>(a), //
	    safe_arg<Color::White>(3)  //
	};
	BOOST_TEST(arr.size() == 3);
	BOOST_TEST(arr[Color::Blue] == 1);
	BOOST_TEST(arr[Color::Black] == 2);
	BOOST_TEST(arr[Color::White] == 3);
}

BOOST_AUTO_TEST_CASE(safe_arg_unique_ptr)
{
	auto p = std::make_unique<int>(123);
	indexed_array<std::unique_ptr<int>, index_range<Color::Red, Color::Blue> > arr{
	    safe_arg<Color::Red>(std::make_unique<int>(12)), //
	    safe_arg<Color::Green>(std::move(p)),            //
	    safe_arg<Color::Blue>(std::make_unique<int>(-1)) //
	};
	BOOST_TEST(arr.size() == 3);
	BOOST_TEST(*arr[Color::Red] == 12);
	BOOST_TEST(*arr[Color::Green] == 123);
	BOOST_TEST(*arr[Color::Blue] == -1);
}

BOOST_AUTO_TEST_CASE(safe_arg_holes)
{
	auto p = std::make_unique<int>(123);
	indexed_array<std::unique_ptr<int>, union_of<single_value<Color::Red>, index_range<Color::Blue, Color::Black> > > arr{
	    safe_arg<Color::Red>(std::make_unique<int>(12)),  //
	    safe_arg<Color::Blue>(std::move(p)),              //
	    safe_arg<Color::Black>(std::make_unique<int>(-1)) //
	};
	BOOST_TEST(arr.size() == 3);
	BOOST_TEST(*arr[Color::Red] == 12);
	BOOST_TEST(*arr[Color::Blue] == 123);
	BOOST_TEST(*arr[Color::Black] == -1);
}

BOOST_AUTO_TEST_CASE(safe_arg_single_value)
{
	indexed_array<int, union_of<single_value<Color::Red> > > arr{safe_arg<Color::Red>(1)};
	BOOST_TEST(arr[Color::Red] == 1);
}
