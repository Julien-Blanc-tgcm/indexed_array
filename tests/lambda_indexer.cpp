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

constexpr auto l = [](std::chrono::seconds n) { return n.count() + 3; };

using seconds_indexer1 = lambda_indexer<l, 10>;

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
	constexpr auto day_to_unsigned = [](std::chrono::day d) { return static_cast<unsigned>(d) - 1u; };
	using day_of_month_indexer = lambda_indexer<day_to_unsigned, 31>;
	using Base = indexed_array<int, day_of_month_indexer, Color>;
	Base b;
	BOOST_TEST(b.size() == 31 * 5);
}

BOOST_AUTO_TEST_CASE(weekday_indexer)
{
	using namespace std::chrono;
	constexpr auto weekday_to_unsigned = [](weekday w) -> std::size_t { return w.iso_encoding() - 1; };
	using wday_indexer =
	    lambda_indexer<weekday_to_unsigned, 7u>;
	using Base = indexed_array<int, wday_indexer>;
	Base b;
	BOOST_TEST(b.size() == 7);

	static_assert(detail::is_indexer_invocable_with<wday_indexer, weekday>::value);
	static_assert(detail::is_indexer_invocable_with_detail<wday_indexer, boost::mp11::mp_list<weekday> >::value);
	static_assert(!detail::is_indexer_invocable_with_detail<wday_indexer, boost::mp11::mp_list<month> >::value);
#if __cpp_concepts >= 202002L
	static_assert(!jbc::indexed_array::concepts::indexer_invocable_with<wday_indexer, month>);
#endif
}
