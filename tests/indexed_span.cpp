#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"

#include <type_traits>

using namespace jbc::indexed_array;
namespace mp11 = boost::mp11;

enum class Color
{
	Red = -4,
	Green = -1,
	Blue = 0,
	Black = 6,
	White = 8
};

BOOST_DESCRIBE_ENUM(Color, Red, Green, Blue, Black, White);

BOOST_AUTO_TEST_CASE(indexed_span_test)
{
	std::array<int, 10> data{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	indexed_span<int, Color, index_range<0, 1> > sp(data.data());
	BOOST_TEST(sp.size() == 10);
	BOOST_TEST(sp.at(Color::Blue, 1) == 5);
	BOOST_TEST(sp.at(Color::White, 1) == 9);
	auto s = sp.slice(Color::Green);
	BOOST_TEST(s.size() == 2);
	BOOST_TEST(s.at(0) == 2);
	BOOST_TEST(s[1] == 3);
	BOOST_TEST(!decltype(sp)::is_o1);
	BOOST_TEST(sp.in_range(Color::Green, 0));
	BOOST_TEST(!sp.in_range(Color::Green, -1));
}

BOOST_AUTO_TEST_CASE(indexed_span_foreach)
{
	std::array<std::string, 10> data{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
	indexed_span<std::string, Color, index_range<0, 1> > sp(data.data());
	for (auto& c : sp)
	{
		c = c + c;
	}
	BOOST_TEST(sp.size() == 10);
	BOOST_TEST(sp.at(Color::Blue, 1) == "55");
	BOOST_TEST(sp.at(Color::White, 1) == "99");
	auto s = sp.slice(Color::Green);
	BOOST_TEST(s.size() == 2);
	BOOST_TEST(s.at(0) == "22");
	BOOST_TEST(s[1] == "33");
}

BOOST_AUTO_TEST_CASE(indexed_span_slice)
{
	indexed_array<int, index_range<2, 4>, index_range<3, 5> > arr{0, 1, 2, 3, 4, 5, 6, 7, 8};
	BOOST_TEST((std::is_same_v<decltype(arr.slice_at(2)), indexed_span<int, index_range<3, 5> > >));
	BOOST_TEST((std::is_same_v<decltype(arr.slice(2)), indexed_span<int, index_range<3, 5> > >));
	BOOST_TEST((arr[std::make_tuple(2, 4)] == 1));
	arr.slice(2)[4] = 10;
	BOOST_TEST((arr[std::make_tuple(2, 4)] == 10));
}

BOOST_AUTO_TEST_CASE(indexed_span_slice_at)
{ // test that slice_at correctly raise exceptions
	indexed_array<int, index_range<2, 4>, index_range<3, 5> > arr;
	BOOST_TEST((std::is_same_v<decltype(arr.slice_at(2)), indexed_span<int, index_range<3, 5> > >));
	BOOST_TEST((std::is_same_v<decltype(arr.slice(2)), indexed_span<int, index_range<3, 5> > >));
	BOOST_CHECK_THROW(arr.slice_at(0), std::out_of_range);
	auto v = arr.slice_at(3); // does not throw
	BOOST_CHECK_THROW(v.at(0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(indexed_span_slice_at2)
{ // test that slice_at correctly raise exceptions
	indexed_array<int, index_range<2, 4>, index_range<3, 5> > const arr{};
	BOOST_CHECK_THROW(arr.slice_at(1), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(indexed_span_slice_const)
{
	indexed_array<int, index_range<2, 4>, index_range<3, 5> > const arr{0, 1, 2, 3, 4, 5, 6, 7, 8};
	BOOST_TEST((std::is_same_v<decltype(arr.slice_at(2)), indexed_span<int const, index_range<3, 5> > >));
	BOOST_TEST((std::is_same_v<decltype(arr.slice(2)), indexed_span<int const, index_range<3, 5> > >));
	BOOST_TEST(arr.slice(2)[5] == 2);
}

BOOST_AUTO_TEST_CASE(indexed_span_reverse_iterator)
{ // test that reverse iteration is ok
	indexed_array<int, index_range<2, 4>, index_range<3, 5> > const arr{0, 1, 2, 3, 4, 5, 6, 7, 8};
	auto v = arr.slice(3);
	int i = 0;
	for (auto it = v.rbegin(); it != v.rend(); ++it, ++i)
	{
		BOOST_TEST(*it == 5 - i);
	}
	// also check const version
	auto const arr2 = arr;
	auto v2 = arr2.slice(4);
	i = 0;
	for (auto it = v2.rbegin(); it != v2.rend(); ++it, ++i)
	{
		BOOST_TEST(*it == 8 - i);
	}
}

BOOST_AUTO_TEST_CASE(indexed_span_data)
{
	char const* d = "toto";
	indexed_span<char const, index_range<0, 3> > sp(d);
	BOOST_TEST(sp.data() == d);
	
}
