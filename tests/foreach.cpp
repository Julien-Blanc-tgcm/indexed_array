#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include "indexed_array.hpp"

using namespace jbc::indexed_array;

using Test = indexed_array<int, interval<1, 5> >;

BOOST_AUTO_TEST_CASE(foreach)
{
	Test t{3, 4, 5, 6, 7};
	for_each(t, [&t](int key, int value) { //
		BOOST_TEST(t[key] == value);
	});
}

enum class Foo : std::size_t
{
	Bar1 = 3,
	Bar2,
	Bar3,
	Bar4,
	Bar5,
	Bar6
};

using Test2 = indexed_array<int, interval<Foo::Bar1, Foo::Bar6> >;

BOOST_AUTO_TEST_CASE(foreach2)
{
	Test2 t{3, 4, 5, 6, 7, 8};
	for_each(t, [&t](Foo key, int value) { //
		BOOST_TEST(t[key] == value);
	});
}

enum class Color
{
	Red = -4,
	Green = -1,
	Blue = 0,
	Black = 6,
	White = 8
};

BOOST_DESCRIBE_ENUM(Color, Red, Green, Blue, Black, White);

using Test3 = indexed_array<std::string, Color>;

BOOST_AUTO_TEST_CASE(foreach3)
{
	Test3 t{"Red", "Green", "Blue", "Black", "White"};
	for_each(t, [&t](Color key, std::string& value) { //
		BOOST_TEST(t[key] == value);
		value = "";
	});
	for (auto& c : t)
	{
		BOOST_TEST(c.empty());
	}
}

using Test4 = indexed_array<int, union_of<interval<1, 5>, interval<10, 15> > >;

BOOST_AUTO_TEST_CASE(foreach4)
{
	Test4 t{1, 2, 3, 4, 5, 10, 11, 12, 13, 14, 15};
	for_each(t, [&t](int key, int& val) { //
		BOOST_TEST(t[key] == val);
		t[key] = 2 * val;
	});
	BOOST_TEST(t[5] == 10);
	BOOST_TEST(t[10] == 20);
}
