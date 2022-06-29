#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>
#include <wise_enum/wise_enum.h>

#include "indexed_array.hpp"

#include <type_traits>

using namespace jbc::indexed_array;

WISE_ENUM_CLASS((Color, int), (Red, -4), (Green, -1), (Blue, 0), (Black, 6), (White, 8))

BOOST_AUTO_TEST_CASE(subindexer)
{
	using Base = indexed_array<int, Color>;

	Base b{1, 2, 3, 4, 5};

	BOOST_TEST(b[Color::Red] == 1);
	BOOST_TEST(b[Color::Black] == 4);
}
