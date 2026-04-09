#define BOOST_TEST_MODULE Indexed Array
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"

using namespace jbc::indexed_array;

enum class Foo
{
	bar = -2,
	ber = -1,
	bir = 0,
	bor = 1,
	bur
};

BOOST_AUTO_TEST_CASE(index_range_enum_s)
{
	indexed_array<int, index_range<Foo::bir, Foo::bur>> arr{1, 2, 3};
	BOOST_TEST(arr.size() == 3);
	BOOST_TEST(arr[Foo::bir] == 1);
	BOOST_TEST(arr[Foo::bor] == 2);
	BOOST_TEST(arr[Foo::bur] == 3);
}

BOOST_AUTO_TEST_CASE(index_range_enum_negative)
{
	indexed_array<int, index_range<Foo::ber, Foo::bor>> arr{1, 2, 3};
	BOOST_TEST(arr.size() == 3);
	BOOST_TEST(arr[Foo::ber] == 1);
	BOOST_TEST(arr[Foo::bir] == 2);
	BOOST_TEST(arr[Foo::bor] == 3);
}

BOOST_AUTO_TEST_CASE(index_range_enum_out_of_bound)
{
	indexed_array<int, index_range<Foo::bar, Foo::bir>> arr{1, 2, 3};
	BOOST_TEST(arr.size() == 3);
	bool catched = false;
	int a = 0;
	try
	{
		a = arr.at(Foo::bur);
	}
	catch (std::out_of_range&)
	{
		catched = true;
	}
	BOOST_TEST(catched);
	BOOST_TEST(a == 0);
}

BOOST_AUTO_TEST_CASE(index_range_enum_empty)
{
	indexed_array<int, index_range<Foo::bar, Foo::bar>> arr{3};
	BOOST_TEST(arr.size() == 1);
	BOOST_TEST(arr[Foo::bar] == 3);
}

BOOST_AUTO_TEST_CASE(index_range_enum_limits)
{
	indexed_array<int, index_range<Foo::bar, Foo::bir>> arr{3};
	bool eq = arr.low_limit() == Foo::bar;
	BOOST_TEST(eq);
	eq = arr.high_limit() == Foo::bir;
	BOOST_TEST(eq);
}

#ifdef INDEXED_ARRAY_HAS_REFLECTION
BOOST_AUTO_TEST_CASE(index_reflection)
{
	using Idx = jbc::indexed_array::detail::default_indexer<Foo>;
	static_assert(Idx::is_o1);
	indexed_array<int, Foo> arr{2, 3, 4, 5, 6};
	BOOST_TEST(arr.size() == 5);
	BOOST_TEST(arr[Foo::bar] == 2);
}
#endif
