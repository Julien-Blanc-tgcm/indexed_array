#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>
#include <boost/describe.hpp>

#include "jbc/indexed_array.hpp"

#include <type_traits>

using namespace jbc::indexed_array;

enum class Color
{
	Red = -4,
	Green = -1,
	Blue = 0,
	Black = 6,
	White = 8
};

BOOST_DESCRIBE_ENUM(Color, Red, Green, Blue, Black, White);

BOOST_AUTO_TEST_CASE(subindexer)
{
	using Base = indexed_array<int, Color, index_range<3, 8>, index_range<-2, 4>>;

	using Slice = Base::indexer::slice_indexer;

	BOOST_TEST((std::is_same<Slice, detail::to_single_indexer_t<index_range<3, 8>, index_range<-2, 4>>>::value));

	using Slice2 = Slice::slice_indexer;

	BOOST_TEST((std::is_same<Slice2, detail::to_single_indexer_t<index_range<-2, 4>>>::value));
}

enum class Duplicates
{
	A = 3,
	Alias = A,
	B,
	C
};

BOOST_DESCRIBE_ENUM(Duplicates, A, Alias, B, C);

BOOST_AUTO_TEST_CASE(duplicates)
{
	using Base = indexed_array<int, Duplicates>;
	Base b{};
	BOOST_TEST(b.size(), 3);
	BOOST_TEST(Base::is_o1);
}

BOOST_AUTO_TEST_CASE(error)
{
	using Base = indexed_array<int, Color>;
	Base b;
	b[Color::Red] = 0;
}
