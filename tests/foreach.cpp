#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"

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

BOOST_AUTO_TEST_CASE(foreach5)
{
	Test4 t{1, 2, 3, 4, 5, 10, 11, 12, 13, 14, 15};
	Test4 const t2(t);
	for_each(t2, [&t2](int key, int val) { // reference won't compile here
		BOOST_TEST(t2[key] == val);
	});
}

using Test5 = indexed_array<int, interval<1, 5>, interval<3, 10> >;

BOOST_AUTO_TEST_CASE(foreach_span1)
{
	Test5 t;
	int i = 0;
	for (auto& v : t)
		v = i++;
	auto s = t.slice(2);
	for_each(s, [&t](int key, int& val) {
		BOOST_TEST((t[{2, key}] == val));
		val = 2 * val;
	});
	BOOST_TEST(s[3] == 16);
}

BOOST_AUTO_TEST_CASE(foreach_span2)
{
	Test5 t;
	int i = 0;
	for (auto& v : t)
		v = i++;
	Test5 const t2(t);
	auto s = t2.slice(2);
	for_each(s, [&t](int key, int const& val) {
		BOOST_TEST((t[{2, key}] == val));
	});
}

/*
Not implemented yet. Uncomment this when foreach is implemented for
multidimensional arrays
using Test5 = indexed_array<int, interval<1, 5>, interval<10, 15> >;

BOOST_AUTO_TEST_CASE(foreach5)
{
	Test5 t{};
	int i = 0;
	for (auto& v : t)
		v = i++;

	for_each(t, [](int key1, auto& val) {
		for_each(val, [key1](int key, int& v) {
			v = key1 * 100 + key;
		});
	});
	BOOST_TEST(t.at(2, 11) == 211);
}
*/
