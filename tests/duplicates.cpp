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
	blue = Blue,
	Black = 6,
    black = Black,
	White = 8,
    white = White
};

BOOST_DESCRIBE_ENUM(Color, Red, Green, Blue, Black, black, White, white);

BOOST_AUTO_TEST_CASE(duplicate_indexer)
{
	using Base = indexed_array<int, Color>;

	Base b{1, 2, 3, 4, 5};

	BOOST_TEST(b.size() == 5);
	BOOST_TEST(b[Color::Red] == 1);
	BOOST_TEST(b[Color::White] == 5);
	BOOST_TEST(b[Color::white] == 5);
	BOOST_TEST(b.at(Color::Black) == 4);
	BOOST_TEST(b.at(Color::black) == 4);
}

BOOST_AUTO_TEST_CASE(duplicate_indexer_at)
{
	using Base = indexed_array<int, Color>;
    BOOST_TEST(Base::indexer::at(Color::Red) == 0);
    BOOST_TEST(Base::indexer::at(Color::Green) == 1);
    BOOST_TEST(Base::indexer::at(Color::Blue) == 2);
    BOOST_TEST(Base::indexer::at(Color::blue) == 2);
    BOOST_TEST(Base::indexer::at(Color::Black) == 3);
    BOOST_TEST(Base::indexer::at(Color::black) == 3);
    BOOST_TEST(Base::indexer::at(Color::White) == 4);
    BOOST_TEST(Base::indexer::at(Color::white) == 4);
}


