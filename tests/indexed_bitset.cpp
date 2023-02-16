#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include <chrono>

#include "jbc/indexed_array.hpp"

enum class Color
{
	Red,
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
	BOOST_TEST(b.to_ulong(), 0b100010001);
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
	BOOST_TEST(b.to_ulong(), 0b100010001);
}

BOOST_AUTO_TEST_CASE(internal_bitset)
{
	indexed_bitset<index_range<2, 10> > b(0u);
	BOOST_TEST(b.size() == 9);
	static_cast<std::bitset<9>&>(b) = 0x11u;
	BOOST_TEST(b.test(2));
	BOOST_TEST(!b.test(3));
	BOOST_TEST(b.test(6));
	BOOST_TEST(static_cast<std::bitset<9> const&>(b).test(0));
	BOOST_TEST(b.to_ullong() == static_cast<std::bitset<9> const&>(b).to_ullong());
}

BOOST_AUTO_TEST_CASE(operator_brackets)
{
	indexed_bitset<index_range<2, 10> > b(0x11u);
	BOOST_TEST(b.size() == 9);
	BOOST_TEST(b[2]);
	BOOST_TEST(!b[3]);
	b[3] = 1;
	BOOST_TEST(b[3]);
}
