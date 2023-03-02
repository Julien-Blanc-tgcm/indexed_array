#define BOOST_TEST_MODULE Indexed Array
#include <boost/describe.hpp>
#include <boost/test/unit_test.hpp>

#include "jbc/indexed_array.hpp"
#include "jbc/indexed_array/safe_numerics.hpp"

using namespace jbc::indexed_array;
using namespace boost::safe_numerics;

BOOST_AUTO_TEST_CASE(safe_range_indexer)
{
	using Base = indexed_array<int, safe_signed_range_indexer<3, 10>>;

	Base b{1, 2, 3, 4, 5, 6, 7, 8};

	BOOST_TEST(b.size() == 8);
	BOOST_TEST(b[4] == 2);
	safe_signed_range<5, 7> j = 6;
	BOOST_TEST(b[j] == 4);
}
