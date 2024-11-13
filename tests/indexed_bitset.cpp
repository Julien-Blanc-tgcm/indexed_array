#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include <chrono>

#include "jbc/indexed_array.hpp"

enum class Color
{
	Red = -1,
	Green,
	Blue
};
BOOST_DESCRIBE_ENUM(Color, Red, Green, Blue);

using namespace jbc::indexed_array;

BOOST_AUTO_TEST_CASE(indexed_bitset1)
{
	indexed_bitset<index_range<2, 10> > b(0b001001001u);
	BOOST_TEST(b.size() == 9);
	BOOST_TEST(b.test(2));
	BOOST_TEST(!b.test(3));
	b.set(4);
	b.reset(5);
	b.flip(6);
	b.flip(7);
	b.set(10);
//	BOOST_TEST(b.to_ulong(), 0b100010001);
}

BOOST_AUTO_TEST_CASE(multi_indexed_bitset)
{
	indexed_bitset<index_range<1, 3>, Color> b(0b001001001u);
	BOOST_TEST(b.size() == 9);
	BOOST_TEST(b.test(2, Color::Red));
	BOOST_TEST(!b.test(3, Color::Blue));
	b.set(2, Color::Green);
	b.reset(2, Color::Blue);
	b.flip(2, Color::Red);
	b.flip(3, Color::Red);
	b.set(3, Color::Blue);
//	BOOST_TEST(b.to_ulong(), 0b100010001);
}

BOOST_AUTO_TEST_CASE(assignment)
{
	indexed_bitset<index_range<2, 10> > b(0u);
	BOOST_TEST(b.size() == 9);
	b = 0x11u;
	BOOST_TEST(b.test(2));
	BOOST_TEST(!b.test(3));
	BOOST_TEST(b.test(6));
	BOOST_TEST(b.to<unsigned long long>() == 0x11u);
}

BOOST_AUTO_TEST_CASE(operator_brackets)
{
	indexed_bitset<index_range<2, 10> > b(0x11u);
	BOOST_TEST(b.size() == 9);
	BOOST_TEST(static_cast<bool>(b[2]));
	BOOST_TEST(!b[3]);
	b[3] = 1;
	BOOST_TEST(static_cast<bool>(b[3]));
}

BOOST_AUTO_TEST_CASE(count)
{
	indexed_bitset<index_range<2, 10> > b(0x11u);
	BOOST_TEST(b.count() == 2);
	b[7] = true;
	BOOST_TEST(b.count() == 3);
	b[2] = true;
	BOOST_TEST(b.count() == 3);
	b[2] = false;
	BOOST_TEST(b.count() == 2);
}

BOOST_AUTO_TEST_CASE(all)
{
	indexed_bitset<index_range<2, 10> > b(0x11u);
	BOOST_TEST(!b.all());
	for (auto i = 2u; i <= 10u; ++i)
		b[i] = true;
	BOOST_TEST(b.all());
	b[5] = false;
	BOOST_TEST(!b.all());
}

BOOST_AUTO_TEST_CASE(none)
{
	indexed_bitset<index_range<2, 10> > b(0x11u);
	BOOST_TEST(!b.none());
	for (auto i = 2u; i <= 10u; ++i)
		b[i] = false;
	BOOST_TEST(b.none());
	b[5] = true;
	BOOST_TEST(!b.none());
}

BOOST_AUTO_TEST_CASE(any)
{
	indexed_bitset<index_range<2, 10> > b(0x11u);
	BOOST_TEST(b.any());
	for (auto i = 2u; i <= 10u; ++i)
		b[i] = false;
	BOOST_TEST(!b.any());
	b[5] = true;
	BOOST_TEST(b.any());
}

BOOST_AUTO_TEST_CASE(safe_arg_init)
{
	indexed_bitset<Color> b{safe_arg<Color::Red>(true), safe_arg<Color::Green>(false), safe_arg<Color::Blue>(true)};
//	BOOST_TEST(b.to_ullong() == 0b101);
	BOOST_TEST(b.test(Color::Red));
	BOOST_TEST(!b.test(Color::Green));
	BOOST_TEST(b.test(Color::Blue));
}

BOOST_AUTO_TEST_CASE(multidimensional)
{
	indexed_bitset<Color, interval<2,3>> b{
        safe_arg<Color::Red, 2>(true), 
        safe_arg<Color::Red, 3>(true), 
        safe_arg<Color::Green, 2>(false),
        safe_arg<Color::Green, 3>(true),
        safe_arg<Color::Blue, 2>(true),
        safe_arg<Color::Blue, 3>(false)};

	BOOST_TEST(b.test(Color::Red, 2));
	BOOST_TEST(!b.test(Color::Green, 2));
	BOOST_TEST(!b.test(Color::Blue, 3));

//	BOOST_TEST(b.to_ullong(), 0b011011);

	BOOST_TEST(static_cast<bool>(b[std::tuple{Color::Red, 2}]));
	BOOST_TEST(b(Color::Red, 3));
	BOOST_TEST(b.test(Color::Green, 3));

	auto const& b2 = b;
	BOOST_TEST(static_cast<bool>(b2[std::tuple{Color::Red, 2}]));
	BOOST_TEST(static_cast<bool>(b2(Color::Red, 3)));
	BOOST_TEST(static_cast<bool>(b2.test(Color::Green, 3)));

}
