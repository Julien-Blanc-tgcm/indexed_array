#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>
#include <boost/describe.hpp>

#include "indexed_array.hpp"
#include "detail/indexed_span.hpp"

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

BOOST_AUTO_TEST_CASE(indexed_span)
{
	std::array<int, 10> data{0,1,2,3,4,5,6,7,8,9};
	detail::indexed_span<int, detail::default_indexer<mp11::mp_list<detail::default_indexer<Color>, detail::default_indexer<interval<0, 1>>>>> sp(data.data());
	BOOST_TEST(sp.size() == 10);
	BOOST_TEST(sp.at(Color::Blue, 1) == 5);
	BOOST_TEST(sp.at(Color::White, 1) == 9);
	auto s = sp.slice(Color::Green);
	BOOST_TEST(s.size() == 2);
	BOOST_TEST(s.at(0) == 2);
	BOOST_TEST(s[1] == 3);
	auto s2 = s.slice(1);
	BOOST_TEST(s2 == 3);
}
