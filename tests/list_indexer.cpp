#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"

#include <chrono>
#include <type_traits>

using namespace jbc::indexed_array;
using namespace std::chrono_literals;

enum class Color
{
	Red = -4,
	Green = -1,
	Blue = 0,
	Black = 6,
	White = 8
};

BOOST_DESCRIBE_ENUM(Color, Red, Green, Blue, Black, White);

using List = value_list<Color::Red, Color::Green, 4, 5>;

using ListIndexer = list_indexer<List>;

static_assert(detail::type_in_list<Color, List>::value);

BOOST_AUTO_TEST_CASE(list_indexer1)
{
	using Base = indexed_array<int, list_indexer<List>>;

	Base b{1, 2, 3, 4};

	BOOST_TEST(b.size() == 4);
	BOOST_TEST(b[Color::Green] == 2);
	BOOST_TEST(b[4] == 3);
}
