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

using seconds_indexer1 =
    lambda_indexer<std::chrono::seconds, [](std::chrono::seconds n) { return n.count(); }, -3l, 6l>;

BOOST_AUTO_TEST_CASE(duration_indexer)
{
	using Base = indexed_array<int, seconds_indexer1>;

	Base b{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	BOOST_TEST(b.size() == 10);
	BOOST_TEST(b[2s] == 6);
	BOOST_TEST(b.at(0s) == 4);
}

BOOST_AUTO_TEST_CASE(day_of_month_indexer)
{
	constexpr auto day_to_unsigned = [](std::chrono::day d) { return static_cast<unsigned>(d); };
	using day_of_month_indexer = lambda_indexer<std::chrono::day, day_to_unsigned, 1u, 31u>;
	using Base = indexed_array<int, day_of_month_indexer, Color>;
	Base b;
	BOOST_TEST(b.size() == 31 * 5);
}

BOOST_AUTO_TEST_CASE(weekday_indexer)
{
	using namespace std::chrono;
	constexpr auto weekday_to_unsigned = [](weekday w) { return w.iso_encoding(); };
	using wday_indexer =
	    lambda_indexer<weekday, weekday_to_unsigned, weekday_to_unsigned(Monday), weekday_to_unsigned(Sunday)>;
	using Base = indexed_array<int, wday_indexer>;
	Base b;
	BOOST_TEST(b.size() == 7);
}
